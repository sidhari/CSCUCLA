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

    CSCInfo::Wires wires(t);
	CSCInfo::ALCTs alcts(t);
	
	for (int i = 0; i<eventnum; i++)
	{
		t->GetEntry(i);

		bool head_bool = false; 

		std::vector<ALCT_ChamberHits*> cvec;
		for (int i=0; i<16; i++)
		{
			ALCT_ChamberHits * temp = new ALCT_ChamberHits(ST,RI,CH,EC);
			temp->fill(wires,i);
			cout << *temp << endl; 
			cvec.push_back(temp);
		}
		std::vector<ALCTCandidate*> candvec;
		ALCTCandidate * head;

		cout << "=== PreTriggerring Results ===" << endl << endl; 

		for (int i = 0; i<(cvec.at(0))->get_maxWi(); i++)
		{
			ALCTCandidate * cand;
			cand = (i==0) ? new ALCTCandidate(i,1) : new ALCTCandidate(i,1,cand);
			if (preTrigger(0,cvec,config,*cand)) candvec.push_back(cand); 
		}

		for (int i=0; i< candvec.size(); i++)
		{
			std::cout << candvec.at(i) << endl; 
		}

		std::vector<ALCTCandidate*> candvec2;

		cout << "=== PatternDetection Results ===" << endl << endl; 

		for (int i = 0; i<candvec.size(); i++)
		{
			ALCTCandidate* cand = candvec.at(i);
			if (patternDection(cvec,config,*cand)) candvec2.push_back(cand); 
		}
		
		for (int i=0; i< candvec2.size(); i++)
		{
			std::cout << candvec2.at(i) << endl; 
		}

		cout << "=== GhostBuster Results ===" << endl << endl; 

		if (candvec2.size()!= 0) ghostBuster(candvec2.at(0));

		cout << "got here" << endl; 
		for (int i=0; i<candvec2.size();i++)
		{
			if (candvec2.at(i)->isValid()) std::cout<< candvec2.at(i) << endl;
		}

		cout << " finished emulation results" << endl << endl;

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
			cout<< "key wire group = " << (int)(alcts.keyWG->at(i)) 
				<< ", quality = " << " " << (int)(alcts.quality->at(i)) 
				<< ", accelerator = " << " " << (int) alcts.accelerator->at(i)
				<< ", BX = " << " " << (int) alcts.BX->at(i) << endl; 
		}
		if (candvec2.size()) return 0; 
	}
    return 0;
}