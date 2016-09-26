#include <vector>

#define NLAY 6

using namespace std;

class hsData
{
    private:
        vector<vector<vector<bool>>> data;
        int count;
        int ct;
        int ct2NHS[10] = {160,160,160,160,160,160,224,160,128,224};

    public:
        hsData(vector<int> patData,vector<int> patLayer, vector<vector<int>> timeOn,int CT);
        ~hsData();

        vector<bool> getLayer(int lay,int time) {return data[time][lay];};
        bool getBit(int lay, int hs, int time) {return bool(data[time][lay][hs]);};
        vector< vector<bool> > getTime(int time) {return data[time];};
        vector< vector<bool> > getTimeSum(int it, int ft);
        vector< vector<bool> > getSumMax(int persist);
        vector<bool> getHS(int hs, int time);
        int getCT(){return ct;};
        int getCount() {return count;};
        void print();
};
