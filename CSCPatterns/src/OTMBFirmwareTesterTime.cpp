/*
 * OTMBFirmwareTesterTime.cpp
 *
 *  Created on: September 20, 2019
 *      Author: Siddharth Hariprakash
 */

#include <stdio.h>
#include <time.h>

using namespace std;

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/OTMBFirmwareTesterTime.h"

int main(int argc, char* argv[]){
	OTMBFirmwareTesterTime p;
	return p.main(argc,argv);
}


int OTMBFirmwareTesterTime::run(string inputfile, string outputfile, int start, int end) 
{   
    //auto t1 = std::chrono::high_resolution_clock::now();

	//are we running over real data or fake stuff for testing?
	const bool fakedata = true;

    if(!fakedata)
    cout << endl << "Running over file: " << inputfile << endl << endl;
	else
	cout << endl << "Not running over file: " << inputfile << endl << endl;	

    TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

	TFile* f_emu = TFile::Open("dat/Trees/EmulationResults.root");

	TTree* t_emu = (TTree*)f_emu->Get("EmulationResults");

	//
	// SET INPUT BRANCHES
	//

	CSCInfo::Event evt(t);
	CSCInfo::Muons muons(t);
	CSCInfo::Segments segments(t);
	CSCInfo::RecHits recHits(t);
	CSCInfo::LCTs lcts(t);
	CSCInfo::CLCTs clcts(t);
	CSCInfo::Comparators comparators_t(t);

	//initialize output files
	std::ofstream CFEBFiles[MAX_CFEBS];
	for(unsigned int i=0; i < MAX_CFEBS; i++)
	{
		//CFEBFiles[i].open("CFEB"+to_string(i)+"-fake.mem"); //TODO smart naming
		CFEBFiles[i].open("cfeb"+to_string(i)+".mem");
	}

	const unsigned int nCLCTs = 2;
	ofstream CLCTFiles[nCLCTs]; //2 clcts
	CLCTFiles[0].open("expected_1st.mem");
	CLCTFiles[1].open("expected_2nd.mem");

	vector<CSCPattern>* newPatterns = createNewPatterns();

	
	//if((end > t->GetEntries() && !fakedata) || end < 0) end = t->GetEntries();
	
	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	//
    //EVENT LOOP
    //	

	srand (time(NULL));

	for(int i = start; i < end; i++)
    {
        if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);		
		t_emu->GetEntry(i);	

        /*First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 *installed on September 12
		 */
		 
		//if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct

		//Era after min-separation change (10 -> 5), also includes 3 layer firmware change		

        for(unsigned int chamberHash = 0; chamberHash < (unsigned int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
        {	
			//cout << dec << "CHAMBERHASH: " << chamberHash << endl << endl;

            CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

			ChamberHits comphits(ST,RI,EC,CH);

			vector<CLCTCandidate*> emulatedclcts;	

			if(fakedata)
			{	
				Comparators_gen comparators_gen;

				unsigned int comparatorcount = (rand() % 192) + 25; //total comparators in the chamber
				if(comparatorcount >= 192)
					comparatorcount = 191;

				for(unsigned int icomp = 0; icomp < comparatorcount; icomp++)
				{
					int time_comp = rand() % 16; //comparator time bin 0-15;				
					int hs = rand() % 2; //0 or 1
					int maxstrip = ((comphits.maxHs()-hs)/2);
					int strip = rand() % maxstrip + 1;
					int lay = rand() % NLAYERS + 1; //comparator layer

					comparators_gen.ch_id.push_back((int)chamberHash);
					comparators_gen.lay.push_back(lay);
					comparators_gen.strip.push_back(strip);
					comparators_gen.halfStrip.push_back(hs);
					comparators_gen.bestTime.push_back(time_comp);
				}

				if(comparators_gen.size() == 0)
				continue;

				if(searchForMatch_pretrigger(comphits,newPatterns,emulatedclcts,comparators_gen,true))
				{
					emulatedclcts.clear();
					continue;
				}

				while(emulatedclcts.size() > 2)
				{
					emulatedclcts.pop_back();
				}				

				writeToMEMFiles_v1(comphits,comparators_gen,CFEBFiles);

				//cout << "khs, patt, ccode" << endl << endl;
				cout << internal << setfill('0');
				
				for(unsigned int itime = 1; itime <= end_time_bin+6; itime++)
				{	
					ChamberHits compHits_1(ST,RI,EC,CH);
					if(compHits_1.fill_time(comparators_gen,itime))
						return -1;
					if(compHits_1.clearcomparators())
						return -1;

					compHits_1.print();
					cout << endl;					

					vector <CLCTCandidate*> clctsintimebin;

					for(unsigned int iclct = 0; iclct < emulatedclcts.size(); iclct++)
					{
						if(emulatedclcts.at(iclct)->triggertimebin == itime)
							clctsintimebin.push_back(emulatedclcts.at(iclct));						
					}

					for(unsigned int iclct = 0; iclct < nCLCTs; iclct++)
					{
						if(iclct < clctsintimebin.size())
						{
							auto& clct = clctsintimebin.at(iclct);					
							std::cout << dec << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId()/10 << " cc: " << clct->comparatorCodeId() << std::endl;
							std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
							std::cout << setw(4) << clct->patternId()/10 << endl;
							std::cout << setw(4) << clct->comparatorCodeId() << endl;
							CLCTFiles[iclct] << internal << setfill('0')  << hex << setw(4) << clct->keyHalfStrip() << setw(4) << clct->patternId()/10 << setw(4) << clct->comparatorCodeId() << endl;
							
						}
						else
						{
							CLCTFiles[iclct] << "000000000000" << endl;
						}
						
					}									
				}

			}
		}
	}
    
	for(auto& cfebFile : CFEBFiles) cfebFile.close();
	for(auto& clctFile : CLCTFiles) clctFile.close();

	return 0;
}