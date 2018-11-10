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

int BayesPatternAnalysis(string inputfile, string outputfile, int start=0, int end=-1) {

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

	//
	// MAKE ALL THE PATTERNS
	//


	vector<CSCPattern>* newEnvelopes = createNewPatterns();
	vector<CSCPattern>* oldEnvelopes = createOldPatterns();


	//
	// Map to look at probability
	// TODO: incorporate this functionality into LUT class once it is more figured out
	//
	LUT bayesLUT("bayes", LINEFIT_LUT_PATH);

	//
	// OUTPUT TREE
	//

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

	TH1F* h_clctPatterns_real = new TH1F("h_clctPatterns_real","Recorded CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);
	TH1F* h_clctPatterns_emulated = new TH1F("h_clctPatterns_emulated","Emulated CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);

	//
	// TREE ITERATION
	//


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

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
			// Emulate the TMB to find all the CLCTs
			//

			ChamberHits compHits(1, ST, RI, EC, CH);

			if(fillCompHits(compHits, comparators)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			if(searchForMatch(compHits, oldEnvelopes,oldSetMatch) || searchForMatch(compHits, newEnvelopes,newSetMatch)) {
				oldSetMatch.clear();
				newSetMatch.clear();
				continue;
			}

			//TODO: currently no implementation dealing with cases where we find one and not other
			if(!oldSetMatch.size() || !newSetMatch.size()) {
				oldSetMatch.clear();
				newSetMatch.clear();
				continue;
			}


			vector<int> matchedNewId;
			vector<int> matchedOldId;


			//iterate through segments
			for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
				int segHash = segments.ch_id->at(thisSeg);
				if(segHash != chamberHash) continue;


				float segmentX = segments.pos_x->at(thisSeg); //strips
				float segmentdXdZ = segments.dxdz->at(thisSeg);

				/*
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
				*/

				if(DEBUG > 0) cout << "--- Segment Position: " << segmentX << " [strips] ---" << endl;
				if(DEBUG > 0) cout << "Legacy Match: (";
				int closestOldMatchIndex = findClosestToSegment(oldSetMatch,segmentX);
				if(DEBUG > 0) cout << ") [strips]" << endl;


				if(DEBUG > 0)cout << "New Match: (";
				int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);
				if(DEBUG > 0) cout << ") [strips]" << endl;

				/* TODO currently not optimum selection could
				 * have a case where clct1 and clct2 are closest to seg1,
				 * clct1 could match seg2, but gets ignore by this procedure
				 */
				if(find(matchedNewId.begin(), matchedNewId.end(), closestNewMatchIndex) == matchedNewId.end()){
					matchedNewId.push_back(closestNewMatchIndex);
					auto& clct = newSetMatch.at(closestNewMatchIndex);

					float clctX = clct->keyStrip();
					LUTEntry* entry = 0;

					if(bayesLUT.editEntry(clct->key(),entry)){
						return -1;
					}
					entry->addSegment(segmentX-clctX, segmentdXdZ);

				}
				if(find(matchedOldId.begin(),matchedOldId.end(), closestOldMatchIndex) == matchedOldId.end()){
					matchedOldId.push_back(closestOldMatchIndex);
				}
			}

			for(unsigned int iclct=0; iclct < newSetMatch.size(); iclct++){
				auto& clct = newSetMatch.at(iclct);
				LUTEntry* entry = 0;

				if(bayesLUT.editEntry(clct->key(),entry)){
					return -1;
				}
				entry->addCLCT(newSetMatch.size());


			}


			oldSetMatch.clear();
			newSetMatch.clear();
		}


	}

	bayesLUT.print();
	bayesLUT.writeToROOT("machineLearnMe.root");


	outF->cd();
	h_clctPatterns_emulated->Write();
	h_clctPatterns_real->Write();


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
			return BayesPatternAnalysis(string(argv[1]), string(argv[2]));
		case 4:
			return BayesPatternAnalysis(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return BayesPatternAnalysis(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







