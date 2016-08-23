#include <vector>

#define NLAY 6

using namespace std;

class hsData
{
    private:
        vector<vector<bool>> data;
        int count;
        int ct2NHS[10] = {160,160,160,160,160,160,160,160,160,160};

    public:
        hsData(vector<int> patData,vector<int> patLayer,int CT);
        ~hsData();

        vector<bool> getLayer(int lay) {return data[lay];};
        bool getBit(int lay, int hs) {return data[lay][hs];};
        vector<bool> getHS(int hs);
        int getCount() {return count;};
        void print();
};
