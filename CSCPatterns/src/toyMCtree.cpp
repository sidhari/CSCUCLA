#define toyMCtree_cxx
#include "toyMCtree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void toyMCtree::Loop(int Nev)
{
    Event_EventNumber     = 0xdeadbeef;
    Event_RunNumber       = 420;
    Event_LumiSection     = 123;
    Event_BXCrossing      = 135;

    //Loop over muons applying the selection used for the timing analysis
    for (int i = 0; i < Nev; i++) 
    {
        os = true;
        ss = false;

        //reset branch vars
        segEc->clear();
        segSt->clear();
        segRi->clear();
        segCh->clear();
        segX->clear();
        segY->clear();
        segdXdZ->clear();
        segdYdZ->clear();

        rhId->clear();
        rhLay->clear();
        rhPos->clear();
        rhE->clear();
        rhMax->clear();

        lctId->clear();
        lctQ->clear();
        lctPat->clear();
        lctKWG->clear();
        lctKHS->clear();
        lctBend->clear();
        lctBX->clear();

        csctflctId->clear();
        csctflctQ->clear();
        csctflctPat->clear();
        csctflctKWG->clear();
        csctflctKHS->clear();
        csctflctBend->clear();
        csctflctBX->clear();

        emtflctId->clear();
        emtflctQ->clear();
        emtflctPat->clear();
        emtflctKWG->clear();
        emtflctKHS->clear();
        emtflctBend->clear();
        emtflctBX->clear();

        clctId->clear();
        clctQ->clear();
        clctPat->clear();
        clctKHS->clear();
        clctCFEB->clear();
        clctBend->clear();
        clctBX->clear();
        clctFBX->clear();

        alctId->clear();
        alctQ->clear();
        alctKWG->clear();
        alctAc->clear();
        alctPB->clear();
        alctBX->clear();
        alctFBX->clear();

        compId->clear();
        compLay->clear();
        compStr->clear();
        compHS->clear();
        compTimeOn->clear();

        wireId->clear();
        wireLay->clear();
        wireGrp->clear();
        wireTimeOn->clear();

        stripId->clear();
        stripLay->clear();
        strip->clear();
        stripADCs->clear();

        dduId->clear();
        dduHeader->clear();
        dduTrailer->clear();

        dmbId->clear();
        dmbHeader->clear();
        dmbTrailer->clear();

        tmbId->clear();
        tmbHeader->clear();
        tmbTrailer->clear();

        Pt=200.0;
        eta=2.1;
        phi=0.1;
        q=1;

        int chamber = 101;

        //Match selected muons to CSCSegments
        for (int seg = 0; seg < 1; seg++) 
        {
            // Create the ChamberId
            Nseg = 1;
            segEc->push_back(rand.Integer(2)*2 - 1);
            segSt->push_back(1);
            segRi->push_back(1);
            segCh->push_back(rand.Integer(36));
            chamber = chamberSerial(segEc->at(0), segSt->at(0), segRi->at(0), segCh->at(0));
            segX->push_back(rand.Uniform()*80.0);
            segdXdZ->push_back(rand.Uniform()*5.0 - 2.5);
            segY->push_back(rand.Uniform()*200.0 - 100.0);
            segdYdZ->push_back(rand.Uniform()*2.0 - 1.0);

            for (int rh = 0; rh < 6; rh++)
            {
                float pos = segX->at(0) + segdXdZ->at(0)*float(rh - 3);
                if (pos < 1.0 || pos > 80.0) continue;
                rhId->push_back(chamber);
                rhLay->push_back(rh+1);
                rhPos->push_back(segX->at(0) + segdXdZ->at(0)*float(rh - 3));
                rhE->push_back(rand.Landau(250.0,65.0));
                rhMax->push_back(1.9);
            }

            lctId->push_back(chamber);
            vector<int> lctQBuf (1,rand.Integer(4)+7);
            lctQ->push_back(lctQBuf);
            vector<int> lctPatBuf (1,rand.Integer(9)+2);
            lctPat->push_back(lctPatBuf);
            vector<int> lctKWGBuf (1,rand.Integer(16));
            lctKWG->push_back(lctKWGBuf);
            vector<int> lctKHSBuf (1,(int)TMath::Floor(segX->at(0)*2.0));
            lctKHS->push_back(lctKHSBuf);
            vector<int> lctBendBuf (1,1);
            lctBend->push_back(lctBendBuf);
            vector<int> lctBxBuf (1,1);
            lctBX->push_back(lctBxBuf);

            emtflctId->push_back(chamber);
            emtflctQ->push_back(lctQBuf);
            emtflctPat->push_back(lctPatBuf);
            emtflctKWG->push_back(lctKWGBuf);
            emtflctKHS->push_back(lctKHSBuf);
            emtflctBend->push_back(lctBendBuf);
            emtflctBX->push_back(lctBxBuf);

            csctflctId->push_back(chamber);
            csctflctQ->push_back(lctQBuf);
            csctflctPat->push_back(lctPatBuf);
            csctflctKWG->push_back(lctKWGBuf);
            csctflctKHS->push_back(lctKHSBuf);
            csctflctBend->push_back(lctBendBuf);
            csctflctBX->push_back(lctBxBuf);

            clctId->push_back(chamber);
            clctQ->push_back(lctQBuf);
            clctPat->push_back(lctPatBuf);
            clctKHS->push_back(lctKHSBuf);
            vector<int> clctCFEBBuf (1,lctKHSBuf[0]/32);
            clctCFEB->push_back(clctCFEBBuf);
            clctBend->push_back(lctBendBuf);
            clctBX->push_back(lctBxBuf);
            clctFBX->push_back(lctBxBuf);

            alctId->push_back(chamber);
            alctQ->push_back(lctQBuf);
            alctKWG->push_back(lctKWGBuf);
            alctAc->push_back(lctBxBuf);
            alctPB->push_back(lctBxBuf);
            alctBX->push_back(lctBxBuf);
            alctFBX->push_back(lctBxBuf);

            for(unsigned int rh = 0; rh < rhLay->size(); rh++)
            {
                // Extract Comparator Data
                vector<int> compStrBuf;
                vector<int> compHSBuf;
                vector<int> timeOnBuf (1,rand.Integer(4)+6);
                vector<int> emptyBuf;
                vector<vector<int> > compTimeOnBuf;
                int lay = rhLay->at(rh);
                float rhX = rhPos->at(rh);
                float rhEn = rhE->at(rh);
                float compX = rand.Gaus(rhX+0.50/rhEn, 0.045) + 0.5;
                compId->push_back(chamber);
                compLay->push_back(lay);
                if (compX > 1.0 && rhX > 0.0 && rhEn > 70.0)
                {
                    int str = (int) TMath::Floor(compX);
                    int hs = (int) ( compX > float(str) + 0.5);
                    compStrBuf.push_back( str );
                    compHSBuf.push_back( hs );
                    compTimeOnBuf.push_back(timeOnBuf);
                    compStr->push_back(compStrBuf);
                    compHS->push_back(compHSBuf);
                    compTimeOn->push_back(compTimeOnBuf);
                }
                else
                {
                    //compStrBuf.push_back( emptyBuf );
                    //compHSBuf.push_back( emptyBuf );
                    compTimeOnBuf.push_back(emptyBuf);
                    compStr->push_back(emptyBuf);
                    compHS->push_back(emptyBuf);
                    compTimeOn->push_back(compTimeOnBuf);
                }

                //Wires
                wireId->push_back(chamber);
                wireLay->push_back(lay);
                vector<int> wgOn;
                vector<int> timeOn;
                vector< vector<int> > wgTimes;
                timeOn.push_back(6);
                wgTimes.push_back(timeOn);
                wgOn.push_back(lctKWGBuf.at(0));
                wireGrp->push_back(wgOn);
                wireTimeOn->push_back(wgTimes);

                //Strips
                stripId->push_back(chamber);
                stripLay->push_back(lay);
                vector<int> stripBuf;
                vector<int> ADCBuf;
                vector< vector<int> > stripADCBuf;
                stripBuf.push_back((int)TMath::Floor(rhPos->at(rh) - 1));
                stripBuf.push_back((int)TMath::Floor(rhPos->at(rh)));
                stripBuf.push_back((int)TMath::Floor(rhPos->at(rh) + 1));
                ADCBuf.push_back(12); ADCBuf.push_back(507); ADCBuf.push_back(667); ADCBuf.push_back(512); ADCBuf.push_back(112);
                stripADCBuf.push_back(ADCBuf); stripADCBuf.push_back(ADCBuf); stripADCBuf.push_back(ADCBuf);
                strip->push_back(stripBuf);
                stripADCs->push_back(stripADCBuf);
            }



        }// Matched CSCSegment loop

        //ddu status digis
        dduId->push_back(chamber);
        vector<int> dduHeaderBuf;
        vector<int> dduTrailerBuf;
        for(int i = 0; i < 12; i++)
        {
            dduHeaderBuf.push_back(0xdead);
            dduTrailerBuf.push_back(0xbeef);
        }
        dduHeader->push_back(dduHeaderBuf);
        dduTrailer->push_back(dduTrailerBuf);

        //dmb status digis
        dmbId->push_back(chamber);
        vector<int> dmbHeaderBuf;
        vector<int> dmbTrailerBuf;
        for(int i = 0; i < 8; i++)
        {
            dmbHeaderBuf.push_back(0xdead);
            dmbTrailerBuf.push_back(0xbeef);
        }
        dmbHeader->push_back(dmbHeaderBuf);
        dmbTrailer->push_back(dmbTrailerBuf);

        //tmb status digis
        tmbId->push_back(chamber);
        vector<int> tmbHeaderBuf;
        vector<int> tmbTrailerBuf;
        for(int i = 0; i < 43; i++)
        {
            tmbHeaderBuf.push_back(0xdead);
            tmbTrailerBuf.push_back(0xbeef);
        }
        tmbHeader->push_back(tmbHeaderBuf);
        tmbTrailer->push_back(tmbTrailerBuf);

        tree->Fill();
    }
    TFile * outF = new TFile("../data/toyMCtree.root","RECREATE");
    outF->cd();
    tree->Write();
    outF->Write();
    outF->Close();
}













