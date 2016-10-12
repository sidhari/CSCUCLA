#include "../src/hsData.cc"
#include "../include/patFilter.h"

#include <fstream>
#include <iostream>

using namespace std;

patFilter::patFilter(string filename)
{
    ifstream inF(filename.c_str());
    char cBuf;
    string sBuf;
    for(int pat = 10; pat > 1; pat--)
    {
        for(int lay = 0; lay < 6; lay++)
        {
            for(int hs = 0; hs < 11; hs++)
            {
                inF.get(cBuf);
                sBuf = cBuf;
                pats[pat][lay][hs] = (sBuf == "x");
            }
            inF.get(cBuf);
        }
        inF.get(cBuf);
    }

    emu = false;
    emuKHS0 = -99;
    emuPatID0 = -9;
    emuKHS1 = -99;
    emuPatID1 = -9;
    Nlay0 = -9;
    T0 = -9;
    Nlay1 = -9;
    T1 = -9;
    clct_persist = 4;
}

patFilter::~patFilter()
{
}

vector<vector<bool>> patFilter::filter(hsData data, int pat, int khs)
{
    vector<vector<bool>> sumBuf = data.getSumMax(4);
    int CT = data.getCT();
    bool stag = true;
    if(CT == 6 || CT == 9) stag = false;
    for(int lay = 0; lay < 6; lay++)
    {
        for(int hs = 0; hs < int(sumBuf[lay].size()); hs++)
        {
            if(lay%2 == 1 && stag)
            {
                if(hs >= khs - 4 && hs <= khs + 6) sumBuf[lay][hs] = ( sumBuf[lay][hs] && pats[pat][lay][hs-(khs-4)] );
                else sumBuf[lay][hs] = 0;
            }
            else
            {
                if(hs >= khs - 5 && hs <= khs + 5) sumBuf[lay][hs] = ( sumBuf[lay][hs] && pats[pat][lay][hs-(khs-5)] );
                else sumBuf[lay][hs] = 0;
            }
        }
    }
    return sumBuf;
}

void patFilter::emulate(hsData data, bool debug)
{
    //Get the Chamber type for this data and set stager bool
    int CT = data.getCT();
    bool stag = true;
    if(CT == 6 || CT == 9) stag = false;

    //initialize vector to hold extended data
    vector<vector<vector<bool>>> EXdata;
    vector<vector<bool>> initData = data.getTime(0);
    for(int ll = 0; ll < int(initData.size()); ll++)
    {
        for(int hs = 0; hs < int(initData[ll].size()); hs++)
        {
            initData[ll][hs] = 0;
        }
    }
    for(int t = 0; t < 19; t++)
    {
        EXdata.push_back(initData);
    }

    //Fill extended data with data
    for(int t = 0; t < 16; t++)
    {
        vector<vector<bool>> dataNow = data.getTime(t);
        for(int lay = 0; lay < dataNow.size(); lay++)
        {
            for(int hs = 0; hs < dataNow[lay].size(); hs++)
            {
                if(dataNow[lay][hs])
                {
                    for(int xt = 0; xt < 4; xt++)
                    {
                        EXdata[t+xt][lay][hs] = 1;
                    }
                }
            }
        }
    }

    //Apply Pattern Filter to each time bin and khs and fill highest pattern number into a vector to parse later
    vector<vector<int>> tPids;
    vector<vector<int>> tNlays;
    if(debug) cout << "Start applying filters" << endl;
    for(int t = 0; t < 16; t++)
    {
        vector<vector<bool>> dataNow = EXdata[t];
        vector<int> khsPids;
        vector<int> khsNlays;
        if(debug) { cout << "Data for t = " << t << endl; printVector(dataNow,stag); }
        for(int khs = 0; khs < dataNow[0].size(); khs++)
        {
            if(debug) cout << "Apply filters at khs " << khs << endl;
            int NlayMax = -1;
            int Bpat = -1;
            for(int pat = 10; pat > 1; pat--)
            {
                if(debug) cout << "Pattern " << pat << endl;
                vector<vector<bool>> dataHereNow = dataNow;
                for(int lay = 0; lay < int(dataHereNow.size()); lay++)
                {
                    for(int hs = 0; hs < int(dataHereNow[lay].size()); hs++)
                    {
                        if(lay%2 == 1 && stag)
                        {
                            if(hs >= khs - 4 && hs <= khs + 6) dataHereNow[lay][hs] = ( dataHereNow[lay][hs] && pats[pat][lay][hs-(khs-4)] );
                            else dataHereNow[lay][hs] = 0;
                        }
                        else
                        {
                            if(hs >= khs - 5 && hs <= khs + 5) dataHereNow[lay][hs] = ( dataHereNow[lay][hs] && pats[pat][lay][hs-(khs-5)] );
                            else dataHereNow[lay][hs] = 0;
                        }
                    }
                }//lay  Everthing inside here is applying the pat filter

                //Now count Nlay
                int Nlay = 0;
                for(int lay = 0; lay < int(dataHereNow.size()); lay++)
                {
                    for(int khs = 0; khs < int(dataHereNow[lay].size()); khs++)
                    {
                        if(dataHereNow[lay][khs]){Nlay++; break;}
                    }
                }
                if(debug && Nlay > 0) 
                {
                    printVector(dataHereNow,stag);
                    cout << endl;
                    cout << "Number of layers: " << Nlay << endl;
                }
                if(Nlay > NlayMax) {NlayMax = Nlay; Bpat = pat;}
                //if(Nlay == 6) break;//If this happens you found the solution for this khs
            }//pat  For each pattern, filter data through pat and count layers. If Nlay > NlayMax update buffer
            if(debug) cout << endl;
            khsPids.push_back(Bpat);
            khsNlays.push_back(NlayMax);
        }//khs  For each khs apply all patterns and decide if it is better than anything found so far
        tPids.push_back(khsPids);
        tNlays.push_back(khsNlays);
    }//t  For each time bin calculate NlayMax for each khs
    emuPatID0 = 0;
    emuKHS0 = 0;
    Nlay0 = 0;
    T0 = 0;
    int maxNlay = 0;
    int maxPid = 0;
    int bestKHS = -99;
    for(int t = 0; t < 16; t++)
    {
        if(debug) cout << "t = " << t << endl;
        for(int khs = 0; khs < tPids[t].size(); khs++)
        {
            if(debug && tNlays[t][khs] > 0) cout << tNlays[t][khs] << " ";
        }
        if(debug) cout << endl;
        for(int khs = 0; khs < tPids[t].size(); khs++)
        {
            if(tNlays[t][khs] > maxNlay) {maxNlay = tNlays[t][khs]; maxPid = tPids[t][khs]; bestKHS = khs;}
            else if(tNlays[t][khs] == maxNlay && tPids[t][khs] > maxPid) {maxNlay = tNlays[t][khs]; maxPid = tPids[t][khs]; bestKHS = khs;}
            if(debug && tNlays[t][khs] > 0) cout << tPids[t][khs] << " ";
        }
        if(debug) cout << endl << endl;
        if(maxNlay < 4) continue;
        if(maxNlay > Nlay0) {emuPatID0 = maxPid; emuKHS0 = bestKHS; Nlay0 = maxNlay; T0 = t;}
        else if(maxNlay == Nlay0 && maxPid > emuPatID0) {emuPatID0 = maxPid; emuKHS0 = bestKHS; Nlay0 = maxNlay; T0 = t;}
    }
    if(debug) cout << "emuPatID0: " << emuPatID0 << " emuKHS0: " << emuKHS0 << " Nlay0: " << Nlay0 << " T0: " << T0 << endl;

    emuPatID1 = 0;
    emuKHS1 = 0;
    Nlay1 = 0;
    T1 = 0;
    maxNlay = 0;
    maxPid = 0;
    bestKHS = -99;
    for(int t = 0; t < 16; t++)
    {
        for(int khs = 0; khs < tPids[t].size(); khs++)
        {
            if(khs > emuKHS0 - 10 && khs < emuKHS0 + 10) continue;
            if(tNlays[t][khs] > maxNlay) {maxNlay = tNlays[t][khs]; maxPid = tPids[t][khs]; bestKHS = khs;}
            else if(tNlays[t][khs] == maxNlay && tPids[t][khs] > maxPid) {maxNlay = tNlays[t][khs]; maxPid = tPids[t][khs]; bestKHS = khs;}
        }
        if(maxNlay < 4) continue;
        if(maxNlay > Nlay1) {emuPatID1 = maxNlay; emuKHS1 = bestKHS; Nlay1 = maxNlay; T1 = t;}
        else if(maxNlay == Nlay1 && maxPid > emuPatID1) {emuPatID1 = maxPid; emuKHS1 = bestKHS; Nlay1 = maxNlay; T1 = t;}
    }
    if(debug) cout << "emuPatID1: " << emuPatID1 << " emuKHS1: " << emuKHS1 << " Nlay1: " << Nlay1 << " T1: " << T1 << endl;
}

void patFilter::print()
{
    cout << endl;
    for(int pat = 10; pat > 1; pat--)
    {
        cout << "Pattern " << pat << ":" << endl;
        for(int lay = 0; lay < 6; lay++)
        {
            for(int hs = 0; hs < 11; hs++)
            {
                cout << pats[pat][lay][hs] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void patFilter::printVector(vector<vector<bool>> data, bool stag)
{
    for(int ll = 0; ll < 6; ll++)
    {
        if(ll%2==0 && stag) cout << " ";
        for(int hs = 0; hs < int(data[ll].size()); hs++)
        {
            if(data[ll][hs]) cout << "x";
            else cout << "-";
        }
        cout << endl;
    }
}

























