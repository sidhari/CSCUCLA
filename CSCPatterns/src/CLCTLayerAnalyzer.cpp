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


//looks at a map of histograms, if it contains the key, fills the corresponding histogram with "histValue"
void fillHist(map<unsigned int, TH1F*> hists, unsigned int key, float histValue){
	//look to see if we care about this chamber
	auto it = hists.find(key);
	if(it != hists.end()){
		//fill the correct histogram
		it->second->Fill(histValue);
	}
}


vector<pair<int,int>> RING_ENDCAP_PERMUTATIONS{make_pair(4,1), make_pair(1,1), make_pair(4,2), make_pair(1,2)};

map<unsigned int, TH1F*> makeHistPermutation(string name, string title, unsigned int bins, unsigned int low, unsigned int high){
	TH1F* h_me11a_plus = new TH1F((name+"_me_p11a_11").c_str(),("me_p11a_11 " +title).c_str(),bins,low,high);
	TH1F* h_me11b_plus = new TH1F((name+"_me_p11b_11").c_str(),("me_p11b_11" +title).c_str(),bins,low,high);
	TH1F* h_me11a_minus = new TH1F((name+"_me_m11a_11").c_str(),("me_m11a_11 "+title).c_str(),bins,low,high);
	TH1F* h_me11b_minus = new TH1F((name+"_me_m11b_11").c_str(),("me_m11b_11" +title).c_str(),bins,low,high);

	unsigned int me_p11a11 = CSCHelper::serialize(1,4,11,1);
	unsigned int me_p11b11 = CSCHelper::serialize(1,1,11,1);
	unsigned int me_m11a11 = CSCHelper::serialize(1,4,11,2);
	unsigned int me_m11b11 = CSCHelper::serialize(1,1,11,2);

	map<unsigned int, TH1F*> hists;
	hists[me_p11a11] = h_me11a_plus;
	hists[me_p11b11] = h_me11b_plus;
	hists[me_m11a11] = h_me11a_minus;
	hists[me_m11b11] = h_me11b_minus;
	return hists;
}


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

	for(unsigned int i =1; i <=36; i++){
	//for(unsigned int i =1; i <=12; i++){
		clctLayerCount_mePlus.push_back(new TH1F(("h_clctLayerCount_me_p_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_p_1_1_"+to_string(i)+"; Layer Count; Matched CLCTs").c_str(), 7,0,7));
		clctLayerCount_meMinus.push_back(new TH1F(("h_clctLayerCount_me_m_1_1_"+to_string(i)).c_str(), ("h_clctLayerCount_me_m_1_1_"+to_string(i)+"; Layer Count; Matched CLCTs").c_str(), 7,0,7));
	}

/*TODO: make each histogram follow the framework of makehistpermutation, which will make it so you easily write the correct histograms, etc
 *
 */
	map<unsigned int,TH1F*> clctLayerCounts = makeHistPermutation("h_clctLayerCount", "h_clctLayerCount;Layer Count; Matched CLCTs",7,0,7);
	map<unsigned int,TH1F*> unmatchedClctLayerCounts = makeHistPermutation("h_unmatchedClctLayerCount", "h_unmatchedClctLayerCount;Layer Count; Unmatched CLCTs",7,0,7);

/*
	//all segments in the chamber with the clct layer threshold changed
	map<unsigned int,TH1F*> clctEff_den = makeHistPermutation("h_clctEff_den", "h_clctEff_den; Pt [GeV]; Efficiency", 15, 20,100);
	//if the chamber associated with the segment has any clct in it
	map<unsigned int,TH1F*> clctEff_hasClct = makeHistPermutation("h_clctEff_hasClct", "h_clctEff_hasClct; Pt [GeV]; Efficiency", 15, 20,100);
	//if the chamber has a 3 layer clct in it
	map<unsigned int,TH1F*>clctEff_has3LayClct = makeHistPermutation("h_clctEff_has3layClct", "h_clctEff_has3LayClct; Pt [GeV]; Efficiency", 15, 20,100);
	//if the 3 layer clct is the closest to the segment
	map<unsigned int,TH1F*> clctEff_3LayClct = makeHistPermutation("h_clctEff_3LayClct", "h_clctEff_3LayClct; Pt [GeV]; Efficiency", 15, 20,100);
*/

	TH1F* clctLayerCount_mep11a_11 = new TH1F("h_clctLayerCount_me_p11a11","h_clctLayerCount_me_p11a11; Layer Count; Matched CLCTs", 7,0,7);
	TH1F* clctLayerCount_mep11b_11 = new TH1F("h_clctLayerCount_me_p11b11","h_clctLayerCount_me_p11b11; Layer Count; Matched CLCTs", 7,0,7);

	TH1F* clctLayerCount_mem11a_11 = new TH1F("h_clctLayerCount_me_m11a11","h_clctLayerCount_me_m11a11; Layer Count; Matched CLCTs", 7,0,7);
	TH1F* clctLayerCount_mem11b_11 = new TH1F("h_clctLayerCount_me_m11b11","h_clctLayerCount_me_m11b11; Layer Count; Matched CLCTs", 7,0,7);


	TH1F* unmatched_clctLayerCount_mep11a_11 = new TH1F("h_unmatched_clctLayerCount_me_p11a11","h_unmatched_clctLayerCount_me_p11a11; Layer Count; Matched CLCTs", 7,0,7);
	TH1F* unmatched_clctLayerCount_mep11b_11 = new TH1F("h_unmatched_clctLayerCount_me_p11b11","h_unmatched_clctLayerCount_me_p11b11; Layer Count; Matched CLCTs", 7,0,7);

	TH1F* me11_clctMult = new TH1F("h_me11_clctMult","h_me11_clctMult", 8,0,8);
	TH1F* me11_3lay_clctMult = new TH1F("h_me11_clctMult","h_me11_clctMult", 8,0,8);

	TH1F* mep11a_11_Pt = new TH1F("h_mep11a_11_Pt","h_mep11a_11_Pt; Pt; CLCTs",18,0,90);
	TH1F* mep11a_11_3Lay_Pt = new TH1F("h_mep11a_3Lay_11_Pt","h_mep11a_3Lay_11_Pt; Pt; CLCTs",18,0,90);
	TH1F* mep11b_11_Pt = new TH1F("h_mep11b_11_Pt","h_mep11b_11_Pt; Pt; CLCTs",18,0,90);
	TH1F* mep11b_11_3Lay_Pt = new TH1F("h_mep11b_3Lay_11_Pt","h_mep11b_3Lay_11_Pt; Pt; CLCTs",18,0,90);

	TH1F* lctLayerCount_mep11a_11 = new TH1F("h_lctLayerCount_me_p11a11","h_lctLayerCount_me_p11a11; Layer Count; Matched LCTs", 16,0,16);
	TH1F* lctLayerCount_mep11b_11 = new TH1F("h_lctLayerCount_me_p11b11","h_lctLayerCount_me_p11b11; Layer Count; Matched LCTs", 16,0,16);

	TH1F* lctLayerCount_mem11a_11 = new TH1F("h_lctLayerCount_me_m11a11","h_lctLayerCount_me_m11a11; Layer Count; Matched LCTs", 16,0,16);
	TH1F* lctLayerCount_mem11b_11 = new TH1F("h_lctLayerCount_me_m11b11","h_lctLayerCount_me_m11b11; Layer Count; Matched LCTs", 16,0,16);



	//all segments in the chamber with the clct layer threshold changed
	TH1F* clctEff_den = new TH1F("h_clctEff_den", "h_clctEff_den; Pt [GeV]; Efficiency", 15, 20,100);
	//if the chamber associated with the segment has any clct in it
	TH1F* clctEff_hasClct = new TH1F("h_clctEff_hasClct", "h_clctEff_hasClct; Pt [GeV]; Efficiency", 15, 20,100);
	//if the chamber has a 3 layer clct in it
	TH1F* clctEff_has3LayClct = new TH1F("h_clctEff_has3layClct", "h_clctEff_has3LayClct; Pt [GeV]; Efficiency", 15, 20,100);
	//if the 3 layer clct is the closest to the segment
	TH1F* clctEff_3LayClct = new TH1F("h_clctEff_3LayClct", "h_clctEff_3LayClct; Pt [GeV]; Efficiency", 15, 20,100);


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
		//if(evt.RunNumber <= 323362) continue;
		if(evt.RunNumber > 323362) continue;


		// chamberid, index in clct array - the sorting isn't perfect, since we go in order of segments, so can find a worse match first
		//vector<pair<int, unsigned int>> matchedCLCTs;
		vector< unsigned int> matchedCLCTs;
		//vector<pair<int, unsigned int>> matchedLCTs;
		vector<unsigned int> matchedLCTs;




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

			//if(muons.pt->at(segments.mu_id->at(thisSeg)) < 25) continue;


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

			if(!(me11b || me11a)) continue;

			/*
			float Pt = muons.pt->at(segments.mu_id->at(thisSeg));

			int closestCLCTtoSegmentIndex = -1;
			float minDistanceSegmentToClosestCLCT = 1e5;
			//iterate through clcts
			for(unsigned int iclct =0; iclct < clcts.size(); iclct++){
				int thisClctId = clcts.ch_id->at(iclct);
				if(thisClctId != segId) continue;
				unsigned int qual = clcts.quality->at(iclct);
				if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), iclct) != matchedCLCTs.end()) continue;
				float clctStripPos = clcts.halfStrip->at(iclct) / 2. + 16*clcts.CFEB->at(iclct);
				if(me11a) clctStripPos -= 16*4;
				if(abs(clctStripPos - segmentX) < minDistanceSegmentToClosestCLCT) {
					minDistanceSegmentToClosestCLCT = abs(clctStripPos - segmentX);
					closestCLCTtoSegmentIndex = iclct;
				}

				//now iterate through the types of chambers we want to look at
				for(unsigned int iperm = 0; iperm < RING_ENDCAP_PERMUTATIONS.size(); iperm++){
					const auto& pair = RING_ENDCAP_PERMUTATIONS.at(iperm);
					if(RI != pair.first || EC != pair.second) continue;


				}
			}
			*/

			bool inUpdatedChamber = (CH == 11 && EC == 1);

			//Selecting only CLCTs in these chambers
			float Pt = muons.pt->at(segments.mu_id->at(thisSeg));
			//fillHist(clctEff_den, segId, Pt);

			if(inUpdatedChamber)clctEff_den->Fill(Pt);
			bool foundCLCT = false; //if we found a clct in the same chamber as the segment
			bool found3LayCLCT = false;
			bool matched3LayCLCT = false;

			//ChamberHits theseCompHits(1, ST, RI, EC, CH);

			//if(fillCompHits(theseCompHits, comparators)) return -1;

			int closestCLCTtoSegmentIndex = -1;
			float minDistanceSegmentToClosestCLCT = 1e5;
			for(unsigned int iclct =0; iclct < clcts.size(); iclct++){
				int thisClctId = clcts.ch_id->at(iclct);
				if(thisClctId != segId) continue;
				if(inUpdatedChamber)foundCLCT = true;
				unsigned int qual = clcts.quality->at(iclct);
				if(qual == 3 && inUpdatedChamber) found3LayCLCT = true;


				if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), iclct) != matchedCLCTs.end()) continue;
				float clctStripPos = clcts.halfStrip->at(iclct) / 2. + 16*clcts.CFEB->at(iclct);
				if(me11a) clctStripPos -= 16*4;
				if(abs(clctStripPos - segmentX) < minDistanceSegmentToClosestCLCT) {
					minDistanceSegmentToClosestCLCT = abs(clctStripPos - segmentX);
					closestCLCTtoSegmentIndex = iclct;
				}
			}
			if(closestCLCTtoSegmentIndex != -1){ //if we found one
				//printf("found: %i\n", clctQ->at(iclct).at(closestCLCTtoSegmentIndex));
				//matchedCLCTs.push_back(make_pair(thisClctId, (unsigned int)closestCLCTtoSegmentIndex));
				matchedCLCTs.push_back((unsigned int)closestCLCTtoSegmentIndex);
				unsigned int qual = clcts.quality->at(closestCLCTtoSegmentIndex);

				//look to see if we care about this chamber
				auto it = clctLayerCounts.find(segId);
				if(it != clctLayerCounts.end()){
					//fill the correct histogram
					it->second->Fill(qual);
				}


				//if(qual == 3) printChamber(theseCompHits);
				if(qual == 3 && inUpdatedChamber) matched3LayCLCT = true;
				if(EC == 1){
					me11_clctMult->Fill(clcts.size(closestCLCTtoSegmentIndex));
					if(qual == 3) me11_3lay_clctMult->Fill(clcts.size(closestCLCTtoSegmentIndex));
					clctLayerCount_mePlus.at(CH-1)->Fill(qual);
					if(CH == 11){
						if(me11a){
							clctLayerCount_mep11a_11->Fill(qual);
							mep11a_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
							if(qual == 3) mep11a_11_3Lay_Pt->Fill(Pt);
						}
						if(me11b){
							clctLayerCount_mep11b_11->Fill(qual);
							mep11b_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
							if(qual== 3) mep11b_11_3Lay_Pt->Fill(Pt);

						}

					}
				}else if (EC == 2){
					clctLayerCount_meMinus.at(CH-1)->Fill(qual);
					if(CH == 11){
						if(me11a){
							clctLayerCount_mem11a_11->Fill(qual);
						}
						if(me11b){
							clctLayerCount_mem11b_11->Fill(qual);
						}

					}
				}
			}//closest index
			if(foundCLCT) clctEff_hasClct->Fill(Pt);
			if(found3LayCLCT) clctEff_has3LayClct->Fill(Pt);
			if(matched3LayCLCT) clctEff_3LayClct->Fill(Pt);
		}//segments


		//now look through the clcts that were not matched to a segment
		for(unsigned int iclct =0; iclct < clcts.size(); iclct++){
			int thisClctId = clcts.ch_id->at(iclct);
			CSCHelper::ChamberId c = CSCHelper::unserialize(thisClctId);

			EC = c.endcap;
			ST = c.station;
			RI = c.ring;
			CH = c.chamber;

			bool inUpdatedChamber = (CH == 11 && EC == 1);

			if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), iclct) != matchedCLCTs.end()) continue;
			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);

			unsigned int qual = clcts.quality->at(iclct);

			//could be invalidated by segments at edges of chambers, TODO: could make sure CLCTs are not on the edge
			if(inUpdatedChamber){
				if(me11a){
					unmatched_clctLayerCount_mep11a_11->Fill(qual);
				}else if(me11b){
					unmatched_clctLayerCount_mep11b_11->Fill(qual);
				}
			}

		}

			/*
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
					unsigned int qual = clcts.quality->at(closestCLCTtoSegmentIndex);
					if(qual == 3) printChamber(theseCompHits);
					if(EC == 1){
						me11_clctMult->Fill(clcts.size(thisClctId));
						if(qual == 3) me11_3lay_clctMult->Fill(clcts.size(thisClctId));
						clctLayerCount_mePlus.at(CH-1)->Fill(qual);
						if(CH == 11){
							if(me11a){
								clctLayerCount_mep11a_11->Fill(qual);
								mep11a_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								if(qual == 3) mep11a_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
							}
							if(me11b){
								clctLayerCount_mep11b_11->Fill(qual);
								mep11b_11_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));
								if(qual== 3) mep11b_11_3Lay_Pt->Fill(muons.pt->at(segments.mu_id->at(thisSeg)));

							}

						}
					}else if (EC == 2){
						clctLayerCount_meMinus.at(CH-1)->Fill(qual);
						if(CH == 11){
							if(me11a){
								clctLayerCount_mem11a_11->Fill(qual);
							}
							if(me11b){
								clctLayerCount_mem11b_11->Fill(qual);
							}

						}
					}
				}

			}
			*/

			//MATCH LCTS
		/*
			for(unsigned int ilct =0; ilct < lcts.size(); ilct++){
				int thisLctId = lcts.ch_id->at(ilct);
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

				}*/




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
	unmatched_clctLayerCount_mep11a_11->Write();
	unmatched_clctLayerCount_mep11b_11->Write();

	clctEff_den->Write();
	clctEff_hasClct->Write();
	clctEff_has3LayClct->Write();
	clctEff_3LayClct->Write();

	mep11a_11_Pt->Write();
	mep11a_11_3Lay_Pt->Write();
	mep11b_11_Pt->Write();
	mep11b_11_3Lay_Pt->Write();

	me11_clctMult->Write();
	me11_3lay_clctMult->Write();

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

	for(auto hist : clctLayerCounts) hist.second->Write();

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


