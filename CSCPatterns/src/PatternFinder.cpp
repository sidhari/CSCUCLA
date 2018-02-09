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

#include <vector>
#include <map>
#include <stdio.h>

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"

using namespace std;

//secret sauce to make vectors of vectors work in root
#ifdef __MAKECINT__
#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<vector<int> > >+;
#endif


int PatternFinder() {
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
    vector<vector<int>>* lctPat = 0;
    vector<vector<int>>* lctKHS = 0;

    //comparators
    vector<vector<int>>* compStr = 0; //comparator strip #
    vector<vector<int>>* compHS = 0; //comparator half strip #
    vector<vector<vector<int>>>* compTimeOn = 0;
    vector<int>* compLay = 0; // y axis
    vector<int>* compId = 0; // index of what ring/station you are on
    vector<int>* tmbId = 0;

    t->SetBranchAddress("segEc", &segEc);
    t->SetBranchAddress("segSt",&segSt);
    t->SetBranchAddress("segRi", &segRi);
    t->SetBranchAddress("segCh", &segCh);
    t->SetBranchAddress("segX", &segX);
    t->SetBranchAddress("segdXdZ", &segdXdZ);
    t->SetBranchAddress("compId",&compId);
    t->SetBranchAddress("compLay",&compLay);
    t->SetBranchAddress("compStr",&compStr);
    t->SetBranchAddress("compHS",&compHS);
    t->SetBranchAddress("compTimeOn", &compTimeOn);
    t->SetBranchAddress("Pt", &Pt);
    t->SetBranchAddress("eta", &eta);
    t->SetBranchAddress("rhId",&rhId);
    t->SetBranchAddress("rhLay", &rhLay);
    t->SetBranchAddress("rhPos", &rhPos);
    t->SetBranchAddress("rhE", &rhE);
    t->SetBranchAddress("lctId", &lctId);
    t->SetBranchAddress("lctPat",&lctPat);
    t->SetBranchAddress("lctKHS",&lctKHS);
    t->SetBranchAddress("tmbId",&tmbId);
    t->SetBranchAddress("os", &os);


    //
    // SET ALL OUR CHARGE ENVELOPES
    //


    vector<ChargeEnvelope>* newEnvelopes = createNewEnvelopes();

    vector<ChargeEnvelope>* oldEnvelopes = createOldEnvelopes();//createNewEnvelopesVERIFICATION(); //createOldEnvelopes();


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
    TFile * outF = new TFile(string("../data/" + folder + "/plotTree.root").c_str(),"RECREATE");
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


    //
    // TREE ITERATION
    //

    unsigned int nChambersRanOver = 0;
    unsigned int nChambersMultipleInOneLayer = 0;

	const unsigned int max = 1000; //t->GetEntries(); //or t->GetEntries() or MAX_ENTRY
	for(unsigned int i = 0; i < max; i++) {
        if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*i/max, i);

        t->GetEntry(i);

        if(!os) continue;

        //iterate through segments
        for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

            EC = (*segEc)[thisSeg];
            ST = (*segSt)[thisSeg];
            RI = (*segRi)[thisSeg];
            CH = (*segCh)[thisSeg];
            chSid = chamberSerial(EC, ST, RI, CH);

            segmentX = (*segX)[thisSeg];
            segmentdXdZ = (*segdXdZ)[thisSeg];

            //if(ST != 2 && RI != 2) continue;

            bool me11a = (ST == 1 && RI == 4);
            bool me11b = (ST == 1 && RI == 1);


            ChamberHits theseRHHits;
            ChamberHits theseCompHits;
            //initialize them to zero
            for(int x=0; x < N_MAX_HALF_STRIPS; x++){
                for(int y = 0; y < NLAYERS; y++){
                    theseRHHits.hits[x][y] = 0;
                    theseCompHits.hits[x][y] = 0;
                }
            }

            theseRHHits.isComparator = false;
            theseCompHits.isComparator = true;

            //label their station, ring , endcap
            theseRHHits.station = ST;
            theseCompHits.station = ST;
            theseRHHits.ring = RI;
            theseCompHits.ring = RI;
            theseRHHits.endcap = EC;
            theseCompHits.endcap = EC;
            theseRHHits.chamber = CH;
            theseCompHits.chamber = CH;


            for(unsigned int icomp = 0; icomp < compId->size(); icomp++){
                if(chSid != (*compId)[icomp]) continue; //only look at where we are now

                unsigned int thisCompLay = (*compLay)[icomp]-1;
                for(unsigned int icompstr = 0; icompstr < (*compStr)[icomp].size(); icompstr++){
                    //goes from 1-80
                    int compStrip = compStr->at(icomp).at(icompstr);
                    int compHStrip = compHS->at(icomp).at(icompstr);

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


                    int halfStripVal;
                    if(me11a ||me11b || !(thisCompLay%2)){ //if we are in me11 or an even layer (opposite from Cameron's code, since I shift to zero)
                        halfStripVal = 2*(compStrip-1)+compHStrip+1;
                    } else { //odd layers shift down an extra half strip
                        halfStripVal = 2*(compStrip-1)+compHStrip;
                    }


                    if(halfStripVal >= N_MAX_HALF_STRIPS || halfStripVal < 0) {
                        printf("Error: For compId = %i, ST = %i, RI = %i Comp Half Strip Value out of range index = %i - me11b = %i\n",
                                chSid,ST,RI, halfStripVal, me11b);
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

            //lct = local charge track,  alct = anode (wire), clct = cathode (strip)
            //these are all the lct's that were recorded in the run.
            /*
            unsigned int LCT_ID = 0;
            for(unsigned int ilct = 0; ilct < lctId->size(); ilct++)
            {
                //skip until we find a match
                if(lctId->at(ilct) != chSid) continue;
                if(!lctPat->at(ilct).size()) break; //if its not in this one, it wont be in any

                int bestLCTFit = 0; //index of the best fit lct to the segment
                float minDiff = 999999;
                for(unsigned int chLct = 0; chLct < lctKHS->at(ilct).size(); chLct++)
                {
                    //lct - 0.5 - 2*N_S + 0.5 , segX - 0 -N_S
                    float lctStrip = 0.5*(lctKHS->at(ilct).at(chLct)- 0.5);

                    float thisDiff = lctStrip -segX->at(thisSeg);
                    if(abs(thisDiff) < abs(minDiff)) {
                        minDiff = thisDiff;
                        bestLCTFit = chLct; //assign the best index to the one we are on now
                    }
                }
                LCT_ID = lctPat->at(ilct).at(bestLCTFit);
                break;
            }
			*/



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

                if(iRhStrip >= N_MAX_HALF_STRIPS || iRhStrip < 0){
                    printf("ERROR: recHit index %i invalid\n", iRhStrip);
                    return -1;
                }

                theseRHHits.hits[iRhStrip][iLay] = true;
            }



            //find which group the current chamber you are looking at belongs to
            unsigned int groupIndex;
            if((ST == 1 && RI == 4 )||(ST == 1 && RI == 2)) groupIndex = 0; //ME11a || ME12
            else if((ST == 1 && RI == 1) || (ST == 1 && RI == 3)) groupIndex = 1; //ME11b || ME13
            else if((ST == 2 && RI == 1) || (ST == 2 && RI == 2)) groupIndex = 2; //ME21 || ME22
            else if(ST == 3 || ST == 4) groupIndex = 3; //ME3X || ME4X
            else{
                printf("ERROR: can't find group\n");
                return -1;
            }



            EnvelopeSetMatchInfo* newSetMatch = new EnvelopeSetMatchInfo();
            EnvelopeSetMatchInfo* oldSetMatch = new EnvelopeSetMatchInfo();
            ChamberHits* testChamber;
            unsigned int nHits; //number of hits, counts at max one from each layer
            if(USE_COMP_HITS){
                testChamber = &theseCompHits;
            } else {
                testChamber = &theseRHHits;
            }


            nChambersRanOver++;

            //now run on comparator hits
            if(searchForMatch(*testChamber, oldEnvelopes,oldSetMatch) || searchForMatch(*testChamber, newEnvelopes,newSetMatch)) {
            		delete newSetMatch;
            		delete oldSetMatch;
            		nChambersMultipleInOneLayer++;
            		continue;
            }


            //check if we pass the cut
            if(oldSetMatch->bestLayerCount() < N_LAYER_REQUIREMENT || newSetMatch->bestLayerCount() < N_LAYER_REQUIREMENT) {
                delete newSetMatch;
                delete oldSetMatch;
            		continue;
            }


            // Fill Tree Data

            patX = newSetMatch->bestX()/2.;
            patternId = newSetMatch->bestChargeComparatorCode().getComparatorCodeId();
            envelopeId = newSetMatch->bestEnvelopeId();
            legacyLctId = oldSetMatch->bestEnvelopeId();
            legacyLctX = oldSetMatch->bestX()/2.;


            plotTree->Fill();


            delete newSetMatch;
            delete oldSetMatch;
        }

    }

	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

    plotTree->Write();
    outF->Close();


    return 0;
}
