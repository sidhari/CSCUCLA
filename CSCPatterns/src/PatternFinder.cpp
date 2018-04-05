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



int PatternFinder(int index) {
    int start = (index-1)*NEVENTS;
    int end = index*NEVENTS;
    TFile* f = TFile::Open(("../data/"+INPUT_FILENAME).c_str());

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
    vector< vector<int> >* compStr = 0; //comparator strip #
    vector< vector<int> >* compHS = 0; //comparator half strip #
    vector< vector< vector<int> > >* compTimeOn = 0;
    vector<int>* compLay = 0; // y axis
    vector<int>* compId = 0; // index of what ring/station you are on
    vector<int>* tmbId = 0;

    TBranch *b_Pt;
    TBranch *b_eta;
    TBranch *b_os;
    TBranch *b_rhId; //id/serial
    TBranch *b_rhLay;
    TBranch *b_rhPos;
    TBranch *b_rhE; //energy
    TBranch *b_segEc;
    TBranch *b_segSt;
    TBranch *b_segRi;
    TBranch *b_segCh;
    TBranch *b_segX;
    TBranch *b_segdXdZ;
    TBranch *b_lctId;
    TBranch *b_lctPat;
    TBranch *b_lctKHS;
    TBranch *b_compStr; //comparator strip #
    TBranch *b_compHS; //comparator half strip #
    TBranch *b_compTimeOn;
    TBranch *b_compLay; // y axis
    TBranch *b_compId; // index of what ring/station you are on
    TBranch *b_tmbId;

    t->SetBranchAddress("segEc", &segEc, &b_segEc );
    t->SetBranchAddress("segSt",&segSt, &b_segSt);
    t->SetBranchAddress("segRi",      &segRi     , &b_segRi     );
    t->SetBranchAddress("segCh",      &segCh     , &b_segCh     );
    t->SetBranchAddress("segX",       &segX      , &b_segX      );
    t->SetBranchAddress("segdXdZ",    &segdXdZ   , &b_segdXdZ   );
    t->SetBranchAddress("compId",     &compId    , &b_compId    );
    t->SetBranchAddress("compLay",    &compLay   , &b_compLay   );
    t->SetBranchAddress("compStr",    &compStr   , &b_compStr   );
    t->SetBranchAddress("compHS",     &compHS    , &b_compHS    );
    t->SetBranchAddress("compTimeOn", &compTimeOn, &b_compTimeOn);
    t->SetBranchAddress("Pt",         &Pt        , &b_Pt        );
    t->SetBranchAddress("eta",        &eta       , &b_eta       );
    t->SetBranchAddress("rhId",       &rhId      , &b_rhId      );
    t->SetBranchAddress("rhLay",      &rhLay     , &b_rhLay     );
    t->SetBranchAddress("rhPos",      &rhPos     , &b_rhPos     );
    t->SetBranchAddress("rhE",        &rhE       , &b_rhE       );
    t->SetBranchAddress("lctId",      &lctId     , &b_lctId     );
    t->SetBranchAddress("lctPat",     &lctPat    , &b_lctPat    );
    t->SetBranchAddress("lctKHS",     &lctKHS    , &b_lctKHS    );
    t->SetBranchAddress("tmbId",      &tmbId     , &b_tmbId     );
    t->SetBranchAddress("os",         &os        , &b_os        );


    //
    // SET ALL OUR CHARGE ENVELOPES
    //


    vector<ChargePattern>* newEnvelopes = createNewEnvelopes();
    vector<ChargePattern>* oldEnvelopes = createOldEnvelopes();


    //
    // OUTPUT TREE
    //

    int envelopeId = 0;
    int patternId = 0;
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

    string folder = (USE_COMP_HITS ? "compHits" : "recHits");
    TFile * outF = new TFile(string("../data/" + folder + "/processedMatches_" + to_string((long long int) index) + ".root").c_str(),"RECREATE");
    TTree * plotTree = new TTree("plotTree","TTree holding processed info for CSCPatterns studies");
    plotTree->Branch("EC",&EC,"EC/I");
    plotTree->Branch("ST",&ST,"ST/I");
    plotTree->Branch("RI",&RI,"RI/I");
    plotTree->Branch("CH",&CH,"CH/I");
    plotTree->Branch("envelopeId", &envelopeId, "envelopeId/I");
    plotTree->Branch("patternId", &patternId, "patternId/I");
    plotTree->Branch("legacyLctId", &legacyLctId, "legacyLctId/I");
    plotTree->Branch("segmentX", &segmentX, "segmentX/F");
    plotTree->Branch("segmentdXdZ", &segmentdXdZ, "segmentdXdZ/F");
    plotTree->Branch("patX", &patX, "patX/F");
    plotTree->Branch("legacyLctX", &legacyLctX, "legacyLctX/F");

    //cout << "Tree Loaded, now starting loop" << endl;

    //
    // TREE ITERATION
    //

    unsigned int nChambersRanOver = 0;
    unsigned int nChambersMultipleInOneLayer = 0;


    if(end > t->GetEntries()) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	for(int i = start; i < end; i++) {
        if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

        t->GetEntry(i);

        ///
        //TEMPRARY PT CUT TO REMOVE LATER!!!!!!!
        ///

        if(Pt < 30) continue;

        ///
        ///
        ///

        if(!os) continue;

        //iterate through segments
        for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

            EC = (*segEc)[thisSeg];
            ST = (*segSt)[thisSeg];
            RI = (*segRi)[thisSeg];
            CH = (*segCh)[thisSeg];
            chSid = chamberSerial(EC, ST, RI, CH);

            //cout << "Tree loop" << endl;

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


            for(unsigned int icomp = 0; icomp < compId->size(); icomp++){
                if(chSid != (*compId)[icomp]) continue; //only look at where we are now

                unsigned int thisCompLay = (*compLay)[icomp]-1;
                for(unsigned int icompstr = 0; icompstr < (*compStr)[icomp].size(); icompstr++){
                    //goes from 1-80
                    int compStrip = compStr->at(icomp).at(icompstr);
                    int compHStrip = compHS->at(icomp).at(icompstr);
                    if(compStrip < 1.0) printf("compStrip = %i, how did that happen?\n", compStrip);

                    int timeOn = 0;

                    //look at the time on value, to fill the chamber array
                    if(!compTimeOn->at(icomp)[icompstr].size()){
                        printf("Error dimensions of comparator time on vector are incorrect. size %lu= \n",
                                compTimeOn->at(icomp)[icompstr].size());
                        return -1;
                    } else {
                        timeOn = compTimeOn->at(icomp)[icompstr].front();
                    }

                    //account for weird me11a/b
                    if((me11a || me11b) && compStrip > 64) compStrip -= 64;


                    unsigned int halfStripVal;
                    if(me11a ||me11b || !(thisCompLay%2)){ //if we are in me11 or an even layer (opposite from Cameron's code, since I shift to zero)
                        halfStripVal = 2*(compStrip-1)+compHStrip+1;
                    } else { //odd layers shift down an extra half strip
                        halfStripVal = 2*(compStrip-1)+compHStrip;
                    }


                    if(halfStripVal >= N_MAX_HALF_STRIPS || halfStripVal < 0) {
                        printf("Error: For compId = %i, ST = %i, RI = %i Comp Half Strip Value out of range index = %i - me11b = %i, compStrip = %i, compHStrip = %i, layer = %i\n",
                                chSid,ST,RI, halfStripVal, me11b, compStrip, compHStrip, thisCompLay);
                        return -1;
                    } else {
                        if(timeOn < 0 || timeOn >= 16) {
                            printf("Error timeOn is an invalid number: %i\n", timeOn);
                            return -1;
                        } else {
                            theseCompHits.hits[halfStripVal][thisCompLay] = timeOn+1; //store +1, so we dont run into trouble with hexadecimal
                        }
                    }


                }
            }


            //find out where the reconstructed hits are for this segment
            for(unsigned int thisRh = 0; thisRh < rhId->size(); thisRh++)
            {
                int thisId = rhId->at(thisRh);

                if(chSid != thisId) continue; //just look at matches
                //rhLay goes 1-6
                unsigned int iLay = rhLay->at(thisRh)-1;


                //goes 1-80
                float thisRhPos = rhPos->at(thisRh);


                unsigned int iRhStrip = round(2.*thisRhPos-.5)-1; //round and shift to start at zero
                if(me11a ||me11b || !(iLay%2)) iRhStrip++; // add one to account for staggering, if even layer

                if(iRhStrip >= N_MAX_HALF_STRIPS || iRhStrip < 0){
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
            patternId = newSetMatch.at(closestNewMatchIndex)->comparatorCodeId();
            envelopeId = newSetMatch.at(closestNewMatchIndex)->envelopeId();
            legacyLctId = oldSetMatch.at(closestOldMatchIndex)->envelopeId();
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

	//printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

    plotTree->Write();
    outF->Close();


    return 0;
}


int main(int argc, char* argv[])
{
    if(argc != 2) 
    {
        cout << "Gave %i Arguments, usage is ./PatternFinder jobIndex" << endl;
        return -1;
    }

    //secret sauce using g++
    gROOT->ProcessLine("#include <vector>");
    gROOT->ProcessLine("#pragma link C++ class vector<vector<int> >+;");
    gROOT->ProcessLine("#pragma link C++ class vector<vector<vector<int> > >+;");

    return PatternFinder(atoi(argv[1])-1);
}




