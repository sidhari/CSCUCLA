#include <string>
#include <vector>
//#include "hsData.h"

using namespace std;

class patFilter
{
    private:
        bool pats[11][6][11];
        bool emu;
        int emuKHS0;
        int emuPatID0;
        int emuKHS1;
        int emuPatID1;
        int Nlay0;
        int T0;
        int Nlay1;
        int T1;
        int clct_persist;

    public:
        patFilter(string filename);
        ~patFilter();

        vector<vector<bool>> filter(hsData data, int pat, int khs);
        void emulate(hsData data, bool debug);
        int getEmuKHS(int n){if(n==0) return emuKHS0; else return emuKHS1;};
        int getEmuPatID(int n){if(n==0) return emuPatID0; else return emuPatID1;};
        int getEmuNlay(int n){if(n==0) return Nlay0; else return Nlay1;};
        int getEmuTime(int n){if(n==0) return T0; else return T1;};

        void print();
};
