#include "../include/patFilter.h"

#include <fstream>

using namespace std;

patFilter::patFilter(string filename)
{
    ifstream inF(filename.c_str());
}

patFilter::~patFilter()
{
}
