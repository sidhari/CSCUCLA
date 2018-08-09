/*
 * PatternFinder.cpp
 *
 *  Created on: Sep 27, 2017
 *      Author: root
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

using namespace std;

int PatternFinder(string inputfile, string outputfile, int start=0, int end=-1) {

	printf("Running over file: %s\n", inputfile.c_str());
	TFile* f = TFile::Open(inputfile.c_str());

	if(!f)
	{
		printf("Can't open file\n");
		return -1;
	}

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t){
		printf("Can't find tree\n");
		return -1;
	}

	double Pt = 0;
	double eta = 0;
	bool os = 0;

	//reconstructed offline hits
	vector<int>* rhId = 0; //id/serial
	vector<int>* rhLay = 0;
	vector<float>* rhPos = 0;
	vector<float>* rhE = 0; //energy

	//segments
	vector<int>     *segEc = 0;
	vector<int>     *segSt = 0;
	vector<int>     *segRi = 0;
	vector<int>     *segCh = 0;
	vector<float>   *segX = 0;
	vector<float>	*segdXdZ = 0;

	vector<int>* lctId = 0;
	vector< vector<int> >* lctPat = 0;
	vector< vector<int> >* lctKHS = 0;

	//comparators
	vector<int>* compLay = 0; // y axis
	vector<int>* compId = 0; // index of what ring/station you are on
	vector< vector<int> >* compStr = 0; //comparator strip #
	vector< vector<int> >* compHS = 0; //comparator half strip #
	vector< vector< vector<int> > >* compTimeOn = 0;

	t->SetBranchAddress("Pt",         &Pt);
	t->SetBranchAddress("eta",        &eta);
	t->SetBranchAddress("os",         &os);
	t->SetBranchAddress("rhId",       &rhId);
	t->SetBranchAddress("rhLay",      &rhLay);
	t->SetBranchAddress("rhPos",      &rhPos);
	t->SetBranchAddress("rhE",        &rhE);
	t->SetBranchAddress("segEc",      &segEc);
	t->SetBranchAddress("segSt",      &segSt);
	t->SetBranchAddress("segRi",      &segRi);
	t->SetBranchAddress("segCh",      &segCh);
	t->SetBranchAddress("segX",       &segX);
	t->SetBranchAddress("segdXdZ",    &segdXdZ);
	t->SetBranchAddress("lctId",      &lctId);
	t->SetBranchAddress("lctPat",     &lctPat);
	t->SetBranchAddress("lctKHS",     &lctKHS);
	t->SetBranchAddress("compId",     &compId);
	t->SetBranchAddress("compLay",    &compLay);
	t->SetBranchAddress("compStr",    &compStr);
	t->SetBranchAddress("compHS",     &compHS);
	t->SetBranchAddress("compTimeOn", &compTimeOn);


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


	TH1F* lutSegmentPosDiff = new TH1F("lutSegmentPosDiff", "lutSegmentPosDiff", 100, -1, 1);
	TH1F* lutSegmentSlopeDiff = new TH1F("lutSegmentSlopeDiff", "lutSegmentSlopeDiff", 100, -1, 1);

	vector<TH1F*> segEffNums;  //segment efficiency histograms, based on ranking of LUT Entry
	TH1F* segEffDen = new TH1F("segEffDen", "segEffDen", 30,0,150);


	segEffDen->GetXaxis()->SetTitle("Pt [GeV]");
	segEffDen->GetYaxis()->SetTitle("Count / 5 GeV");
	// vector<TH1F*> segEffDens;
	for(unsigned int clctRank = 0; clctRank < 6; clctRank++){
		segEffNums.push_back(new TH1F(("segEffNum"+to_string(clctRank)).c_str(),
				string("segEffNum"+to_string(clctRank)).c_str(), 30, 0, 150));
		segEffNums.back()->GetXaxis()->SetTitle("Pt [GeV]");
		segEffNums.back()->GetYaxis()->SetTitle("Count / 5 GeV");
	}

	TH1F* foundOneMatchEffNum = new TH1F("foundOneMatchEffNum", "foundOneMatchEffNum", 30,0,150);
	TH1F* foundOneMatchEffDen = new TH1F("foundOneMatchEffDen", "foundOneMatchEffDen", 30,0,150);

	foundOneMatchEffNum->GetXaxis()->SetTitle("Pt [GeV]");
	foundOneMatchEffNum->GetYaxis()->SetTitle("Count / 5 Gev");

	foundOneMatchEffDen->GetXaxis()->SetTitle("Pt [GeV]");
	foundOneMatchEffDen->GetXaxis()->SetTitle("Count / 5 GeV");

	//
	// MAKE LUT
	//

	string dataset = "Charmonium/charmonium2016F+2017BCEF";

	DetectorLUTs newLUTs;
	DetectorLUTs legacyLUTs(true);


	//check if we have made .lut files already
	if(newLUTs.loadAll("data/"+dataset+"/luts/") ||
			legacyLUTs.loadAll("data/"+dataset+"/luts/")){
		printf("Could not find .lut files, recreating them...\n");
		string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/data/"+dataset+"/CLCTMatch-Full.root";
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

		newLUTs.writeAll("data/"+dataset+"/luts/");
		legacyLUTs.writeAll("data/"+dataset+"/luts/");
	} else {
		newLUTs.makeFinal();
		legacyLUTs.makeFinal();
	}


	//pointers used to look at different LUT's
	LUT* thisLUT = 0;
	const LUTEntry* thisEntry = 0;

	//
	// TREE ITERATION
	//

	unsigned int nChambersRanOver = 0;
	unsigned int nChambersMultipleInOneLayer = 0;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);


	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		if(!os) continue;

		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

			EC = (*segEc)[thisSeg];
			ST = (*segSt)[thisSeg];
			RI = (*segRi)[thisSeg];
			CH = (*segCh)[thisSeg];

			segmentX = segX->at(thisSeg); //strips
			segmentdXdZ = segdXdZ->at(thisSeg);


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


			ChamberHits theseRHHits(0, ST, RI, EC, CH);
			ChamberHits theseCompHits(1, ST, RI, EC, CH);

			if(fillCompHits(theseCompHits, compStr,compHS,compTimeOn, compLay,compId)) return -1;

			if (!USE_COMP_HITS || DEBUG) if(fillRecHits(theseRHHits,rhId, rhLay,rhPos)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			ChamberHits* testChamber;
			testChamber = USE_COMP_HITS ? &theseCompHits : &theseRHHits;

			nChambersRanOver++;
			foundOneMatchEffDen->Fill(Pt);

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

			if(newLUTs.getLUT(ST,RI,thisLUT)) {
				printf("Error: can't access LUT for: %i %i\n", ST,RI);
				return -1;
			}

			//TODO: make debug printout of this stuff
			for(auto & clct: newSetMatch){
				if(thisLUT->getEntry(clct->key(), thisEntry)){
					printf("Error: unable to get entry for clct\n");
					return -1;
				}
				//assign the clct the LUT entry we found to be associated with it
				clct->_lutEntry = thisEntry;
			}


			if(newSetMatch.size() > 1){
				sort(newSetMatch.begin(), newSetMatch.end(), CLCTCandidate::quality);
			}

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
			float posCaptureWindow = 0.30; //strips
			float slopeCaptureWindow = 0.25; //strips/layer

			bool foundMatchingCandidate = false;

			//look through all the candidates, until we find the first match
			for(unsigned int iclct = 0; !foundMatchingCandidate && iclct < newSetMatch.size() && iclct < segEffNums.size(); iclct++){
				//depending on how many clcts were allowed to look at,
				// look until we find one
				const LUTEntry* iEntry = newSetMatch.at(iclct)->_lutEntry;


				float lutX = newSetMatch.at(iclct)->keyStrip() + iEntry->position();
				float lutdXdZ =iEntry->slope();

				//only fill the best candidate
				if(iclct == 0){
					lutSegmentPosDiff->Fill(lutX - segmentX);
					lutSegmentSlopeDiff->Fill(lutdXdZ - segmentdXdZ);
				}


				if(abs(lutX - segmentX) < posCaptureWindow &&
						abs(lutdXdZ -segmentdXdZ) < slopeCaptureWindow){

					foundMatchingCandidate = true;
					segEffNums.at(iclct)->Fill(Pt);
					segEffDen->Fill(Pt);
				}
				//segEffNums.at(isegeff)->Fill(Pt);
			}

			if(foundMatchingCandidate) foundOneMatchEffNum->Fill(Pt);


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


			//Clear everything

			oldSetMatch.clear();
			newSetMatch.clear();
		}

	}



	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

	outF->cd();
	plotTree->Write();
	lutSegmentPosDiff->Write();
	lutSegmentSlopeDiff->Write();

	for(unsigned int isegeff = 0; isegeff < segEffNums.size(); isegeff++){
		segEffNums.at(isegeff)->Write();
	}
	segEffDen->Write();

	//defined as : for every segment, we have at least one clct matched within the range
	foundOneMatchEffNum->Write();
	foundOneMatchEffDen->Write();

	outF->Close();

	printf("Finished writing to file: %s\n",outputfile.c_str());

	return 0;
}


int main(int argc, char* argv[])
{

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
}








