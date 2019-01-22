/*
 * PatternFinder.cpp
 *
 *  Created on: Nov. 7 2018
 *      Author: root
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;


/* Calculates probability of a muon given a comparator code.
 * See slides: https://indico.cern.ch/event/744948/
 */

int MultiplicityStudy(string inputfile, string outputfile, int start=0, int end=-1) {

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
	CSCInfo::GenParticles gen(t);

	//
	// MAKE ALL THE PATTERNS
	//


	vector<CSCPattern>* newPatterns = createNewPatterns();

	//
	// Map to look at probability
	// TODO: incorporate this functionality into LUT class once it is more figured out
	//
	//LUT bayesLUT("bayes", LINEFIT_LUT_PATH);

	//
	// OUTPUT TREE
	//

	//x = ring, y = stat
	//TH2F* multiplicityByChamber = new TH2F("multByCham", "mumltByCham; Ring; Station", 4, 1,5, 3, 1,4);


	const int nbins = 10;
	float binVals[nbins+1] = {1,2,3,4,5,6,7,8,9,10,15};
	vector<TH1F*> multiplicities;
	for(auto& name : CHAMBER_NAMES){
		//TH1F* cham = new TH1F(name.c_str(), (name+"; CLCT Multiplicity; CLCTs").c_str(), 15, 1,16);
		TH1F* cham = new TH1F(name.c_str(), ("h_"+name+"; CLCT Multiplicity; CLCTs").c_str(),nbins, binVals);
		multiplicities.push_back(cham);
	}
	vector<TH1F*> clctRMS;
	for(auto& bin : binVals){
		string name = "h_rmsMult_" + to_string(round(bin));
		TH1F* rms = new TH1F(name.c_str(), (name+"; Position From Mean [strips]; CLCTs").c_str(), 40, -20, 20);
		clctRMS.push_back(rms);
	}

	TH1F* h_nChambersShowered = new TH1F("h_nChambersShowered", "h_nChambersShowered; Amount of Showering (>1 CLCT) Chambers In Event; Events", 20, 0, 20);
	TH2F* h_multiplicityVsPt = new TH2F("h_multiplicityVsPt", "h_multiplicityVsPt; Pt [GeV]; CLCT Multiplicity",  40, 0, 400,10, 1, 11);
	TH2F* h_multiplicityVsP = new TH2F("h_multiplicityVsP", "h_multiplicityVsP; P [GeV]; CLCT Multiplicity",  80, 0, 800,10, 1, 11);

	//const unsigned int MAX_CHAMBERS = 4; //max overlapping chambers
	TH2F* h_energyPerChamberVsP = new TH2F("h_energyPerChamberVsP","h_energyPerChamberVsP; P [GeV]; Energy Per Chamber [?]", 80,0.,800., 100, -100000., -2.);
	map<int, TH2F*> h_energyPerChamberVsP_byChamber;
	map<int, TH2F*> h_compHitsPerChamberVsGenP_byChamber;
	//map<int, TH2F*> h_compHitsPerChamberVsGenP_byChamber;
	for(int chamberCount =  1; chamberCount < 7; chamberCount++){
		h_energyPerChamberVsP_byChamber[chamberCount] = new TH2F(("h_energyPerChamberVsP_"+to_string(chamberCount)).c_str(),
				("h_energyPerChamberVsP"+to_string(chamberCount) +"; P [GeV]; Energy Per Chamber [?]").c_str(), 80,0.,800., 100, -100000., -2.);
		h_compHitsPerChamberVsGenP_byChamber[chamberCount] = new TH2F(("h_compHitsPerChamberVsGenP_"+to_string(chamberCount)).c_str(),
				("h_compHitsPerChamberVsGenP"+to_string(chamberCount) +"; Gen P [GeV]; Comparator Hits").c_str(),60 ,100.,6000., 20, 0, 100);
	}

	//take slices of p, of size 100 GeV
	map<double, TH1F*> h_compHitsPSlices_4chambers;
	map<double, TH1F*> h_compHitsPSlices_4chambers_perChamber; //how many hits each chamber has individually
	vector<double> p_slices;
	for(double dist_p = 0; dist_p <= 2000; dist_p += 500){
		p_slices.push_back(dist_p);
		h_compHitsPSlices_4chambers[dist_p] = new TH1F(("h_compHitsPSlice_4chambers_"+to_string((int)round(dist_p))).c_str(),
				("h_compHitsPSlice_4chambers_"+to_string((int)round(dist_p))).c_str(), 100,0,100);
		h_compHitsPSlices_4chambers_perChamber[dist_p] = new TH1F(("h_compHitsPSlice_4chambers_perChamber_"+to_string((int)round(dist_p))).c_str(),
						("h_compHitsPSlice_4chambers_perChamber_"+to_string((int)round(dist_p))).c_str(), 100,0,100);
	}


	//int patternId = 0;
	//int ccId = 0;
	//int layers = 0;
	//float closestCLCT
	//bool matchedSegment = false;
	//int nMultiplicity = 0;

	/*
	TTree* outTree = new TTree("clctTree", "TTree for analysis of probability if a CLCT will result in a real muon or not");
	outTree->Branch("patternId", &patternId, "patternId/I");
	outTree->Branch("ccId", &ccId, "ccId/I");
	outTree->Branch("matchedSegment", &matchedSegment);
	outTree->Branch("nMultiplicity", &nMultiplicity, "nMultiplicity/I");
	*/



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	//TH1F* h_clctPatterns_real = new TH1F("h_clctPatterns_real","Recorded CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);
	//TH1F* h_clctPatterns_emulated = new TH1F("h_clctPatterns_emulated","Emulated CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);

	//
	// TREE ITERATION
	//


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		float genP = 0;



		//cout << "-- Printing Gen Information --" << endl;
		if(gen.size() > 1){
			cout << "Error more than one gen particle. quitting..." << endl;
			continue;
		}

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			//cout << "Gen ID: " << gen.pdg_id->at(ig) << " pt = " << gen.pt->at(ig) << endl;
			double pt = gen.pt->at(ig);
			double eta = gen.eta->at(ig);
			double theta = 2.*TMath::ATan(TMath::Exp(-eta));
			if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);
		}



		//iterate through muons
		for(int m =0; m < (int)muons.size(); m++){
			float totalEnergy = 0;
			vector<int> chambers; //all the chambers used to make this muon
			for(unsigned int rh =0; rh < recHits.size(); rh++){
				if(recHits.mu_id->at(rh) != m) continue; //only look at rh associated with one muon at a time
				int chId = recHits.ch_id->at(rh);
				//keep track of how many chambers we have looked at
				if(find(chambers.begin(), chambers.end(), chId) == chambers.end()){
					chambers.push_back(chId);
				}

			}
			if(chambers.size()) {
				for(unsigned int rh=0; rh < recHits.size(); rh++){
					int chId = recHits.ch_id->at(rh);
					//add all energy in all the chambers where a segment was tagged
					//if(find(chambers.begin(), chambers.end(), chId) == chambers.end()){
					if(find(chambers.begin(), chambers.end(), chId) != chambers.end()){
						totalEnergy += recHits.e->at(rh);
					}
				}


				float averageEnergy = totalEnergy / chambers.size();
				double pt = muons.pt->at(segments.mu_id->at(m));
				double eta = muons.eta->at(segments.mu_id->at(m));
				double theta = 2.*TMath::ATan(TMath::Exp(-eta));
				double p = -1; //default to -1 in case that theta is zero (somehow)
				if(TMath::Sin(theta)) p = pt/TMath::Sin(theta);

				h_energyPerChamberVsP->Fill(p, averageEnergy);
				if(chambers.size() < 6){
					h_energyPerChamberVsP_byChamber[chambers.size()]->Fill(p,averageEnergy);
				}

			}
		}


		int nShoweringChambers = 0;
		int nChambersWithHits = 0;
		int nCompHits = 0;
		vector<int> compHitsPerChamber;

		//
		//Iterate through all possible chambers
		//
		for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++){
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;


			//
			// Count all the segments in this chamber
			//
			vector<float> segmentPts;
			vector<float> segmentPs;
			vector<float> segmentPos;
			//if(segments.size())cout << "=== New Chamber ===" << endl;
			for(unsigned int thisSeg =0; thisSeg < segments.size(); thisSeg++){
				int segmentHash = segments.ch_id->at(thisSeg);
				if(segmentHash != chamberHash) continue;
				double pt = muons.pt->at(segments.mu_id->at(thisSeg));
				double eta = muons.eta->at(segments.mu_id->at(thisSeg));
				segmentPts.push_back(pt);
				/* From: https://en.wikipedia.org/wiki/Pseudorapidity
				 *
				 */
				double theta = 2.*TMath::ATan(TMath::Exp(-eta));
				double p = -1; //default to -1 in case that theta is zero (somehow)
				if(TMath::Sin(theta)) p = pt/TMath::Sin(theta);
				//float p = pt
				/*
				cout << "thisSeg = " << thisSeg
						<< " pt = " << pt
						<< " eta = " << eta
						<< " theta = " << theta
						<< " p = " << p << endl;
				*/
				segmentPs.push_back(p);
				segmentPos.push_back(segments.pos_x->at(thisSeg));
			}

			//
			// Emulate the TMB to find all the CLCTs
			//

			ChamberHits chamberCompHits(ST, RI, EC, CH);

			if(chamberCompHits.fill(comparators)) return -1;

			ChamberHits chamberRecHits(ST,RI,EC,CH);

			if(chamberRecHits.fill(recHits)) return -1;

			vector<CLCTCandidate*> eclcts;





			//get all the clcts in the chamber
			if(searchForMatch(chamberCompHits, newPatterns,eclcts)){
				//cout << "Search for match failed!" << endl;
				eclcts.clear();
				continue;
			}

			/*
			if(chamberCompHits.nhits()){
				cout << "-- Found " << eclcts.size() << " matches" << endl;
				chamberCompHits.print();
			}
			*/
			/*
			if(chamberRecHits.nhits() && eclcts.size() > 1){
				chamberCompHits.print();
				chamberRecHits.print();
			}
			*/

			if(segmentPts.size()) {

				float normalizedCLCTs = 1.*eclcts.size()/segmentPts.size();
				//cout << "eclcts: " << eclcts.size() << " segments: " << segmentPts.size() << " normalizedCLCTs: " << normalizedCLCTs << endl;
				//bool testStation = (ST ==2) && (RI==1);

				/*
				if(normalizedCLCTs > 1  && testStation){
					cout << "segment [Pos (strips), Pt, P]" << endl;
					for(unsigned int iseg = 0; iseg < segmentPts.size(); iseg++){
						cout << "\t[ " << segmentPos.at(iseg) << ", " << segmentPts.at(iseg) << ", " << segmentPs.at(iseg) << "]" << endl;
					}
					for(auto& clct : eclcts){
						cout << "CLCT Position [strips]: " << clct->keyStrip() << endl;
					}
					chamberCompHits.print();
				}
				*/
				for(auto& pt: segmentPts){
					h_multiplicityVsPt->Fill( pt,normalizedCLCTs);
				}
				for(auto& p : segmentPs){
					h_multiplicityVsP->Fill(p, normalizedCLCTs);
				}


				//calculate position distribution of clcts, taking a simple case of a single segment to start with
				int nclcts = eclcts.size();
				if(nclcts){
					float meanPos = 0;
					for(auto& eclct: eclcts){
						meanPos += eclct->keyStrip();
					}
					meanPos /= nclcts;
					for(auto& eclct: eclcts){
						float diff = eclct->keyStrip() - meanPos;
						if(nclcts > 10){
							clctRMS.back()->Fill(diff);
						}else {
							clctRMS.at(nclcts-1)->Fill(diff);
						}
					}
				}
				if(nclcts > 2){
					nShoweringChambers++;
				}
			}



			if(!eclcts.size()) continue;

			for(unsigned int c =0; c < NCHAMBERS; c++){
				auto& st_ri = CHAMBER_ST_RI[c];
				//this is the chamber we are in right now
				if(ST == st_ri[0] && RI == st_ri[1]){
					if(eclcts.size() > binVals[nbins]) multiplicities.at(c)->Fill(binVals[nbins-1]);//fill overflow in earlier bin
					else multiplicities.at(c)->Fill(eclcts.size());
					break;
				}

			}

			eclcts.clear();
		}
		if(nChambersWithHits && nChambersWithHits < 7){
			h_compHitsPerChamberVsGenP_byChamber[nChambersWithHits]->Fill(genP, nCompHits);
			if(nChambersWithHits == 4){
				for(auto& p : p_slices){
					if(genP > p && genP <= p + 100){
						h_compHitsPSlices_4chambers[p]->Fill(nCompHits);
						for(auto hits: compHitsPerChamber){
							h_compHitsPSlices_4chambers_perChamber[p]->Fill(hits);
						}
					}
				}
			}
		}
		h_nChambersShowered->Fill(nShoweringChambers);


	}
	outF->cd();
	h_nChambersShowered->Write();
	for(auto& entry : h_energyPerChamberVsP_byChamber) entry.second->Write();
	h_energyPerChamberVsP->Write();
	TH2F* h_energyPerChamberVsP_normalized = (TH2F*)h_energyPerChamberVsP->Clone("h_energyPerChamberVsP_norm");
	for(int i=0; i < h_energyPerChamberVsP_normalized->GetNbinsX()+1; i++){
		float norm = 0;
		//calculate integral along pt bin
		for(int j=0;j < h_energyPerChamberVsP_normalized->GetNbinsY()+1; j++){
			norm += h_energyPerChamberVsP_normalized->GetBinContent(i,j);
		}
		//normalize everything for a given pt
		if(norm){
			for(int j=0;j < h_energyPerChamberVsP_normalized->GetNbinsY()+1; j++){
				float content = h_energyPerChamberVsP_normalized->GetBinContent(i,j);
				h_energyPerChamberVsP_normalized->SetBinContent(i,j,content/norm);
				//norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
			}
		}
	}
	h_energyPerChamberVsP_normalized->Write();

	for (auto& hist : multiplicities) {
		//double norm = hist->GetEntries();
		//if(norm) hist->Scale(1./norm);
		hist->Write();
	}
	for(auto& entry : h_compHitsPSlices_4chambers) {
		entry.second->Scale(1./entry.second->GetEntries());
		entry.second->Write();
	}
	for(auto& entry: h_compHitsPSlices_4chambers_perChamber) {
		entry.second->Scale(1./entry.second->GetEntries());
		entry.second->Write();
	}

	for(auto& hist: clctRMS) hist->Write();
	h_multiplicityVsPt->Write();
	h_multiplicityVsP->Write();

	TH2F* h_multiplicityVsPt_normalized = (TH2F*)h_multiplicityVsPt->Clone("h_multiplicityVsPt_norm");
	for(int i=0; i < h_multiplicityVsPt_normalized->GetNbinsX()+1; i++){
		float norm = 0;
		//calculate integral along pt bin
		for(int j=0;j < h_multiplicityVsPt_normalized->GetNbinsY()+1; j++){
			norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
		}
		//normalize everything for a given pt
		if(norm){
			for(int j=0;j < h_multiplicityVsPt_normalized->GetNbinsY()+1; j++){
				float content = h_multiplicityVsPt_normalized->GetBinContent(i,j);
				h_multiplicityVsPt_normalized->SetBinContent(i,j,content/norm);
				//norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
			}
		}
	}
	h_multiplicityVsPt_normalized->Write();
	h_multiplicityVsPt_normalized->GetBinCenter(1);
	for(auto& h : h_compHitsPerChamberVsGenP_byChamber){
		h.second->Write();
	}

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
			return MultiplicityStudy(string(argv[1]), string(argv[2]));
		case 4:
			return MultiplicityStudy(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return MultiplicityStudy(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







