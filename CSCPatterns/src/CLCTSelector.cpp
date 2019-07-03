/*
 * CLCTSelector.cpp
 *
 *  Created on: June 27, 2019
 *      Author: Siddharth Hariprakash
 */

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include<TCanvas.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>
#include<map>


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;

std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> CFEBSplitter(vector<CLCTCandidate*> emuCLCTs)
{
	
	map<int,vector<CLCTCandidate*>> DetCFEB;
	
	for(unsigned int j = 0; j < emuCLCTs.size(); j++)
	{
		float keystrip = emuCLCTs.at(j)->keyStrip();
		int CFEB = ((keystrip-1)/16);
		DetCFEB[CFEB].push_back(emuCLCTs.at(j));
	}	

	std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> CFEBSplit;
	std::get<0>(CFEBSplit) = DetCFEB[0];
	std::get<1>(CFEBSplit) = DetCFEB[1];
	std::get<2>(CFEBSplit) = DetCFEB[2];
	std::get<3>(CFEBSplit) = DetCFEB[3];
	std::get<4>(CFEBSplit) = DetCFEB[4];

	return CFEBSplit;

}


int CLCTSelector(string inputfile, string outputfile, int start = 0, int end = -1) 
{

	cout << endl << "Running over file: " << inputfile << endl << endl;


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
	CSCInfo::Comparators comparators(t);

    EmulatedCLCTs emulatedclcts(t_emu,2);	

	vector<CSCPattern>* newEnvelopes = createNewPatterns();	

	LUT lut("lut");
	lut.loadROOT("dat/luts/lut.root");


	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();


	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	unsigned long long int SegCount = 0;
	unsigned int totalmatches = 0;
	unsigned int FirstCLCTMatch = 0;
	unsigned int SecondCLCTMatch = 0;
	unsigned int OtherIndexCLCTMatch = 0;

	TH1F* FirstCLCTMatchesvsPt = new TH1F ("First CLCT matches", "First CLCT matches", 50, 0, 100);
	TH1F* SecondCLCTMatchesvsPt = new TH1F ("Second CLCT matches", "Second CLCT matches", 50, 0, 100);
	TH1F* OtherCLCTMatchesvsPt = new TH1F ("Other index CLCT matches", "Other Index CLCT matches", 50, 0, 100);

	for(int i = start; i < end; i++) 
    {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
        t_emu->GetEntry(i);

        /* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;

        for(unsigned int chamberHash = 0; chamberHash < (unsigned int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
        {		

			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

            bool me11a = (ST==1 && RI==4);
            bool me11b = (ST==1 && RI == 1);
            bool me13 = (ST==1 && RI==3);	

            ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> newSetMatch;			
            
            unsigned int PID;    			        

            for(unsigned int i = 0; i < emulatedclcts.size(); i++)
            {
                if(chamberHash != emulatedclcts.ch_id->at(i))
                continue;                       
                
               PID = emulatedclcts.patternId->at(i);

               if(PID == 100)
               {				   	
                   	CSCPattern p = newEnvelopes->at(0);      
				  	CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
              	  	CLCTCandidate *c = &clct;
              		newSetMatch.push_back(c);
                             
               }
               if(PID == 90)
               {	
                   CSCPattern p = newEnvelopes->at(1);      
				   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
              	   CLCTCandidate *c = &clct;
              	   newSetMatch.push_back(c);                   
               }
               if(PID == 80)
               {	
                   CSCPattern p = newEnvelopes->at(2);      
				   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
              	   CLCTCandidate *c = &clct;
              	   newSetMatch.push_back(c);                   
               }
               if(PID == 70)
               {	
                   CSCPattern p = newEnvelopes->at(3);      
				   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
              	   CLCTCandidate *c = &clct;
              	   newSetMatch.push_back(c);                  
               }
               if(PID == 60)
               {	
                   CSCPattern p = newEnvelopes->at(4);      
				   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
              	   CLCTCandidate *c = &clct;
              	   newSetMatch.push_back(c);                   
               }

			   
            }

			//split CLCTs by CFEB
           

			std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> newSetMatchbyCFEB = CFEBSplitter(newSetMatch);
			vector<CLCTCandidate*> CFEB1 = std::get<0>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB2 = std::get<1>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB3 = std::get<2>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB4 = std::get<3>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB5 = std::get<4>(newSetMatchbyCFEB);

			/*compHits.print();

			for(int i = 0; i < 5; i++)
			{
				cout << endl << "CFEB " << i+1 << endl;

				vector<CLCTCandidate*> CFEB = std::get<i>(newSetMatchbyCFEB);

				for(int j = 0; j < CFEB.size(); j++)
				{
					cout << "CLCT#" << j+1 << ": " << "KeyHalfStrip: " << CFEB.at(j)->keyHalfStrip() << "Pattern ID: " << CFEB.at(j)->patternId() << " Comparator Code ID: " << CFEB.at(j)->comparatorCodeId() <<" Layer Count: " << CFEB.at(j)->layerCount() << endl;
				}

			}

			cout << endl;*/

			//sort clcts within each CFEB using cfebquality	

			sort(CFEB1.begin(),CFEB1.end(),CLCTCandidate::cfebquality);
			sort(CFEB2.begin(),CFEB2.end(),CLCTCandidate::cfebquality);
			sort(CFEB3.begin(),CFEB3.end(),CLCTCandidate::cfebquality);
			sort(CFEB4.begin(),CFEB4.end(),CLCTCandidate::cfebquality);
			sort(CFEB5.begin(),CFEB5.end(),CLCTCandidate::cfebquality);

			std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> newSetMatchbyCFEB_sorted;
			std::get<0>(newSetMatchbyCFEB_sorted) = CFEB1;
			std::get<1>(newSetMatchbyCFEB_sorted) = CFEB2;
			std::get<2>(newSetMatchbyCFEB_sorted) = CFEB3;
			std::get<3>(newSetMatchbyCFEB_sorted) = CFEB4;
			std::get<4>(newSetMatchbyCFEB_sorted) = CFEB5;
			
			/*compHits.print();

			for(int i = 0; i < 5; i++)
			{
				cout << endl << "CFEB " << i+1 << endl;

				vector<CLCTCandidate*> CFEB = std::get<i>(newSetMatchbyCFEB_sorted);

				for(unsigned int j = 0; j < CFEB.size(); j++)
				{
					cout << "CLCT#" << j+1 << ": " << "KeyHalfStrip: " << CFEB.at(j)->keyHalfStrip() << "Pattern ID: " << CFEB.at(j)->patternId() << " Comparator Code ID: " << CFEB.at(j)->comparatorCodeId() <<" Layer Count: " << CFEB.at(j)->layerCount() << endl;
				}

			}

			cout << endl;	*/				

			//take first from each cfeb, find best, add to final list, delete from this list, keep going till all cfeb vectors are empty

			vector<CLCTCandidate*> FinalCandidates;

			int size = CFEB1.size() + CFEB2.size() + CFEB3.size() + CFEB4.size() + CFEB5.size();

			for(int i = 0; i < size; i++)
			{
				if(CFEB1.size() != 0)
				FinalCandidates.push_back(CFEB1.at(0));

				if(CFEB2.size() != 0)
				FinalCandidates.push_back(CFEB2.at(0));

				if(CFEB3.size() != 0)
				FinalCandidates.push_back(CFEB3.at(0));

				if(CFEB4.size() != 0)
				FinalCandidates.push_back(CFEB4.at(0));

				if(CFEB5.size() != 0)
				FinalCandidates.push_back(CFEB5.at(0));

				sort(FinalCandidates.begin()+i, FinalCandidates.end(), CLCTCandidate::LUTquality);

				if(CFEB1.size() != 0)
				CFEB1.erase(CFEB1.begin());

				if(CFEB2.size() != 0)
				CFEB2.erase(CFEB2.begin());

				if(CFEB3.size() != 0)
				CFEB3.erase(CFEB3.begin());

				if(CFEB4.size() != 0)
				CFEB4.erase(CFEB4.begin());

				if(CFEB5.size() != 0)
				CFEB5.erase(CFEB5.begin());		

			}	
			

			vector<int> matchedCLCTs;

			for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++)
			{
				
				if(chamberHash != (unsigned int)segments.ch_id->at(thisSeg))
				continue;

				if(segments.mu_id->at(thisSeg) == -1)
				continue;	

				SegCount++;			

				float segmentX = segments.pos_x->at(thisSeg);
				float pt = muons.pt->at(segments.mu_id->at(thisSeg));

				if(me11a)
				{
					if(segmentX > 47) continue;
				}
				else if (me11b || me13) 
				{
					if(segmentX > 63) continue;
				} 
				else 
				{
					if(segmentX > 79) continue;
				}
				
				int closestCLCTtoSegmentIndex = -1;
				float minDistanceSegmentToClosestCLCT = 1e5;

				for(unsigned int iclct = 0; iclct < FinalCandidates.size(); iclct++)
				{	 
					if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), iclct) != matchedCLCTs.end())
					continue;

					float CLCTStripPos = FinalCandidates.at(iclct)->keyStrip();

					if(abs(CLCTStripPos - segmentX) < minDistanceSegmentToClosestCLCT)
					{
						minDistanceSegmentToClosestCLCT = abs(CLCTStripPos - segmentX);
						closestCLCTtoSegmentIndex = iclct;
					}

				}

				if(closestCLCTtoSegmentIndex != -1) //match
				{							
					matchedCLCTs.push_back(closestCLCTtoSegmentIndex);
					totalmatches++;
					if(closestCLCTtoSegmentIndex == 0)
					{
						FirstCLCTMatch++;
						FirstCLCTMatchesvsPt->Fill(pt);
					}					
					else if(closestCLCTtoSegmentIndex == 1)
					{
						SecondCLCTMatch++;
						SecondCLCTMatchesvsPt->Fill(pt);
					}
					else
					{
						OtherIndexCLCTMatch++;
						OtherCLCTMatchesvsPt->Fill(pt);
					}

				}		
								

			}						

        }

	}

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

	outF->cd();
	FirstCLCTMatchesvsPt->Write();
	SecondCLCTMatchesvsPt->Write();
	OtherCLCTMatchesvsPt->Write();

	cout << SegCount << " " << FirstCLCTMatch << " " << SecondCLCTMatch << " " << OtherIndexCLCTMatch << endl << endl;


	cout << "Wrote to file: " << outputfile << endl;

	return 0;
}

int main(int argc, char* argv[])
{
	try 
    {
		switch(argc)
        {
		case 3:
			return CLCTSelector(string(argv[1]), string(argv[2]));
		case 4:
			return CLCTSelector(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return CLCTSelector(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./CLCTSelector inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) 
    {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}

