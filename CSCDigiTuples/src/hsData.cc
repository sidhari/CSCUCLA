#include "../include/hsData.h"
#include <iostream>

using namespace std;

hsData::hsData(vector<int> patData, vector<int> patLayer, vector<vector<int>> timeOn, int CT)
{
    count = 0;
    ct = CT;
    for(int t = 0; t < 16; t++)
    {
        vector<vector<bool>> timeBuf;
        for(int ll = 0; ll < NLAY; ll++)
        {
            vector<bool> layBuf;
            for(int i = 0; i < ct2NHS[CT]; i++)
            {
                layBuf.push_back(0);
            }
            timeBuf.push_back(layBuf);
        }
        data.push_back(timeBuf);
    }
    //if(patData.size() > 0) cout << patLayer[0] << "   " << patData[0] << endl;

    for(int i = 0; i < int(patData.size()); i++)
    {
        for(int t = 0; t < int(timeOn[i].size()); t++)
        {
            data[timeOn[i][t]][patLayer[i]-1][patData[i]] = 1;
            count++;
        }
    }
}

hsData::~hsData()
{
}

vector< vector<bool> > hsData::getTimeSum(int it, int ft)
{
    vector< vector<bool> > sum;
    for(int lay = 0; lay < NLAY; lay++)
    {
        vector<bool> layBuf (data[it][lay].size(),false);
        for(int t = it; t <= ft; t++)
        {
            for(int hs = 0; hs < int(data[t][lay].size()); hs++)
            {
                layBuf[hs] = layBuf[hs] + data[t][lay][hs];
            }
        }
        sum.push_back(layBuf);
    }
    return sum;
}

vector< vector<bool> > hsData::getSumMax(int persist)
{
    int NlayMax = -1;
    vector< vector<bool> > sumMax;
    for(int t = persist; t < 17; t++)
    {
        int Nlay = 0;
        vector< vector<bool> > sumBuf = getTimeSum(t-persist,t-1);
        for(int lay = 0; lay < NLAY; lay++)
        {
            for(int hs = 0; hs < int(sumBuf[lay].size()); hs++)
            {
                if(bool(sumBuf[lay][hs])) {Nlay++; break;}
            }
        }
        if(Nlay > 6) cout << "Somehow you magically have more than 6 layers?" << endl;
        if(Nlay == 6)
        {
            sumMax = sumBuf;
            break;
        }
        if(Nlay > NlayMax)
        {
            sumMax = sumBuf;
            NlayMax = Nlay;
        }
    }
    return sumMax;
}

vector<bool> hsData::getHS(int hs, int time)
{
    vector<bool> hsBuf;
    for(int ll = 0; ll < NLAY; ll++)
    {
        hsBuf.push_back(data[time][ll][hs]);
    }
    return hsBuf;
}

void hsData::print()
{
    for(int t = 0; t < 16; t++)
    {
        cout << endl << "t = " << t << endl;
        for(int ll = 0; ll < NLAY; ll++)
        {
            if(ll%2==0) cout << " ";
            for(int hs = 0; hs < int(data[t][ll].size()); hs++)
            {
                if(bool(data[t][ll][hs])) cout << "x";
                else cout << "-";
            }
            cout << endl;
        }
    }
}
