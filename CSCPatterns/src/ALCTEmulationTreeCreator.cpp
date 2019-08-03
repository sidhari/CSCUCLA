/*
 * ALCTEmulationTreeCreator.cpp
 *
 *  Created on: 29 July 2019
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
#include <map>

#include <TTree.h>
#include <TFile.h>

#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"


using namespace std; 

int main(int argc, char* argv[])
{
    ALCTEmulationTreeCreator p;
    return p,main(argc, argv);
}

int ALCTEmulationTreeCreator::run(string inputfile, string outputfile, int start, int end)
{
	/**********************
	 * TREE INITIALIZATION
	 **********************/

    auto t1 = std::chrono::high_resolution_clock::now();

    cout << endl << "Running over file: " << inputfile << endl;

    TFile* f = TFile::Open(inputfile.c_str());
    if (!f) throw "Can't open file";

    TTree* t = (TTree*) f->Get("CSCDigiTree");
    if(!t) throw "Can't find Tree";

    TFile* outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

    TTree* alct_t_emu = new TTree("ALCTEmulationResults", "ALCTEmulationResults");

    CSCInfo::ALCTs alcts(t);
    CSCInfo::Wires wires(t);

    /**********************
	 * EVENT LOOP
	 **********************/

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	cout << endl;
	
	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	for(int i = start; i < end; i++) 
	{
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		for(unsigned int chamberHash = 0; chamberHash < (unsigned int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
		{
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
		}
		alct_t_emu->Fill();
	}

	outF->cd();
	alct_t_emu->Write();

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;
	return 0;
}
