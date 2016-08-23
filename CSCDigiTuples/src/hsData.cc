#include "../include/hsData.h"
#include <iostream>

using namespace std;

hsData::hsData(vector<int> patData,vector<int> patLayer,int CT)
{
    count = 0;
    for(int ll = 0; ll < NLAY; ll++)
    {
        vector<bool> layBuf;
        for(int i = 0; i < ct2NHS[CT]; i++)
        {
            layBuf.push_back(0);
        }
        data.push_back(layBuf);
    }

    for(int i = 0; i < int(patData.size()); i++)
    {
        data[patLayer[i]][patData[i]] = 1;
        count++;
    }
}

hsData::~hsData()
{
}

vector<bool> hsData::getHS(int hs)
{
    vector<bool> hsBuf;
    for(int ll = 0; ll < NLAY; ll++)
    {
        hsBuf.push_back(data[ll][hs]);
    }
    return hsBuf;
}

void hsData::print()
{
    for(int ll = 0; ll < NLAY; ll++)
    {
        for(int hs = 0; hs < int(data[ll].size()); hs++)
        {
            if(data[ll][hs]) cout << "x";
            else cout << "-";
        }
        cout << endl;
    }
}
