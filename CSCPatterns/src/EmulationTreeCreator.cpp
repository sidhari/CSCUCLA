/*
 * EmulationTreeCreator.cpp
 *
 *  Created on: June 1, 2019
 *      Author: Siddharth Hariprakash
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
#include <TH1F.h>
#include <TH2F.h>

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;

int EmulationTreeCreator(string inputfile, string outputfile, int start=0, int end=-1) 
{

	auto t1 = std::chrono::high_resolution_clock::now();

	cout << endl << "Running over file: " << inputfile << endl;

	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

	TFile* outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	TTree* t_emu = new TTree("EmulationResults", "EmulationResults");	  
	
	//
	// SET INPUT BRANCHES
	//

	CSCInfo::Event evt(t);
	CSCInfo::Muons muons(t);
	CSCInfo::Segments segments(t);
	CSCInfo::RecHits recHits(t);
	CSCInfo::LCTs lcts(t);
	CSCInfo::CLCTs clcts(t);
	CSCInfo::Comparators comparators(t);

    CLCTCandidateCollection OldPatternsEmulatedCLCTs(t_emu,1);	
	CLCTCandidateCollection NewPatternsEmulatedCLCTs(t_emu,2);
	
	vector<CSCPattern>* oldPatterns = createOldPatterns();
	vector<CSCPattern>* newPatterns = createNewPatterns();

	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	cout << endl;
	
	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	for(int i = start; i < end; i++) 
	{
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		/* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		//if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;

		OldPatternsEmulatedCLCTs.Erase();
		NewPatternsEmulatedCLCTs.Erase();

		for(unsigned int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
		{
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

			ChamberHits compHits(ST, RI, EC, CH);
			if(compHits.fill(comparators)) return -1;

			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);

			//bool threeLayerChamber = ((me11a || me11b) && CH == 11 && EC==1);

			//Old Patterns

			vector<CLCTCandidate*> OPemulatedCLCTs;

			if(searchForMatch(compHits, oldPatterns, OPemulatedCLCTs, true))
			{
				OPemulatedCLCTs.clear();
				//cout << "Something broke" << endl;
				//return;

				continue;
			}

			/*if(!threeLayerChamber) //4 Layer min
			{
				for(unsigned int iemu =0; iemu < OPemulatedCLCTs.size(); iemu++)
				{
					if(OPemulatedCLCTs.at(iemu)->layerCount() == 3) 
					{
						OPemulatedCLCTs.erase(OPemulatedCLCTs.begin()+iemu);
						iemu--;
					}
				}
			}*/			

			OldPatternsEmulatedCLCTs.Fill(OPemulatedCLCTs, chamberHash); 	

			//New Patterns

			vector<CLCTCandidate*> NPemulatedCLCTs;

			if(searchForMatch(compHits, newPatterns, NPemulatedCLCTs, true))
			{
				NPemulatedCLCTs.clear();
				//cout << "Something broke" << endl;
				//return;

				continue;
			}

			NewPatternsEmulatedCLCTs.Fill(NPemulatedCLCTs, chamberHash);


		}

		//OldPatternsEmulatedCLCTs.FillTree(1);
		//NewPatternsEmulatedCLCTs.FillTree(2);		
		t_emu->Fill();
	}

	outF->cd();
	t_emu->Write();
	//delete outF;

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;
	return 0;
}



int main(int argc, char* argv[])
{
	try 
	{
		switch(argc)
		{
		case 3:
			return EmulationTreeCreator(string(argv[1]), string(argv[2]));
		case 4:
			return EmulationTreeCreator(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return EmulationTreeCreator(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./EmulationTreeCreator inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) 
	{
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}