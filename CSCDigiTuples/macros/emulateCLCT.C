#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h"
//#include "../src/hsData.cc"
#include "../src/patFilter.cc"
#include "include/HistGetter.h"
#include "include/convCT.h"

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <math.h>

using namespace std;

void CSCDigiTree::Loop(string sName)
{
    if (fChain == 0) return;

    HistGetter plotter;
    plotter.book1D("dataEmuDiff_KHS_h","Difference of Emulated and Digi CLCT KHS",21,-10.5,10.5);
    plotter.book1D("dataEmuDiff_pid_h","Difference of Emulated and Digi CLCT PID",21,-10.5,10.5);
    plotter.book1D("KHS_h","Difference of Emulated and Digi CLCT KHS",200,-0.5,199.5);
    plotter.book1D("pid_h","Difference of Emulated and Digi CLCT PID",10,0.5,10.5);
    plotter.book1D("Nlay_h","Number of Layers in CLCT",6,0.5,6.5);
    plotter.book1D("T_h","Time bin of CLCT",16,-0.5,15.5);
    plotter.book2D("dataEmu_KHS_h","Emulated and Digi CLCT KHS",201,-0.5,200.5,201,-0.5,200.5);
    plotter.book2D("dataEmu_pid_h","Emulated and Digi CLCT PID",10,-0.5,10.5,10,-0.5,10.5);
    plotter.book2D("Nlay_pid_h","Emulated and Digi CLCT PID",7,-0.5,6.5,10,-0.5,10.5);


    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries/1;jentry++) 
    {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        if (jentry == 100) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        //if(jentry%(nentries/1000) == 0) cout << "Loading event " << jentry << " out of " << nentries << endl; if(!os) continue;
        if(jentry%10 == 0) cout << "Loading event " << jentry << " out of " << nentries << endl;
        //cout << "Loading event " << jentry << " out of " << nentries << endl; 
        if(!os) continue;

        int Nlct = 0;
        int Ntflct = 0;
        float missID = -999.9;
        //Loop over segments
        for(int iseg = 0; iseg < Nseg; iseg++)
        {
            int EC = segEc->at(iseg);
            int ST = segSt->at(iseg);
            int RI = segRi->at(iseg);
            int CH = segCh->at(iseg);
            int CT = getCT(ST,RI);
            //cout << endl << "Segment in Chamber ME" << ((EC == 1) ? "+" : "-") << ST << "/" << RI << "/" << CH << endl;
            int chSid = chamberSerial(EC, ST, RI, CH);

            vector<int> compBuf;
            vector<int> layBuf;
            vector<vector<int>> timeOnBuf;

            //Build comparator data structure
            for(int icomp = 0; icomp < int(compId->size()); icomp++)
            {
                if(!(chSid == compId->at(icomp))) continue;
                int layN = compLay->at(icomp);
                for(int jcomp = 0; jcomp < (int) compStr->at(icomp).size(); jcomp++)
                {
                    if(jcomp > 0) break;
                    int cPos = 2*(compStr->at(icomp).at(jcomp) - 1) + compHS->at(icomp).at(jcomp);
                    compBuf.push_back(cPos);
                    layBuf.push_back(layN);
                    timeOnBuf.push_back(compTimeOn->at(icomp).at(jcomp));
                }//jcomp
            }//icomp
            hsData comps(compBuf,layBuf,timeOnBuf,getCT(ST,RI));
            patFilter patF("pats/p5now.pat");

            //Now loop over lcts and filter comps with them
            for(int ilct = 0; ilct < int(lctId->size()); ilct++)
            {
                if(!(chSid == lctId->at(ilct))) continue;
                for(int jlct = 0; jlct < int(lctPat->at(ilct).size()); jlct++)
                {
                    int pat = lctPat->at(ilct).at(jlct);
                    int khs = lctKHS->at(ilct).at(jlct);
                    patF.emulate(comps);
                    plotter.get1D("dataEmuDiff_KHS_h")->Fill(khs - patF.getEmuKHS(0));
                    plotter.get1D("dataEmuDiff_pid_h")->Fill(pat - patF.getEmuPatID(0));
                    plotter.get2D("dataEmu_KHS_h")->Fill(khs,patF.getEmuKHS(0));
                    plotter.get2D("dataEmu_pid_h")->Fill(pat,patF.getEmuPatID(0));
                    plotter.get2D("Nlay_pid_h")->Fill(patF.getEmuNlay(0),patF.getEmuPatID(0));
                    plotter.get1D("KHS_h")->Fill(patF.getEmuKHS(0));
                    plotter.get1D("pid_h")->Fill(patF.getEmuPatID(0));
                    plotter.get1D("T_h")->Fill(patF.getEmuTime(0));
                    plotter.get1D("Nlay_h")->Fill(patF.getEmuNlay(0));
                }
            }
        }//iseg
    }
    plotter.write( (sName+".root").c_str() );
}

