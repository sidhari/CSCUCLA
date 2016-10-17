#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h"
#include "../src/hsData.cc"
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
    //   In a ROOT session, you can do:
    //      root> .L CSCDigiTree.C
    //      root> CSCDigiTree t
    //      root> t.GetEntry(12); // Fill t data members with entry number 12
    //      root> t.Show();       // Show values of entry 12
    //      root> t.Show(16);     // Read and show values of entry 16
    //      root> t.Loop();       // Loop on all entries
    //

    //     This is the loop skeleton where:
    //    jentry is the global entry number in the chain
    //    ientry is the entry number in the current Tree
    //  Note that the argument to GetEntry must be:
    //    jentry for TChain::GetEntry
    //    ientry for TTree::GetEntry and TBranch::GetEntry
    //
    //       To read only selected branches, Insert statements like:
    // METHOD1:
    //    fChain->SetBranchStatus("*",0);  // disable all branches
    //    fChain->SetBranchStatus("branchname",1);  // activate branchname
    // METHOD2: replace line
    //    fChain->GetEntry(jentry);       //read all branches
    //by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) return;

    HistGetter plotter;

    plotter.book2D("compHS_h","Comparator HS Value",251,-0.5,250.5,10,-0.5,9.5);

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
        int Ntflct = 0;
        float missID = -999.9;
        //Loop over segments
        for(int iseg = 0; iseg < Nseg; iseg++)
        {
            int EC = segEc->at(iseg);
            int ST = segSt->at(iseg);
            int RI = segRi->at(iseg);
            int CH = segCh->at(iseg);
            int chSid = chamberSerial(EC, ST, RI, CH);

            vector<int> compBuf;
            vector<int> layBuf;
            vector<vector<int>> timeOnBuf;

            for(int icomp = 0; icomp < int(compId->size()); icomp++)
            {
                if(!(chSid == compId->at(icomp))) continue;
                int layN = compLay->at(icomp);
                for(int jcomp = 0; jcomp < (int) compStr->at(icomp).size(); jcomp++)
                {
                    int cPos = 2*(compStr->at(icomp).at(jcomp) - 1) + compHS->at(icomp).at(jcomp);
                    compBuf.push_back(cPos);
                    layBuf.push_back(layN);
                    timeOnBuf.push_back(compTimeOn->at(icomp).at(jcomp));
                    plotter.get2D("compHS_h")->Fill(cPos,getCT(ST,RI));
                }//jcomp
            }//icomp
            cout << "Fill the comps! CT is " << getCT(ST,RI) << " pt: " << Pt << endl;
            hsData comps(compBuf,layBuf,timeOnBuf,getCT(ST,RI));
            cout << "Print the comps!" << endl;
            comps.print();
            cout << endl << "Max # of Layer Sum with persist 4" << endl;
            vector<vector<bool>> timeSum = comps.getSumMax(4);
            for(int ll = 0; ll < NLAY; ll++)
            {
                if(ll%2==0) cout << " ";
                for(int hs = 0; hs < int(timeSum[ll].size()); hs++)
                {
                    if(timeSum[ll][hs]) cout << "x";
                    else cout << "-";
                }
                cout << endl;
            }

        }//iseg
    }
    plotter.write((sName+".root").c_str());
}

