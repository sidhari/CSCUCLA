/*
 * PatternFinder.cpp
 *
 *  Created on: Sep 27, 2017
 *      Author: root
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
//#include <TROOT.h>


//#include <ROOT/TTreeProcessorMT.hxx>
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



// TODO: Check for TMB Headers

/* TODO: Multithreading
 *
 * - open files in main thread
 * - make histograms in main thread (following recipe here: https://root.cern.ch/doc/v612/imt101__parTreeProcessing_8C.html)
 * - make tree in main thread, using same recipe book
 * - pass tree to new function PatternFinderThread(ttree*(?)), should return a ttree as well
 * - join threads
 * - combine output trees, and write output file: https://root-forum.cern.ch/t/merging-ttrees-on-the-fly/1833
 *
 */


int PatternFinder(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";


	//
	// MAKE LUT
	//

	string dataset = "Charmonium/charmonium2016F+2017BCEF";

	DetectorLUTs newLUTs;
	DetectorLUTs legacyLUTs(true);

	cout << "Loading Luts..." << endl;
	//check if we have made .lut files already
	if(newLUTs.loadAll("dat/"+dataset+"/luts/") ||
			legacyLUTs.loadAll("dat/"+dataset+"/luts/")){
		printf("Could not find .lut files, recreating them...\n");
		string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/"+dataset+"/CLCTMatch-Full.root";
		TFile* lutFile = new TFile(lutFilepath.c_str());
		if(!lutFile){
			printf("Failed to open lut file: %s\n", lutFilepath.c_str());
			return -1;
		}

		//TODO: change the name of the tree!
		TTree* lutTree = (TTree*)lutFile->Get("plotTree");
		if(!lutTree){
			printf("Can't find lutTree\n");
			return -1;
		}
		if(makeLUT(lutTree, newLUTs, legacyLUTs)){
			cout << "Error: couldn't create LUT" << endl;
			return -1;
		}

		newLUTs.writeAll("dat/"+dataset+"/luts/");
		legacyLUTs.writeAll("dat/"+dataset+"/luts/");
	} else {
		newLUTs.makeFinal();
		legacyLUTs.makeFinal();
	}

	cout << "Loaded LUTS" << endl;

	//pointers used to look at different LUT's
	const LUT* thisLUT = 0;
	const LUTEntry* thisEntry = 0;


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
	// OUTPUT TREE
	//

	int patternId = 0;
	int ccId = 0;
	int legacyLctId = 0;
	int EC = 0; // 1-2
	int ST = 0; // 1-4
	int RI = 0; // 1-4
	int CH = 0;
	float segmentX = 0;
	float segmentdXdZ = 0;
	float patX = 0;
	float legacyLctX = 0;

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	TTree * plotTree = new TTree("plotTree","TTree holding processed info for CSCPatterns studies");
	plotTree->Branch("EC",&EC,"EC/I");
	plotTree->Branch("ST",&ST,"ST/I");
	plotTree->Branch("RI",&RI,"RI/I");
	plotTree->Branch("CH",&CH,"CH/I");
	plotTree->Branch("patternId", &patternId, "patternId/I");
	plotTree->Branch("ccId", &ccId, "ccId/I");
	plotTree->Branch("legacyLctId", &legacyLctId, "legacyLctId/I");
	plotTree->Branch("segmentX", &segmentX, "segmentX/F");
	plotTree->Branch("segmentdXdZ", &segmentdXdZ, "segmentdXdZ/F");
	plotTree->Branch("patX", &patX, "patX/F");
	plotTree->Branch("legacyLctX", &legacyLctX, "legacyLctX/F");

	TH1F* lutSegmentPosDiff = new TH1F("h_lutSegmentPosDiff", "h_lutSegmentPosDiff", 100, -1, 1);
	TH1F* lutSegmentPosDiff_halfStrip = new TH1F("h_lutSegmentPosDiff_halfStrip", "h_lutSegmentPosDiff_halfStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_quarterStrip = new TH1F("h_lutSegmentPosDiff_quarterStrip", "h_lutSegmentPosDiff_quarterStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_eighthStrip = new TH1F("h_lutSegmentPosDiff_eighthStrip", "h_lutSegmentPosDiff_eighthStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_sixteenthStrip = new TH1F("h_lutSegmentPosDiff_sixteenthStrip", "h_lutSegmentPosDiff_sixteenthStrip", 100, -1, 1);
	TH1F* lutSegmentSlopeDiff = new TH1F("h_lutSegmentSlopeDiff", "h_lutSegmentSlopeDiff", 100, -1, 1);


	TH1F* legacyLUTSegmentPosDiff = new TH1F("h_legacyPosDiff", "h_legacyPosDiff; lut - seg[strips]; segments",100,-1,1);
	TH1F* legacyLUTSegmentSlopeDiff = new TH1F("h_legacySlopeDiff", "h_legacySlopeDiff; lut - seg [strips/lay]", 100, -1,1);
	TH1F* h_clctLayerCount = new TH1F("h_clctLayerCount", "h_clctlayerCount", 7,0,7);
	//t->SetBranchAddress("Event_EventNumber", &evt.EventNumber);

	//CSCInfo::Muons muons;
	//t->SetBranchAddress("muon_pt", &muons.pt);

	/*
	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		//printf("EventNum = %llu\n", evt.EventNumber);
		for(unsigned int j = 0; j < muons.size(); j++) {
			printf("muon_pt: %f\n", muons.pt[j]);
		}

	}
	return 0;
*/

	//
	// TREE ITERATION
	//

	unsigned int nChambersRanOver = 0;
	unsigned int nChambersMultipleInOneLayer = 0;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	//t->SetImplicitMT(true);


	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
			CSCHelper::ChamberId c = CSCHelper::unserialize(segments.ch_id->at(thisSeg));

			EC = c.endcap;
			ST = c.station;
			RI = c.ring;
			CH = c.chamber;

			segmentX = segments.pos_x->at(thisSeg); //strips
			segmentdXdZ = segments.dxdz->at(thisSeg);


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

			//REMOVE ME
			//if(!me11b) continue;


			ChamberHits theseRHHits(ST, RI, EC, CH,false);
			ChamberHits theseCompHits(ST, RI, EC, CH);

			//if(fillCompHits(theseCompHits, comparators)) return -1;
			if(theseCompHits.fill(comparators)) return -1;

			if (!USE_COMP_HITS || DEBUG > 0) if(fillRecHits(theseRHHits,recHits)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			ChamberHits* testChamber;
			testChamber = USE_COMP_HITS ? &theseCompHits : &theseRHHits;

			nChambersRanOver++;

			//now run on comparator hits
			if(DEBUG > 0) printf("~~~~ Matches for Muon: %i,  Segment %i ~~~\n",i,  thisSeg);
			if(searchForMatch(*testChamber, oldEnvelopes,oldSetMatch) || searchForMatch(*testChamber, newEnvelopes,newSetMatch)) {
				oldSetMatch.clear();
				newSetMatch.clear();
				nChambersMultipleInOneLayer++;
				continue;
			}

			//TODO: currently no implementation dealing with cases where we find one and not other
			if(!oldSetMatch.size() || !newSetMatch.size()) {
				oldSetMatch.clear();
				newSetMatch.clear();
				continue;
			}

			//Now compare with LUT data

			/* NEW LUTS
			 *
			 */

			if(newLUTs.getLUT(ST,RI,thisLUT)) {
				printf("Error: can't access LUT for: %i %i\n", ST,RI);
				return -1;
			}

			//TODO: make debug printout of this stuff
			for(auto & clct: newSetMatch){
				if(thisLUT->getEntry(clct->key(), thisEntry)){
					printf("Error: unable to get entry for clct: pat: %i cc: %i\n", clct->patternId(), clct->comparatorCodeId());
					return -1;
				}
				//assign the clct the LUT entry we found to be associated with it
				clct->_lutEntry = thisEntry;
			}

			//sort the matches
			sort(newSetMatch.begin(), newSetMatch.end(), CLCTCandidate::quality);

			if(DEBUG > 0){
				printf("segmentX: %f - segmentdXdZ: %f\n", segmentX,segmentdXdZ);
				for(auto & clct : newSetMatch){
					thisEntry = clct->_lutEntry;

					float thisLutX = clct->keyStrip() + thisEntry->position();
					float thisLutSlope = thisEntry->slope();
					printf("\t\tlutx: %f, lut dxdz: %f layers: %i, chi2: %f, slope: %f\n",
							thisLutX, thisLutSlope, thisEntry->_layers, thisEntry->_chi2, thisEntry->slope());
				}
			}

			// fill the numerator if it is within our capture window
			//float posCaptureWindow = 0.30; //strips
			//float slopeCaptureWindow = 0.25; //strips/layer


			bool foundMatchingCandidate = false;
			float minX = 1e5;
			float minX_halfStrip = 1e5;
			float minX_quarterStrip = 1e5;
			float minX_eighthStrip = 1e5;
			float minX_sixteenthStrip = 1e5;
			float mindXdZ = 1e5;

			//look through all the candidates, until we find the first match
			for(unsigned int iclct = 0; !foundMatchingCandidate && iclct < newSetMatch.size(); iclct++){
				//depending on how many clcts were allowed to look at,
				// look until we find one
				const LUTEntry* iEntry = newSetMatch.at(iclct)->_lutEntry;


				float lutX = newSetMatch.at(iclct)->keyStrip() + iEntry->position()-1;
				float lutdXdZ =iEntry->slope();

				//only fill the best candidate
/*
				if(iclct == 0){
					lutSegmentPosDiff->Fill(lutX - segmentX);
					lutSegmentSlopeDiff->Fill(lutdXdZ - segmentdXdZ);
				}
*/
				float xDiff = lutX - segmentX;
				float xDiff_halfStrip = round(2.*lutX)/2. - segmentX;
				float xDiff_quarterStrip = round(4.*lutX)/4. - segmentX;
				float xDiff_eighthStrip = round(8.*lutX)/8. - segmentX;
				float xDiff_sixteenthStrip = round(16.*lutX)/16. - segmentX;
				float dxdzDiff = lutdXdZ - segmentdXdZ;
				//cout << " EC " << EC << "ST: " << ST << endl;
				//cout << "xDiff " << xDiff << "nsegs " << newSetMatch.at(iclct)->_lutEntry->nsegments() <<endl;

				if(abs(xDiff) < abs(minX)){
					minX = xDiff;
					minX_halfStrip = xDiff_halfStrip;
					minX_quarterStrip = xDiff_quarterStrip;
					minX_eighthStrip = xDiff_eighthStrip;
					minX_sixteenthStrip = xDiff_sixteenthStrip;
					mindXdZ = dxdzDiff;
					foundMatchingCandidate = true;
				}


/*
				if(abs(lutX - segmentX) < posCaptureWindow &&
						abs(lutdXdZ -segmentdXdZ) < slopeCaptureWindow){

					foundMatchingCandidate = true;
					//segEffNums.at(iclct)->Fill(Pt);
					//segEffDen->Fill(Pt);
					//lutSegmentPosDiff->Fill(lutX - segmentX);
					//lutSegmentSlopeDiff->Fill(lutdXdZ - segmentdXdZ);
				}
*/
				//segEffNums.at(isegeff)->Fill(Pt);
			}
			if(foundMatchingCandidate ){
				lutSegmentPosDiff->Fill(minX);
				lutSegmentPosDiff_halfStrip->Fill(minX_halfStrip);
				lutSegmentPosDiff_quarterStrip->Fill(minX_quarterStrip);
				lutSegmentPosDiff_eighthStrip->Fill(minX_eighthStrip);
				lutSegmentPosDiff_sixteenthStrip->Fill(minX_sixteenthStrip);
				lutSegmentSlopeDiff->Fill(mindXdZ);
			}

			//if(foundMatchingCandidate) foundOneMatchEffNum->Fill(Pt);

			/*
			 *  OLD LUTS
			 */
			if(legacyLUTs.getLUT(ST,RI,thisLUT)) {
				printf("Error: can't access LUT for: %i %i\n", ST,RI);
				return -1;
			}

			for(auto & clct : oldSetMatch){
				if(thisLUT->getEntry(clct->key(), thisEntry)){
					printf("Error: unable to get entry for clct: pat: %i cc: %i\n", clct->patternId(), clct->comparatorCodeId());
					return -1;
				}
				//assign the clct the LUT entry we found to be associated with it
				clct->_lutEntry = thisEntry;


			}

			bool foundMatchingCandidate_legacy = false;
			float minX_legacy = 1e5;
			float mindXdZ_legacy = 1e5;
			for(unsigned int iclct=0; !foundMatchingCandidate_legacy &&iclct < oldSetMatch.size(); iclct++) {
				const LUTEntry* iEntry = oldSetMatch.at(iclct)->_lutEntry;

				float lutX = oldSetMatch.at(iclct)->keyStrip() + iEntry->position()-1;
				float lutdXdZ = iEntry->slope();

				float xDiff = lutX - segmentX;
				float dxdzDiff = lutdXdZ - segmentdXdZ;

				if(abs(xDiff) < abs(minX_legacy)){
					minX_legacy = xDiff;
					mindXdZ_legacy = dxdzDiff;
					foundMatchingCandidate_legacy = true;
				}
			}
			if(foundMatchingCandidate_legacy){
				legacyLUTSegmentPosDiff->Fill(minX_legacy);
				legacyLUTSegmentSlopeDiff->Fill(mindXdZ_legacy);
			}


			if(DEBUG > 0) cout << "--- Segment Position: " << segmentX << " [strips] ---" << endl;
			if(DEBUG > 0) cout << "Legacy Match: (";
			int closestOldMatchIndex = findClosestToSegment(oldSetMatch,segmentX);
			if(DEBUG > 0) cout << ") [strips]" << endl;


			if(DEBUG > 0)cout << "New Match: (";
			int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);
			if(DEBUG > 0) cout << ") [strips]" << endl;

			// Fill Tree Data

			patX = newSetMatch.at(closestNewMatchIndex)->keyStrip();
			ccId = newSetMatch.at(closestNewMatchIndex)->comparatorCodeId();
			patternId = newSetMatch.at(closestNewMatchIndex)->patternId();
			legacyLctId = oldSetMatch.at(closestOldMatchIndex)->patternId();
			legacyLctX = oldSetMatch.at(closestOldMatchIndex)->keyStrip();

			plotTree->Fill();

			h_clctLayerCount->Fill(newSetMatch.at(closestNewMatchIndex)->layerCount());

			//CLCTCandidate* bestCLCT = newSetMatch.at(closestNewMatchIndex);


			//unsigned int layers = bestCLCT->_lutEntry->_layers;

/*
			int code_hits [MAX_PATTERN_WIDTH][NLAYERS];
			if(bestCLCT->getHits(code_hits)){
				printf("Error: can't recover hits\n");
				return -1;
			}
*/
			//float hs_clctkeyhs = 2*bestCLCT->keyStrip();




			//Clear everything

			oldSetMatch.clear();
			newSetMatch.clear();

			//temp
			//return 0;
		}

	}



	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

	outF->cd();
	plotTree->Write();
	lutSegmentPosDiff->Write();
	lutSegmentPosDiff_halfStrip->Write();
	lutSegmentPosDiff_quarterStrip->Write();
	lutSegmentPosDiff_eighthStrip->Write();
	lutSegmentPosDiff_sixteenthStrip->Write();
	lutSegmentSlopeDiff->Write();

	legacyLUTSegmentPosDiff->Write();
	legacyLUTSegmentSlopeDiff->Write();
	h_clctLayerCount->Write();
	/*
	lutSegmentPosDiff->Write();
	lutSegmentSlopeDiff->Write();

	for(unsigned int isegeff = 0; isegeff < segEffNums.size(); isegeff++){
		segEffNums.at(isegeff)->Write();
	}
	segEffDen->Write();

	//defined as : for every segment, we have at least one clct matched within the range
	foundOneMatchEffNum->Write();
	foundOneMatchEffDen->Write();

	for(auto hist : chi2PosDiffs) hist->Write();
	for(auto hist : chi2Distributions) hist->Write();
	for(auto hist : chi2VsSlope) hist->Write();
*/
	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());


	// print program timing information
	//cout << "Time elapsed: " << float(clock()- c_start) / CLOCKS_PER_SEC << " s" << endl;

	//return 0;

//	cout << "Fraction with >1 in layer is " <<  (*nChambersMultipleInOneLayer.Get()) << "/" <<
//			(*nChambersRanOver.Get()) << " = " << 1.*(*nChambersMultipleInOneLayer.Get())/(*nChambersRanOver.Get()) << endl;

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;

}


int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return PatternFinder(string(argv[1]), string(argv[2]));
		case 4:
			return PatternFinder(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return PatternFinder(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







