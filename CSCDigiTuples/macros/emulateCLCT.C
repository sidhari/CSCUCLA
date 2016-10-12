#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h.old"
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
    plotter.book1D("KHS_h","Emulated CLCT KHS",200,-0.5,199.5);
    plotter.book1D("pid_h","Emulated CLCT PID",10,0.5,10.5);
    plotter.book1D("Nlay_h","Number of Layers in CLCT",6,0.5,6.5);
    plotter.book1D("T_h","Time bin of CLCT",16,-0.5,15.5);
    plotter.book1D("Terr_h","Time bin of CLCT",16,-0.5,15.5);
    plotter.book1D("errCount_h","Emulation Categorization",7,-0.5,6.5);
    plotter.book1D("errCT_h","Emulation Categorization",11,-0.5,10.5);
    plotter.book2D("dataEmu_KHS_h","Emulated and Digi CLCT KHS",201,-0.5,200.5,201,-0.5,200.5);
    plotter.book2D("dataEmu_pid_h","Emulated and Digi CLCT PID",10,-0.5,10.5,10,-0.5,10.5);
    plotter.book2D("Nlay_pid_h","Emulated and Digi CLCT PID",7,-0.5,6.5,10,-0.5,10.5);
    plotter.book2D("Nlay_emuNlay_h","Digi Emulated CLCT Number of Layers;Number of Layers from CLCT;Number of Layers from Emulation",7,-0.5,6.5,7,-0.5,6.5);


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
            bool stag = true;
            if(CT == 6 || CT == 9) stag = false;
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
                    patF.emulate(comps,0);
                    int emuKHS = patF.getEmuKHS(0);
                    int emuPID = patF.getEmuPatID(0);
                    int emuT = patF.getEmuTime(0);
                    int emuNlay = patF.getEmuNlay(0);
                    if(fabs(khs - emuKHS) > fabs(khs - patF.getEmuPatID(1))) 
                    {
                        emuKHS = patF.getEmuPatID(1); 
                        emuPID = patF.getEmuPatID(1);
                        emuT = patF.getEmuTime(1);
                        emuNlay = patF.getEmuNlay(1);
                    }

                    int cKHS = -99;
                    int cPID = -99;
                    int cNlay = -99;
                    for(int iclct = 0; iclct < int(clctId->size()); iclct++)
                    {
                        if(!(chSid == clctId->at(iclct))) continue;
                        for(int jclct = 0; jclct < int(clctKHS->at(iclct).size()); jclct++)
                        {
                            int icKHS = 32*clctCFEB->at(iclct).at(jclct) + clctKHS->at(iclct).at(jclct);
                            int icPID = clctPat->at(iclct).at(jclct);
                            if(icKHS == khs && icPID == pat) {cKHS = icKHS; cPID = icPID; cNlay = clctQ->at(iclct).at(jclct);}
                        }
                    }
                    if(cKHS == -99) cout << "Failed to find CLCT for this LCT!" << endl;
                    plotter.get2D("Nlay_emuNlay_h")->Fill(cNlay,emuNlay);

                    if(0 & emuT == 0)
                    {
                        cout << "Event # " << jentry << endl;
                        cout << "PID does not match. Data PID: " << pat << " Emu PID: " << emuPID;
                        comps.print();
                        cout << endl;
                        vector<vector<bool>> dataFilter = patF.filter(comps,pat,khs);
                        vector<vector<bool>> emuFilter = patF.filter(comps,emuPID,emuKHS);
                        vector<vector<bool>> compSumT = comps.getTimeSum(0,15);
                        cout << "Comps sumed in time:" << endl;
                        patF.printVector(compSumT,stag);
                        cout << endl;
                        cout << "Filtered by Data CLCT:" << endl;
                        patF.printVector(dataFilter,stag);
                        cout << endl;
                        cout << "Filtered by Emulated CLCT:" << endl;
                        patF.printVector(emuFilter,stag);
                        cout << endl;
                        //patF.emulate(comps,1);
                        cout << "data PID: " << pat << " data KHS: " << khs << endl;
                        cout << "emulated PID: " << emuPID << " emulated KHS: " << emuKHS << endl;
                    }

                    plotter.get1D("errCount_h")->Fill(0);
                    if(emuT != 0 && khs - emuKHS == 0 && pat - emuPID == 0) {plotter.get1D("errCount_h")->Fill(1); plotter.get1D("errCT_h")->Fill(CT);}
                    if(emuT != 0 && khs - emuKHS != 0 && pat - emuPID == 0) {plotter.get1D("errCount_h")->Fill(2); plotter.get1D("errCT_h")->Fill(CT);}
                    if(emuT != 0 && khs - emuKHS == 0 && pat - emuPID != 0) {plotter.get1D("errCount_h")->Fill(3); plotter.get1D("errCT_h")->Fill(CT);}
                    if(emuT != 0 && khs - emuKHS != 0 && pat - emuPID != 0) {plotter.get1D("errCount_h")->Fill(4); plotter.get1D("errCT_h")->Fill(CT);}
                    if(emuT == 0) {plotter.get1D("errCount_h")->Fill(5); plotter.get1D("errCT_h")->Fill(CT);}
                    if(khs != emuKHS || pat != emuPID) plotter.get1D("Terr_h")->Fill(emuT);
                    plotter.get1D("dataEmuDiff_KHS_h")->Fill(khs - emuKHS);
                    plotter.get1D("dataEmuDiff_pid_h")->Fill(pat - emuPID);
                    plotter.get2D("dataEmu_KHS_h")->Fill(khs,emuKHS);
                    plotter.get2D("dataEmu_pid_h")->Fill(pat,emuPID);
                    plotter.get2D("Nlay_pid_h")->Fill(emuNlay,emuPID);
                    plotter.get1D("KHS_h")->Fill(emuKHS);
                    plotter.get1D("pid_h")->Fill(emuPID);
                    plotter.get1D("T_h")->Fill(emuT);
                    plotter.get1D("Nlay_h")->Fill(emuNlay);
                }
            }
        }//iseg
    }
    plotter.write( (sName+".root").c_str() );
}

