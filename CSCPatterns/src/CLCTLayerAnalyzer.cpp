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

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"


//quick and dirty hack, sorry later me
/*
int matchCLCTs(const CSCInfo::CLCTs& clcts, const int segId, const float segmentX, vector<pair<int, unsigned int>>& matchedCLCTs,
		vector<TH1F*>& clctLayerCount_mePlus,
		vector<TH1F*>& clctLayerCount_meMinus,
		TH1F* clctLayerCount_mep11a_11,
		TH1F* clctLayerCount_mep11b_11,
		TH1F* clctLayerCount_mem11a_11,
		TH1F* clctLayerCount_mem11b_11,
		TH1F* mep11a_11_Pt,
		TH1F* mep11a_11_3Lay_Pt,
		TH1F* mep11b_11_Pt,
		TH1F* mep11b_11_3Lay_Pt,
		const CSCInfo::Muons& muons,
		const CSCInfo::Segments& segments,
		unsigned int thisSeg) {
	CSCHelper::ChamberId c = CSCHelper::unserialize(segId);

	int EC = c.endcap;
	int ST = c.station;
	int RI = c.ring;
	int CH = c.chamber;

	bool me11a = (ST == 1 && RI == 4);
	bool me11b = (ST == 1 && RI == 1);
	bool me13 = (ST == 1 && RI == 3);

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
				clctLayerCount_mePlus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
				if(CH == 11){
					if(me11a){
						clctLayerCount_mep11a_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
						mep11a_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
						if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11a_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
					}
					if(me11b){
						clctLayerCount_mep11b_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
						mep11b_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
						if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11b_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));

					}

				}
			}else if (EC == 2){
				clctLayerCount_meMinus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
				if(CH == 11){
					if(me11a){
						clctLayerCount_mem11a_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
					}
					if(me11b){
						clctLayerCount_mem11b_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
					}

				}
			}
		}

	}
	return 0;
}
*/



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
	vector<TH1F*> clctLayerCount_meMinus;

	//for(unsigned int i =1; i <=36; i++){
	for(unsigned int i =1; i <=12; i++){
		clctLayerCount_mePlus.push_back(new TH1F(("h_clctLayerCount_me_p_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_p_1_1_"+to_string(i)+"; Layer Count; Matched CLCTs").c_str(), 7,0,7));
		clctLayerCount_meMinus.push_back(new TH1F(("h_clctLayerCount_me_m_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_m_1_1_"+to_string(i)+"; Layer Count; Matched CLCTs").c_str(), 7,0,7));
	}

	TH1F* clctLayerCount_mep11a_11 = new TH1F("h_clctLayerCount_me_p11a11","h_clctLayerCount_me_p11a11; Layer Count; Matched CLCTs", 7,0,7);
	TH1F* clctLayerCount_mep11b_11 = new TH1F("h_clctLayerCount_me_p11b11","h_clctLayerCount_me_p11b11; Layer Count; Matched CLCTs", 7,0,7);

	TH1F* clctLayerCount_mem11a_11 = new TH1F("h_clctLayerCount_me_m11a11","h_clctLayerCount_me_m11a11; Layer Count; Matched CLCTs", 7,0,7);
	TH1F* clctLayerCount_mem11b_11 = new TH1F("h_clctLayerCount_me_m11b11","h_clctLayerCount_me_m11b11; Layer Count; Matched CLCTs", 7,0,7);


	TH1F* mep11a_11_Pt = new TH1F("h_mep11a_11_Pt","h_mep11a_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11a_11_3Lay_Pt = new TH1F("h_mep11a_3Lay_11_Pt","h_mep11a_3Lay_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11b_11_Pt = new TH1F("h_mep11b_11_Pt","h_mep11b_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11b_11_3Lay_Pt = new TH1F("h_mep11b_3Lay_11_Pt","h_mep11b_3Lay_11_Pt; Pt; CLCTs",14,0,70);

	TH1F* lctLayerCount_mep11a_11 = new TH1F("h_lctLayerCount_me_p11a11","h_lctLayerCount_me_p11a11; Layer Count; Matched LCTs", 12,0,12);
	TH1F* lctLayerCount_mep11b_11 = new TH1F("h_lctLayerCount_me_p11b11","h_lctLayerCount_me_p11b11; Layer Count; Matched LCTs", 12,0,12);

	TH1F* lctLayerCount_mem11a_11 = new TH1F("h_lctLayerCount_me_m11a11","h_lctLayerCount_me_m11a11; Layer Count; Matched LCTs", 12,0,12);
	TH1F* lctLayerCount_mem11b_11 = new TH1F("h_lctLayerCount_me_m11b11","h_lctLayerCount_me_m11b11; Layer Count; Matched LCTs", 12,0,12);

/*
	TH1F* mep11a_11_Pt = new TH1F("h_mep11a_11_Pt","h_mep11a_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11a_11_3Lay_Pt = new TH1F("h_mep11a_3Lay_11_Pt","h_mep11a_3Lay_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11b_11_Pt = new TH1F("h_mep11b_11_Pt","h_mep11b_11_Pt; Pt; CLCTs",14,0,70);
	TH1F* mep11b_11_3Lay_Pt = new TH1F("h_mep11b_3Lay_11_Pt","h_mep11b_3Lay_11_Pt; Pt; CLCTs",14,0,70);
*/

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
		//
		// Oct. 29 Skipping all events past new firmware update
		//
		if(evt.RunNumber > 323362) continue;


		// chamberid, index in clct array - the sorting isn't perfect, since we go in order of segments, so can find a worse match first
		vector<pair<int, unsigned int>> matchedCLCTs;
		vector<pair<int, unsigned int>> matchedLCTs;


		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
			int segId = segments.ch_id->at(thisSeg);
			CSCHelper::ChamberId c = CSCHelper::unserialize(segId);

			EC = c.endcap;
			ST = c.station;
			RI = c.ring;
			CH = c.chamber;

			//
			// Cut to look at only Pt > 25
			//Remove after rerunning patternextractor, done now so I don't have to wait for crab to finish again

			if(muons.pt->at(segments.mu_id->at(thisSeg)) < 25) continue;


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

			//
			// MATCH CLCTS
			//
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
						clctLayerCount_mePlus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
						if(CH == 11){
							if(me11a){
								clctLayerCount_mep11a_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
								mep11a_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11a_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
							}
							if(me11b){
								clctLayerCount_mep11b_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
								mep11b_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11b_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));

							}

						}
					}else if (EC == 2){
						clctLayerCount_meMinus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
						if(CH == 11){
							if(me11a){
								clctLayerCount_mem11a_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
							}
							if(me11b){
								clctLayerCount_mem11b_11->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
							}

						}
					}
				}

			}

			//MATCH LCTS

			for(unsigned int ilct =0; ilct < lcts.size(); ilct++){
				int thisLctId = clcts.ch_id->at(ilct);
				if(thisLctId != segId) continue;
				int closestLCTtoSegmentIndex = -1;
				float minDistanceSegmentToClosestLCT = 1e5;
				if(std::find(matchedLCTs.begin(), matchedLCTs.end(), make_pair(thisLctId, ilct)) != matchedLCTs.end()) continue;
				float lctStripPos = lcts.keyHalfStrip->at(ilct)/2.;
				if(me11a) lctStripPos -= 16*4;
				if(abs(lctStripPos - segmentX) < minDistanceSegmentToClosestLCT) {
					minDistanceSegmentToClosestLCT = abs(lctStripPos - segmentX);
					closestLCTtoSegmentIndex = ilct;
				}
				if(closestLCTtoSegmentIndex != -1){ //if we found one
					//printf("found: %i\n", clctQ->at(iclct).at(closestCLCTtoSegmentIndex));
					matchedLCTs.push_back(make_pair(thisLctId, (unsigned int)closestLCTtoSegmentIndex));
					if(EC == 1){
						//lctLayerCount_mePlus.at(CH-1)->Fill(clcts.quality->at(closestLCTtoSegmentIndex));
						if(CH == 11){
							if(me11a){
								lctLayerCount_mep11a_11->Fill(lcts.quality->at(closestLCTtoSegmentIndex));
								//mep11a_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								//if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11a_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
							}
							if(me11b){
								lctLayerCount_mep11b_11->Fill(lcts.quality->at(closestLCTtoSegmentIndex));
								//mep11b_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								//if(clcts.quality->at(closestCLCTtoSegmentIndex) == 3) mep11b_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));

							}

						}
					}else if (EC == 2){
						//clctLayerCount_meMinus.at(CH-1)->Fill(clcts.quality->at(closestCLCTtoSegmentIndex));
						if(CH == 11){
							if(me11a){
								lctLayerCount_mem11a_11->Fill(lcts.quality->at(closestLCTtoSegmentIndex));
							}
							if(me11b){
								lctLayerCount_mem11b_11->Fill(lcts.quality->at(closestLCTtoSegmentIndex));
							}

						}
					}
				}
			}

		}
	}


	unsigned int p_entries = 0;
	unsigned int m_entries = 0;
	for(unsigned int i = 1; i <= 36; i++) {

		TH1F* p_hist = clctLayerCount_mePlus.at(i-1);
		p_hist->Write();
		p_entries += p_hist->GetBinContent(4);
		TH1F* m_hist = clctLayerCount_meMinus.at(i-1);
		m_hist->Write();
		m_entries += m_hist->GetBinContent(4);
	}

	clctLayerCount_mep11a_11->Write();
	clctLayerCount_mep11b_11->Write();
	clctLayerCount_mem11a_11->Write();
	clctLayerCount_mem11b_11->Write();
	mep11a_11_Pt->Write();
	mep11a_11_3Lay_Pt->Write();
	mep11b_11_Pt->Write();
	mep11b_11_3Lay_Pt->Write();

	TH1F* me_plus_11_3lay_clct_mult = new TH1F("h_me_plus_11_3lay_clct_mult","h_me_plus_11_3lay_clct_mult; Chamber; 3Layer CLCTs", 36, 1,37);
	TH1F* me_minus_11_3lay_clct_mult = new TH1F("h_me_minus_11_3lay_clct_mult","h_me_minus_11_3lay_clct_mult; Chamber; Matched 3Layer CLCTs", 36, 1,37);

	me_plus_11_3lay_clct_mult->SetEntries(p_entries);
	me_plus_11_3lay_clct_mult->Write();
	me_minus_11_3lay_clct_mult->SetEntries(m_entries);
	me_minus_11_3lay_clct_mult->Write();

	lctLayerCount_mep11a_11->Write();
	lctLayerCount_mep11b_11->Write();
	lctLayerCount_mem11a_11->Write();
	lctLayerCount_mem11b_11->Write();


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


