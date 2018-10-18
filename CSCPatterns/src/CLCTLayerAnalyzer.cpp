/*
 * CLCTLayerAnalyzer.cpp
 *
 *  Created on: Oct 18, 2018
 *      Author: wnash
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include <string>
#include <stdio.h>
#include <algorithm>
#include <time.h>

using namespace std;

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

#include "../../CSCDigiTuples/include/CSCInfo.h"
#include "../../CSCDigiTuples/include/CSCHelper.h"


int CLCTLayerAnalyzer(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";


	//
	// SET INPUT BRANCHES
	//

	CSCInfo::Event evt(t);
	CSCInfo::Muons muons(t);
	CSCInfo::Segments segments(t);
	CSCInfo::RecHits recHits(t);
	CSCInfo::LCTs lcts(t);
	CSCInfo::CLCTs clcts(t);
	CSCInfo::Comparators comparators(t);



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}


	vector<TH1F*> clctLayerCount_mePlus;
	vector<TH1F*> real_clctLayerCount_mePlus;
	vector<TH1F*> clctLayerCount_meMinus;
	vector<TH1F*> real_clctLayerCount_meMinus;

	for(unsigned int i =1; i <=36; i++){
		real_clctLayerCount_mePlus.push_back(new TH1F(("h_real_clctLayerCount_me_p_1_1_"+to_string(i)).c_str(), ("ME+1/1/"+to_string(i)+"; Layer Count; Segments").c_str(), 7,0,7));
		clctLayerCount_mePlus.push_back(new TH1F(("h_clctLayerCount_me_p_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_p_1_1_"+to_string(i)+"; Layer Count; Segments").c_str(), 7,0,7));
		real_clctLayerCount_meMinus.push_back(new TH1F(("h_real_clctLayerCount_me_m_1_1_"+to_string(i)).c_str(), ("ME-1/1/"+to_string(i)+"; Layer Count; Segments").c_str(), 7,0,7));
		clctLayerCount_meMinus.push_back(new TH1F(("h_clctLayerCount_me_m_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_m_1_1_"+to_string(i)+"; Layer Count; Segments").c_str(), 7,0,7));
	}


	vector<TH1F*> clctsInChamber_matchedCLCTs;// how many clcts in the chamber with a given matched clct, split by layer count
	vector<TH1F*> clctsInChamber_matchedCLCTs_me11a;// how many clcts in the chamber with a given matched clct, split by layer count
	vector<TH1F*> clctsInChamber_matchedCLCTs_me11b;// how many clcts in the chamber with a given matched clct, split by layer count
	for(unsigned int i =3; i <= 6; i++){
		clctsInChamber_matchedCLCTs.push_back(new TH1F(
				("h_clctsInChamber_"+to_string(i)+"layersCLCTs").c_str(),
				("h_clctsInChamber_"+to_string(i)+"layersCLCTs; CLCTs in Chamber; Segment Matched CLCTs").c_str(),
				2, 1, 3));
		clctsInChamber_matchedCLCTs_me11a.push_back(new TH1F(
						("h_clctsInChamber_"+to_string(i)+"layersCLCTs_me11a").c_str(),
						("h_clctsInChamber_"+to_string(i)+"layersCLCTs_me11a; CLCTs in Chamber; Segment Matched CLCTs").c_str(),
						2, 1, 3));
		clctsInChamber_matchedCLCTs_me11b.push_back(new TH1F(
						("h_clctsInChamber_"+to_string(i)+"layersCLCTs_me11b").c_str(),
						("h_clctsInChamber_"+to_string(i)+"layersCLCTs_me11b; CLCTs in Chamber; Segment Matched CLCTs").c_str(),
						2, 1, 3));
	}


	int EC = 0; // 1-2
	int ST = 0; // 1-4
	int RI = 0; // 1-4
	int CH = 0;
	float segmentX = 0;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	//t->SetImplicitMT(true);


	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		// chamberid, index in clct array - the sorting isn't perfect, since we go in order of segments, so can find a worse match first
		vector<pair<int, unsigned int>> matchedCLCTs;


		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
			int segId = segments.ch_id->at(thisSeg);
			CSCHelper::ChamberId c = CSCHelper::unserialize(segId);

			EC = c.endcap;
			ST = c.station;
			RI = c.ring;
			CH = c.chamber;


			segmentX = segments.pos_x->at(thisSeg); //strips


			// IGNORE SEGMENTS AT THE EDGES OF THE CHAMBERS
			if(segmentX < 1) continue;
			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);
			bool me13 = (ST == 1 && RI == 3);
			if(me11a){
				if(segmentX > 47) continue;
			} else if (me11b || me13) {
				if(segmentX > 63) continue;
			} else {
				if(segmentX > 79) continue;
			}

			//Selecting only CLCTs in these chambers
			if(!(me11b || me11a)) continue;


			//printf("segmentX = %f\n",segmentX);
			for(unsigned int iclct =0; iclct < clcts.size(); iclct++){
				int thisClctId = clcts.ch_id->at(iclct);
				if(thisClctId != segId) continue;

				int closestCLCTtoSegmentIndex = -1;
				float minDistanceSegmentToClosestCLCT = 1e5;
				if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), make_pair(thisClctId, iclct)) != matchedCLCTs.end()) continue;
				float clctStripPos = clcts.halfStrip->at(iclct) / 2. + 16*clcts.CFEB->at(iclct);
				if(me11a) clctStripPos -= 16*4;
				if(abs(clctStripPos - segmentX) < minDistanceSegmentToClosestCLCT) {
					minDistanceSegmentToClosestCLCT = abs(clctStripPos - segmentX);
					closestCLCTtoSegmentIndex = iclct;
				}

				if(closestCLCTtoSegmentIndex != -1){ //if we found one
					//printf("found: %i\n", clctQ->at(iclct).at(closestCLCTtoSegmentIndex));
					matchedCLCTs.push_back(make_pair(thisClctId, (unsigned int)closestCLCTtoSegmentIndex));
					if(EC == 1){
						real_clctLayerCount_mePlus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
					}else if (EC == 2){
						real_clctLayerCount_meMinus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
					}

				}

			}

		}
	}


	unsigned int p_entries = 0;
	unsigned int m_entries = 0;
	for(unsigned int i = 1; i <= 36; i++) {

		TH1F* p_hist = real_clctLayerCount_mePlus.at(i-1);
		p_hist->Write();
		p_entries += p_hist->GetBinContent(4);
		TH1F* m_hist = real_clctLayerCount_meMinus.at(i-1);
		m_hist->Write();
		m_entries += m_hist->GetBinContent(4);
	}
	TH1F* me_plus_11_3lay_clct_mult = new TH1F("h_me_plus_11_3lay_clct_mult","h_me_plus_11_3lay_clct_mult; Chamber; 3Layer CLCTs", 36, 1,37);
	TH1F* me_minus_11_3lay_clct_mult = new TH1F("h_me_minus_11_3lay_clct_mult","h_me_minus_11_3lay_clct_mult; Chamber; Matched 3Layer CLCTs", 36, 1,37);

	me_plus_11_3lay_clct_mult->SetEntries(p_entries);
	me_plus_11_3lay_clct_mult->Write();
	me_minus_11_3lay_clct_mult->SetEntries(m_entries);
	me_minus_11_3lay_clct_mult->Write();


	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;
	return 0;
}

int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return CLCTLayerAnalyzer(string(argv[1]), string(argv[2]));
		case 4:
			return CLCTLayerAnalyzer(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return CLCTLayerAnalyzer(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./PatternFinder inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}


