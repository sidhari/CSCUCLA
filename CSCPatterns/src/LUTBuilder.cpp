/*
 * LUTBuilder.cpp
 *
 *  Created on: Jun 18, 2019
 *      Author: wnash
 */

#include "../include/LUTBuilder.h"

#include <TTree.h>
#include <TFile.h>

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"
#include "../include/CSCHelperFunctions.h"

int main(int argc, char* argv[]){
	LUTBuilder p;
	return p.main(argc,argv);
}

struct SegmentMatch {
	int clctIndex;
	float posOffset;
	float slopeOffset;
	float pt;
};

int LUTBuilder::run(std::string inputfile, std::string outputfile, int start, int end) {

	cout << "Running over file: " << inputfile << endl;


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
	// TREE ITERATION
	//


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

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

			ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			//get all the clcts in the chamber

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


			vector<SegmentMatch> matchedNew;


			//iterate through segments
			for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
				int segHash = segments.ch_id->at(thisSeg);
				if(segHash != chamberHash) continue;


				float segmentX = segments.pos_x->at(thisSeg); //strips
				float segmentdXdZ = segments.dxdz->at(thisSeg);
				float Pt = muons.pt->at(segments.mu_id->at(thisSeg));


				// IGNORE SEGMENTS AT THE EDGES OF THE CHAMBERS
				if(CSCHelper::segmentIsOnEdgeOfChamber(segmentX, ST,RI)) continue;

				//
				// find all the clcts that are in the chamber, and match
				//

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

					auto& clct = newSetMatch.at(closestNewMatchIndex);

					float clctX = clct->keyStrip();

					SegmentMatch thisMatch;
					thisMatch.clctIndex = closestNewMatchIndex;
					thisMatch.posOffset = segmentX-clctX;
					thisMatch.slopeOffset = segmentdXdZ;
					thisMatch.pt = Pt;

					matchedNewId.push_back(closestNewMatchIndex);
					matchedNew.push_back(thisMatch); //not the most elegant, but fuck it

				}
				if(find(matchedOldId.begin(),matchedOldId.end(), closestOldMatchIndex) == matchedOldId.end()){
					matchedOldId.push_back(closestOldMatchIndex);
				}
			}

			for(int iclct=0; iclct < (int)newSetMatch.size(); iclct++){
				auto& clct = newSetMatch.at(iclct);
				LUTEntry* entry = 0;

				if(bayesLUT.editEntry(clct->key(),entry)){
					return -1;
				}

				bool foundSegment = false;
				for(auto segMatch : matchedNew){
					if(segMatch.clctIndex == iclct){
						foundSegment = true;
						float pt = segMatch.pt;
						float pos = segMatch.posOffset;
						float slope = segMatch.slopeOffset;
						entry->addCLCT(newSetMatch.size(), pt, pos,slope);
					}
				}
				if(!foundSegment){
					entry->addCLCT(newSetMatch.size());
				}
			}


			oldSetMatch.clear();
			newSetMatch.clear();
		}
	}

	bayesLUT.writeToROOT(outputfile);

	cout << "Wrote to file: " << outputfile << endl;
	return 0;
}
