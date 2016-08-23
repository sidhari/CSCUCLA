#include <string>
#include "hsData.h"

using namespace std;

class patFilter
{
    private:
        bool pats[10][15][6];
        bool emu;
        int emuHS;
        int emuPatID;

    public:
        patFilter(string filename);
        ~patFilter();

        hsData filter(hsData data, int pat);
        void emulate(hsData data);
        int getEmuHS();
        int getEmuPatID();

        void print();
}
