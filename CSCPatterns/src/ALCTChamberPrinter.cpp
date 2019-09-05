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

	ALCTConfig config;
	//config.set_narrow_mask_flag(true);
 
    CSCInfo::Wires wires(t);
	CSCInfo::ALCTs alcts(t);
	CSCInfo::LCTs lcts(t); 
	CSCInfo::Segments segments(t);
	
	for (int e_num = 0; e_num<eventnum; e_num++)
	{
		t->GetEntry(e_num);

		std::vector<ALCT_ChamberHits*> cvec;
		for (int i=0; i<16; i++)
		{
			ALCT_ChamberHits * temp = new ALCT_ChamberHits(ST,RI,CH,EC);
			temp->fill(wires,i);
			cout << *temp << endl; 
			cvec.push_back(temp);
		}
		
		std::vector<std::vector<ALCTCandidate*>> end_vec; 

		std::vector<ALCTCandidate*> out_vec;

		for (int i=0; i<config.get_fifo_tbins()-config.get_drift_delay(); i++)
		{
			std::vector <ALCTCandidate*> temp_vec; 
			for (int j = 0; j<cvec.at(0)->get_maxWi(); j++)
			{
				ALCTCandidate * cand = new ALCTCandidate(j,1);
				temp_vec.push_back(cand);
			}
			end_vec.push_back(temp_vec); 
		}

		trig_and_find(cvec, config, end_vec);
		cout << "got past trig_and_find" << endl << endl; 
		ghostBuster(end_vec,config);
		extract_sort_cut(end_vec,out_vec);
		//extract(end_vec,out_vec);

		for (int i = 0; i<out_vec.size(); i++)
		{
			//int myBX = (out_vec.at(i))->get_first_bx(); 
			//if (myBX>=5 && myBX<=11)
				std::cout << out_vec.at(i) << endl << endl;
		}

		for (int i=0; i<alcts.size(); i++)
		{
			int chSid1 = CSCHelper::serialize(ST, RI, CH, EC);
			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
			int chSid2 = chSid1;
			
			bool me11a	= ST == 1 && RI == 4;
			bool me11b	= ST == 1 && RI == 1;

			if (me11a || me11b)
			{
				if (me11a) chSid2 = CSCHelper::serialize(ST, 1, CH, EC);
				if (me11b) chSid2 = CSCHelper::serialize(ST, 4, CH, EC);
			}
			if (chSid1!=alcts.ch_id->at(i) && chSid2!= alcts.ch_id->at(i)) continue;
			CSCHelper::ChamberId c = CSCHelper::unserialize(alcts.ch_id->at(i));
			cout<< "key wire group = " << (int)(alcts.keyWG->at(i))
				<< ", quality = " << (int)(alcts.quality->at(i)) 
				<< ", accelerator = " << (int) alcts.accelerator->at(i)
				<< ", BX = " <<  (int) alcts.BX->at(i)
				<< ", track number = " << (int) alcts.trkNumber->at(i)
				<< ", Station = " << (int) c.station
				<< ", Ring = " << (int) c.ring
				<< endl;
		}

		for (int i=0; i<lcts.size(); i++)
		{
			int chSid1 = CSCHelper::serialize(ST, RI, CH, EC);
			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
			int chSid2 = chSid1;
			
			bool me11a	= ST == 1 && RI == 4;
			bool me11b	= ST == 1 && RI == 1;

			if (me11a || me11b)
			{
				if (me11a) chSid2 = CSCHelper::serialize(ST, 1, CH, EC);
				if (me11b) chSid2 = CSCHelper::serialize(ST, 4, CH, EC);
			}
			if (chSid1!=lcts.ch_id->at(i) && chSid2!= lcts.ch_id->at(i)) continue;
			CSCHelper::ChamberId c = CSCHelper::unserialize(lcts.ch_id->at(i));
			cout<< "key wire group = " << (int)(lcts.keyWireGroup->at(i)) 
				<< ", key half strip = " <<(int)(lcts.keyHalfStrip->at(i))
				<< endl;
		}
		int num_seg = 0; 
		for (int i=0; i<segments.size(); i++)
		{
			int chSid1 = CSCHelper::serialize(ST, RI, CH, EC);
			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
			int chSid2 = chSid1;
			
			bool me11a	= ST == 1 && RI == 4;
			bool me11b	= ST == 1 && RI == 1;

			if (me11a || me11b)
			{
				if (me11a) chSid2 = CSCHelper::serialize(ST, 1, CH, EC);
				if (me11b) chSid2 = CSCHelper::serialize(ST, 4, CH, EC);
			}
			if (chSid1!=segments.ch_id->at(i) && chSid2!= segments.ch_id->at(i)) continue;
			num_seg++;
		}

		cout << "Segment: " << num_seg << endl <<endl;

		cout << "finished alct for event " << e_num << endl << endl;  

		wipe(out_vec);
		wipe(cvec); 

		//if (candvec.size()) return 0; 
	}
    return 0;
}