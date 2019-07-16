/*
 * LUTBuilder_TEMPLATE.cpp
 *
 *  Created on: Jun 18, 2019
 *      Author: wnash
 */


#include "../include/LUTBuilder_TEMPLATE.h"

#include <iostream>

#include <TTree.h>
#include <TFile.h>

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"

int main(int argc, char* argv[]){
	LUTBuilder_TEMPLATE p;
	return p.main(argc,argv);
}

struct SegmentMatch {
	int clctIndex;
	float posOffset;
	float slopeOffset;
	float pt;
};

int LUTBuilder_TEMPLATE::run(std::string inputfile, std::string outputfile, int start, int end){
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

	//
	// LUT
	//
	//TODO: need to fix to use LINEFIT_LUT_PATH and make it capable on LXPLUS as well as LPC
	LUT demoLUT("demo", "dat/linearFits.lut");
	//LUT demoLUT("demo", string("dat/linearFits.lut"));
	demoLUT.print();
	return 0;


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

			//get all the clcts in the chamber

			if(searchForMatch(compHits, newEnvelopes,newSetMatch)) {
				newSetMatch.clear();
				continue;
			}

			if(!newSetMatch.size()) {
				newSetMatch.clear();
				continue;
			}


			vector<int> matchedNewId;
			vector<SegmentMatch> matchedNew;


			//iterate through segments
			for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
				int segHash = segments.ch_id->at(thisSeg);
				if(segHash != chamberHash) continue;


				float segmentX = segments.pos_x->at(thisSeg); //strips
				float segmentdXdZ = segments.dxdz->at(thisSeg);

				//TODO: figure out way to deal with non-muon associated segments
				if(segments.mu_id->at(thisSeg) == -1) continue;
				float Pt = muons.pt->at(segments.mu_id->at(thisSeg));

				//avoid segments at the edge of the chamber
				if(CSCHelper::segmentIsOnEdgeOfChamber(segmentX, ST,RI)) continue;

				//
				// find all the clcts that are in the chamber, and match
				//

				if(DEBUG > 0) cout << "--- Segment Position: " << segmentX << " [strips] ---" << endl;
				if(DEBUG > 0)cout << "New Match: (";
				int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);
				if(DEBUG > 0) cout << ") [strips]" << endl;

				// TODO currently not optimum selection could
				// have a case where clct1 and clct2 are closest to seg1,
				// clct1 could match seg2, but gets ignore by this procedure

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
			}

			//  Look through all the clcts we made, see if we have associated segments,
			//  then add then to our LUT

			for(int iclct=0; iclct < (int)newSetMatch.size(); iclct++){
				auto& clct = newSetMatch.at(iclct);
				LUTEntry* entry = 0;

				if(demoLUT.editEntry(clct->key(),entry)){
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


			newSetMatch.clear();
		}


	}

	// Manipulate LUT here

	//sort the LUT however you want
	demoLUT.sort("sclxp");

	//print it out
	demoLUT.print(10); //print all the entries with at least 10 clcts

	demoLUT.writeToROOT(outputfile);


	cout << "Wrote to file: " << outputfile << endl;
	return 0;
}
