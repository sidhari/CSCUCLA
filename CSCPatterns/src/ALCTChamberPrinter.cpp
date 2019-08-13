/*
 * ALCTChamberPrinter.cpp
 *
 *  Created on: 06 August 2019
 *      Author: Chau Dao
 */

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include<map>

#include <TTree.h>
#include <TFile.h>

#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"
#include "../include/ALCTHelperFunctions.h"

#include "../include/ALCTChamberPrinter.h"

using namespace std;

int main(int argc, char* argv[]){
	ALCTChamberPrinter p;
	return p.main(argc,argv);
}

int ALCTChamberPrinter::run(string inputfile, unsigned int ST, unsigned int RI, unsigned int CH, unsigned int EC, unsigned int eventnum)
{
	cout << endl << "Running over file: " << inputfile << endl;

	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

    CSCInfo::Wires wires(t);
    ALCT_ChamberHits wireHits(ST, RI, CH, EC);
	t->GetEntry(eventnum);

	for (unsigned int i = 0; i<wires.size(); i++)
	{
		print_pulse(wires.timeBinWord->at(i));
		print_pulse(extend_time(wires.timeBinWord->at(i),6));
	}

	// wireHits.fill(wires);
    // std::cout << wireHits << endl;
    return 0;
}