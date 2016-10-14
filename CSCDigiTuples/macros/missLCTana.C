#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h"
#include "include/HistGetter.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

void CSCDigiTree::Loop(string sName)
{
    if (fChain == 0) return;
    bool op = false;
    HistGetter plotter;

    plotter.book1D("NcscLCT_seg_h","Number of CSC LCTs per Segment;Number of CSC LCTs;Entries",5,-0.5,4.5);
    plotter.book1D("cscTFBX_h","CSCTF Bunch Crossing Distribution;BX of cscTF;Entries",16,-0.5,15.5);
    plotter.book1D("emTFBX_h","EMTF Bunch Crossing Distribution;BX of emTF;Entries",16,-0.5,15.5);
    plotter.book1D("cscTFBX_missLCT_h","CSCTF Bunch Crossing for Events missing CSC LCTs;BX of cscTF;Entries",16,-0.5,15.5);
    plotter.book1D("emTFBX_missLCT_h","EMTF Bunch Crossing for Events missing CSC LCTs;BX of emTF;Entries",16,-0.5,15.5);

    plotter.book1D("runNum_missLCT_h","Run Number of Events missing LCTs;Run Number;Entries",1000,276000.5,277000.5);
    plotter.book1D("bxNum_missLCT_h","BX Number of Events missing LCTs;Run Number;Entries",3565,-0.5,3564.5);
    plotter.book1D("lsNum_missLCT_h","Lumi Section of Events missing LCTs;Lumi Section;Entries",1601,-0.5,1600.5);

    plotter.book2D("cscLCT_emtfLCT_h","Number of LCTs from CSCs and emTF;Number of LCTs from emTF;Number of LCTs from CSCs",12,-0.5,11.5,12,-0.5,11.5);
    plotter.book2D("cscLCT_csctfLCT_h","Number of LCTs from CSCs and cscTF;Number of LCTs from cscTF;Number of LCTs from CSCs",12,-0.5,11.5,12,-0.5,11.5);

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries/1;jentry++) 
    {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        if(jentry%(nentries/100) == 0) cout << "Loading event " << jentry << " out of " << nentries << endl;
        if(!os) continue;

        int Nlct = 0;
        int Ncsctflct = 0;
        int Nemtflct = 0;
        vector<int> emtfbxV;
        vector<int> csctfbxV;
        //Loop over segments
        for(int iseg = 0; iseg < Nseg; iseg++)
        {
            int EC = segEc->at(iseg);
            int ST = segSt->at(iseg);
            int RI = segRi->at(iseg);
            int CH = segCh->at(iseg);
            int chSid = chamberSerial(EC, ST, RI, CH);

            int Nlct_seg = 0;

            //Count LCTs
            for(int ilct = 0; ilct < (int) lctId->size(); ilct++)
            {
                if(!(chSid == lctId->at(ilct))) continue;
                for(int jlct = 0; jlct < int(lctQ->at(ilct).size()); jlct++)
                {
                    Nlct++;
                    Nlct_seg++;
                }
            }
            plotter.get1D("NcscLCT_seg_h")->Fill(Nlct_seg);

            int Ncsctflct_seg = 0;
            //Count csctfLCTs
            for(int icsctflct = 0; icsctflct < (int) csctflctId->size(); icsctflct++)
            {
                if(!(chSid == csctflctId->at(icsctflct))) continue;
                for(int jcsctflct = 0; jcsctflct < int(csctflctQ->at(icsctflct).size()); jcsctflct++)
                {
                    Ncsctflct++;
                    Ncsctflct_seg++;
                    int BX = csctflctBX->at(icsctflct).at(jcsctflct);
                    csctfbxV.push_back(BX);
                    plotter.get1D("cscTFBX_h")->Fill(BX);
                }
            }

            int Nemtflct_seg = 0;
            //Count emtfLCTs
            for(int iemtflct = 0; iemtflct < (int) emtflctId->size(); iemtflct++)
            {
                if(!(chSid == emtflctId->at(iemtflct))) continue;
                for(int jemtflct = 0; jemtflct < int(emtflctQ->at(iemtflct).size()); jemtflct++)
                {
                    Nemtflct++;
                    Nemtflct_seg++;
                    int BX = emtflctBX->at(iemtflct).at(jemtflct);
                    emtfbxV.push_back(BX);
                    plotter.get1D("emTFBX_h")->Fill(BX);
                }
            }

        }//iseg
        plotter.get2D("cscLCT_emtfLCT_h")->Fill(Nemtflct,Nlct);
        plotter.get2D("cscLCT_csctfLCT_h")->Fill(Ncsctflct,Nlct);
        if(Nlct == 0 && Ncsctflct > 0)
        {
            for(int i = 0; i < int(csctfbxV.size()); i++)
            {
                plotter.get1D("cscTFBX_missLCT_h")->Fill(csctfbxV[i]);
            }
        }

        if(Nlct == 0 && Nemtflct > 0)
        {
            plotter.get1D("runNum_missLCT_h")->Fill(Event_RunNumber);
            plotter.get1D("bxNum_missLCT_h")->Fill(Event_BXCrossing);
            plotter.get1D("lsNum_missLCT_h")->Fill(Event_LumiSection);
            for(int i = 0; i < int(emtfbxV.size()); i++)
            {
                plotter.get1D("emTFBX_missLCT_h")->Fill(emtfbxV[i]);
            }
        }

    }

    plotter.write(Form("%sCSCDigiTreeAna.root",sName.c_str()));

}

