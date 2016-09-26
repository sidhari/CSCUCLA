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
        void emulate(hsData data);
        int getEmuKHS(){return emuKHS0;};
        int getEmuPatID(){return emuPatID0;};
        int getEmuNlay(){return Nlay0;};
        int getEmuTime(){return T0;};

        void print();
};
