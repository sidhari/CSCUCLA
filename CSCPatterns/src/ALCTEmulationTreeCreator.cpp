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
#include "../include/ALCTHelperFunctions.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/ALCTEmulationTreeCreator.h"

using namespace std; 

int main(int argc, char* argv[])
{
    ALCTEmulationTreeCreator p;
    return p.main(argc, argv);
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

    /*TFile* outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

    TTree* alct_t_emu = new TTree("ALCTEmulationResults", "ALCTEmulationResults");*/

    CSCInfo::ALCTs alcts(t);
    CSCInfo::Wires wires(t);

    /**********************
	 * EVENT LOOP
	 **********************/

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	cout << endl;
	
	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	ALCTConfig config; 

	for(int i = start; i < end; i++) 
	{
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		/**********************
	 	* CHAMBER LOOP
	 	**********************/

		for(unsigned int chamberHash = 0; chamberHash < (unsigned int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
		{
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if (ST == 1 && RI == 1) continue;
			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

			//cout << "Event = " << i << ", ST = " << ST << ", RI = " << RI << ", CH = " << CH << ", EC = " << EC << endl << endl;

			std::vector<ALCT_ChamberHits*> cvec;

			for (int i=0; i<16; i++)
			{
				ALCT_ChamberHits * temp = new ALCT_ChamberHits(ST,RI,CH,EC);
				if (i >= config.get_start_bx())
					temp->fill(wires,i,config.get_start_bx());
				cvec.push_back(temp);
			}

			//cout << "safe here1" << endl; 

			ALCTCandidate * head = new ALCTCandidate(0,1);

			for (int i = 0; i<(cvec.at(0))->get_maxWi(); i++)
			{
				ALCTCandidate * cand; 
				cand = (i==0) ? head : new ALCTCandidate(i,1,cand);
			}

			//cout << "safe here2" << endl; 

			std::vector<ALCTCandidate*> candvec; 
			preTrigger(cvec,config,head);
			patternDetection(cvec, config, head);
			ghostBuster(head);
			clean(head);
			head_to_vec(head,candvec);

			int num_alct=0; 
			for (int i=0; i<alcts.size(); i++)
			{
				int chSid1 = CSCHelper::serialize(ST, RI, CH, EC);
				int chSid2 = chSid1;
				
				bool me11a	= ST == 1 && RI == 4;
				bool me11b	= ST == 1 && RI == 1;

				if (me11a || me11b)
				{
					if (me11a) chSid2 = CSCHelper::serialize(ST, 1, CH, EC);
					if (me11b) chSid2 = CSCHelper::serialize(ST, 4, CH, EC);
				}
				if (chSid1!=alcts.ch_id->at(i) && chSid2!= alcts.ch_id->at(i)) continue;
				num_alct++;
			}
			//if (num_alct>candvec.size()) cout << "Event = " << i << ", ST = " << ST << ", RI = " << RI << ", CH = " << CH << ", EC = " << EC << ", num_alct = " << num_alct << ", size = " << candvec.size() << endl << endl;
			if (num_alct==0 && candvec.size()) cout << "Event = " << i << ", ST = " << ST << ", RI = " << RI << ", CH = " << CH << ", EC = " << EC << ", num_alct = " << num_alct << ", size = " << candvec.size() << endl << endl;
			wipe(candvec);
			wipe(cvec);
		}
		//alct_t_emu->Fill();
	}

	//outF->cd();
	//alct_t_emu->Write();

	//printf("Wrote to file: %s\n",outputfile.c_str());

	//auto t2 = std::chrono::high_resolution_clock::now();
	//cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;
	return 0;
}
