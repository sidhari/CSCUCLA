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
#include <TLegend.h>
#include <TStyle.h>
#include <THStack.h>
#include <TString.h>
#include <TROOT.h>

#include <string>
#include <vector>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"

using namespace std;


#ifdef __MAKECINT__
#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<vector<int> > >+;
#endif


int PatternFinder(string inputfile, string outputfile, int start, int end) {

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
    // SET ALL OUR CHARGE ENVELOPES
    //


    vector<ChargePattern>* newEnvelopes = createNewEnvelopes();
    vector<ChargePattern>* oldEnvelopes = createOldEnvelopes();

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
    int chSid = 0;
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
            chSid = chamberSerial(EC, ST, RI, CH);

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


            if(fillCompHits(theseCompHits, compStr,compHS,compTimeOn, compLay,compId,
            					EC,ST,RI,CH)) return -1;


            //find out where the reconstructed hits are for this segment
            for(unsigned int thisRh = 0; thisRh < rhId->size(); thisRh++)
            {
                int thisId = rhId->at(thisRh);

                if(chSid != thisId) continue; //just look at matches
                //rhLay goes 1-6
                unsigned int iLay = rhLay->at(thisRh)-1;

                //goes 1-80
                float thisRhPos = rhPos->at(thisRh);

                int iRhStrip = round(2.*thisRhPos-.5)-1; //round and shift to start at zero
                if(me11a ||me11b || !(iLay%2)) iRhStrip++; // add one to account for staggering, if even layer

                if((unsigned int)iRhStrip >= N_MAX_HALF_STRIPS || iRhStrip < 0){
                    printf("ERROR: recHit index %i invalid\n", iRhStrip);
                    return -1;
                }

                theseRHHits.hits[iRhStrip][iLay] = true;
            }


            vector<SingleEnvelopeMatchInfo*> newSetMatch;
            vector<SingleEnvelopeMatchInfo*> oldSetMatch;

            ChamberHits* testChamber;
            if(USE_COMP_HITS){
                testChamber = &theseCompHits;
            } else {
                testChamber = &theseRHHits;
            }

            nChambersRanOver++;

            //now run on comparator hits
            if(DEBUG > 0) printf("~~~~ Matches for Muon: %i,  Segment %i ~~~\n",i,  thisSeg);
            if(searchForMatch(*testChamber, oldEnvelopes,oldSetMatch) || searchForMatch(*testChamber, newEnvelopes,newSetMatch)) {
            		while(oldSetMatch.size()){
            			delete oldSetMatch.back();
            			oldSetMatch.pop_back();
            		}
            		while(newSetMatch.size()){
            			delete newSetMatch.back();
            			newSetMatch.pop_back();
            		}
            		nChambersMultipleInOneLayer++;
            		continue;
            }

            //TODO: currently no implementation dealing with cases where we find one and not other
            if(!oldSetMatch.size() || !newSetMatch.size()) {
            		while(oldSetMatch.size()){
            			delete oldSetMatch.back();
            			oldSetMatch.pop_back();
            		}
            		while(newSetMatch.size()){
            			delete newSetMatch.back();
            			newSetMatch.pop_back();
            		}
            		continue;
            }

            if(DEBUG > 0) cout << "--- Segment Position: " << segmentX << " [strips] ---" << endl;
            if(DEBUG > 0) cout << "Legacy Match: (";
            int closestOldMatchIndex = findClosestToSegment(oldSetMatch,segmentX);
            if(DEBUG > 0) cout << ") [strips]" << endl;


            if(DEBUG > 0)cout << "New Match: (";
            int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);
            if(DEBUG > 0) cout << ") [strips]" << endl;

            // Fill Tree Data

            patX = newSetMatch.at(closestNewMatchIndex)->x();
            ccId = newSetMatch.at(closestNewMatchIndex)->comparatorCodeId();
            patternId = newSetMatch.at(closestNewMatchIndex)->patternId();
            legacyLctId = oldSetMatch.at(closestOldMatchIndex)->patternId();
            legacyLctX = oldSetMatch.at(closestOldMatchIndex)->x();

            plotTree->Fill();


            while(oldSetMatch.size()){
            		delete oldSetMatch.back();
            		oldSetMatch.pop_back();
            }
            while(newSetMatch.size()){
            		delete newSetMatch.back();
            		newSetMatch.pop_back();
            }
        }

    }

	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

    plotTree->Write();
    outF->Close();

    return 0;
}

int PatternFinder(string inputfile, string outputfile, int events){
	return PatternFinder(inputfile,outputfile, 0,-1);
}

int PatternFinder(string inputfile, string outputfile){
	return PatternFinder(inputfile, outputfile, 0, -1);
}



int main(int argc, char* argv[])
{

    //secret sauce using g++
    gROOT->ProcessLine("#include <vector>");
    gROOT->ProcessLine("#pragma link C++ class vector<vector<int> >+;");
    gROOT->ProcessLine("#pragma link C++ class vector<vector<vector<int> > >+;");

	switch(argc){
		case 3:
			return PatternFinder(string(argv[1]), string(argv[2]));
		case 4:
			return PatternFinder(string(argv[1]), string(argv[2]),atoi(argv[3]));
		case 5:
			return PatternFinder(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave %i Arguments, usage is:" << endl;
			cout << "./PatternFinder inputFile outputFile (events)" << endl;
			return -1;
	}

}




