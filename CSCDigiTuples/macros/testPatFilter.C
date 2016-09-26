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

    plotter.book2D("compHS_h","Comparator HS Value",251,-0.5,250.5,10,-0.5,9.5);
    plotter.book2D("clctKHS_h","Comparator HS Value",251,-0.5,250.5,10,-0.5,9.5);

    Long64_t nentries = fChain->GetEntriesFast();
    bool findEvt = false;

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
            int CT = getCT(ST,RI);
            bool findCh = (EC==2 && ST==1 && RI==2 && CH == 29);
            cout << endl << "Segment in Chamber ME" << ((EC == 1) ? "+" : "-") << ST << "/" << RI << "/" << CH << endl;
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
                    int cPos = 2*(compStr->at(icomp).at(jcomp) - 1) + compHS->at(icomp).at(jcomp);
                    compBuf.push_back(cPos);
                    layBuf.push_back(layN);
                    timeOnBuf.push_back(compTimeOn->at(icomp).at(jcomp));
                    plotter.get2D("compHS_h")->Fill(cPos,CT);
                }//jcomp
            }//icomp
            hsData comps(compBuf,layBuf,timeOnBuf,getCT(ST,RI));
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
            patFilter patF("pats/p5now.pat");
            //patF.print();

            bool findCLCT = false;
            //Now loop over clcts and filter comps with them
            for(int iclct = 0; iclct < int(clctId->size()); iclct++)
            {
                if(!(chSid == clctId->at(iclct))) continue;
                for(int jclct = 0; jclct < int(clctPat->at(iclct).size()); jclct++)
                {
                    int pat = clctPat->at(iclct).at(jclct);
                    int khs = clctKHS->at(iclct).at(jclct) + 32*clctCFEB->at(iclct).at(jclct);
                    if(khs== 115 && pat==9) findCLCT = true;
                    plotter.get2D("clctKHS_h")->Fill(khs,CT);
                    vector<vector<bool>> filterComps = patF.filter(comps,pat,khs);
                    cout << "CLCT with pattern " << pat << " and KHS " << khs << endl;
                    for(int ll = 0; ll < NLAY; ll++)
                    {
                        if(ll%2==0) cout << " ";
                        for(int hs = 0; hs < int(filterComps[ll].size()); hs++)
                        {
                            if(filterComps[ll][hs]) cout << "x";
                            else cout << "-";
                        }
                        cout << endl;
                    }
                    cout << endl;
                }
            }
            findEvt = (findCh&&findCLCT);
            if(findEvt) {comps.print(); break;}
        }//iseg
        if(findEvt) break;
    }
    plotter.write( (sName+".root").c_str() );
}

