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

void patFilter::emulate(hsData data)
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
    vector<int> Nlays;//time
    vector<int> Bpids;//time
    vector<int> Bkhss;//time
    for(int t = 0; t < 16; t++)
    {
        int BNlay = 0;
        int Bpid = 0;
        int Bkhs = -99;
        vector<vector<bool>> dataNow = EXdata[t];
        vector<vector<int>> bPats;
        for(int khs = 0; khs < dataNow[0].size(); khs++)
        {
            int NlayMax = -1;
            int Bpat = -1;
            for(int pat = 10; pat > 1; pat--)
            {
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
                if(Nlay < 4) continue;
                if(Nlay > NlayMax) {NlayMax = Nlay; Bpat = pat;}
                //if(Nlay == 6) break;//If this happens you found the solution for this khs
            }//pat  For each pattern, filter data through pat and count layers. If Nlay > NlayMax update buffer
            if((NlayMax == BNlay && Bpid > Bpat) || NlayMax > BNlay) {BNlay = NlayMax; Bpid = Bpat; Bkhs = khs;}
            //if(BNlay == 6 && Bpid == 10) break;//if this happens this is the solution, so no need to keep looking
        }//khs  For each khs apply all patterns and decide if it is better than anything found so far
        Nlays.push_back(BNlay);
        Bpids.push_back(Bpid);
        Bkhss.push_back(Bkhs);
    }//t  For each time bin calculate NlayMax for each khs
    emuPatID0 = 0;
    emuKHS0 = 0;
    int NlayBuf = 0;
    for(int t = 0; t < 16; t++)
    {
        if(Nlays[t] >= NlayBuf && Bpids[t] > emuPatID0){ emuPatID0 = Bpids[t]; emuKHS0 = Bkhss[t]; T0 = t; Nlay0 = Nlays[t];}
    }
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











































































