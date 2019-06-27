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

std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> CFEBSplitter(int CFEBcount, vector<CLCTCandidate*> emuCLCTs)
{
	
	map<int,vector<CLCTCandidate*>> DetCFEB;
	
	for(int j = 0; j < emuCLCTs.size(); j++)
	{
		float keystrip = emuCLCTs.at(j)->keyStrip();
		int CFEB = (keystrip/16);
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


int CLCTSelector(string inputfile, string outputfile, int start, int end) 
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

    EmulatedCLCTs emulatedclcts(t_emu,2)



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

	LUT lut("lut", "dat/luts/linearFits.lut");
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

	TH1F* FirstCLCTMatchesvsPt = new TH1F ("First CLCT matches", "First CLCT matches", 20, 0, 100);
	TH1F* SecondCLCTMatchesvsPt = new TH1F ("Second CLCT matches", "Second CLCT matches", 20, 0, 100);
	TH1F* OtherCLCTMatchesvsPt = new TH1F ("Other index CLCT matches", "Other Index CLCT matches", 20, 0, 100);

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
			bool me12 = (ST==1 && RI==2);
			bool me21 = (ST==2 && RI==1);
			bool me22 = (ST==2 && RI==2);
			bool me31 = (ST==3 && RI==1);
			bool me32 = (ST==3 && RI==2);
			bool me41 = (ST==4 && RI==1);
			bool me42 = (ST==4 && RI==2);		


            ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> FinalCandidates;
            
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
           

			std::tuple<vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>, vector<CLCTCandidate*>> newSetMatchbyCFEB = CFEBSplitter(newSetMatch);
			vector<CLCTCandidate*> CFEB1 = std::get<0>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB2 = std::get<1>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB3 = std::get<2>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB4 = std::get<3>(newSetMatchbyCFEB);
			vector<CLCTCandidate*> CFEB5 = std::get<4>(newSetMatchbyCFEB);

			map<int,CLCTCandidate*> QualMap;

			//int size  = CFEB1.size() + CFEB2.size() + CFEB3.size() + CFEB4.size() + CFEB5.size();
			vector<int> QualVec;


			for(int i = 0; i < CFEB1.size(); i++)
			{
				int PID = CFEB1.at(i)->patternId();
				int CCID = CFEB1.at(i)->comparatorCodeId();
				auto key = make_pair(PID,CCID);
				LUTEntry* e = 0;
				lut.getEntry(key, e);
				QualVec.push_back(e->quality());
				QualMap[e->quality()] = CFEB1.at(i);
			}

			for(int i = 0; i < CFEB2.size(); i++)
			{
				int PID = CFEB2.at(i)->patternId();
				int CCID = CFEB2.at(i)->comparatorCodeId();
				auto key = make_pair(PID,CCID);
				LUTEntry* e = 0;
				lut.getEntry(key, e);
				QualVec.push_back(e->quality());
				QualMap[e->quality()] = CFEB2.at(i);
			}

			for(int i = 0; i < CFEB3.size(); i++)
			{
				int PID = CFEB3.at(i)->patternId();
				int CCID = CFEB3.at(i)->comparatorCodeId();
				auto key = make_pair(PID,CCID);
				LUTEntry* e = 0;
				lut.getEntry(key, e);
				QualVec.push_back(e->quality());
				QualMap[e->quality()] = CFEB3.at(i);
			}

			for(int i = 0; i < CFEB4.size(); i++)
			{
				int PID = CFEB4.at(i)->patternId();
				int CCID = CFEB4.at(i)->comparatorCodeId();
				auto key = make_pair(PID,CCID);
				LUTEntry* e = 0;
				lut.getEntry(key, e);
				QualVec.push_back(e->quality());
				QualMap[e->quality()] = CFEB4.at(i);
			}

			for(int i = 0; i < CFEB5.size(); i++)
			{
				int PID = CFEB5.at(i)->patternId();
				int CCID = CFEB5.at(i)->comparatorCodeId();
				auto key = make_pair(PID,CCID);
				LUTEntry* e = 0;
				lut.getEntry(key, e);
				QualVec.push_back(e->quality());
				QualMap[e->quality()] = CFEB5.at(i);
			}

			sort(QualVec.begin(), QualVec.end());		
			

			for(i = 0; i < QualVec.size(); i++)
			{	

				FinalCandidates.push_back(QualMap[QualVec.at(i)]);
				
			}
			

			vector<int> matchedCLCTs;

			for(int thisSeg = 0; thisSeg < segments.size(); thisSeg++)
			{
				
				if(chamberHash != segments.ch_id->at(thisSeg))
				continue;

				if(segments.mu_id->at(thisSeg) == -1)
				continue;	

				SegCount++;			

				float segmentX = segments.pos_x->at(iseg);
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

				for(int iclct = 0; iclct < FinalCandidates.size(); iclct++)
				{
					if(FinalCandidates.at(iclct)->layerCount() < 3)
					continue;

					if(std::find(matchedCLCTs.begin(), matchedCLCTs.end(), iclct) != matchedCLCTs.end())
					continue;

					float CLCTStripPos = FinalCandidates.at(iclct)->keyStrip();

					if(abs(CLCTStripPos - segmentX) < minDistanceSegmentToClosestCLCT)
					{
						minDistanceSegmentToClosestCLCT = abs(CLCTStripPos - segmentX);
						closestCLCTtoSegmentIndex = iclct;
					}

				}

				if(closestCLCTtoSegmentIndex != -1)
				{							
					matchedCLCTs.push_back(closestCLCTtoSegmentIndex);
					totalmatches++;
					if(closestCLCTtoSegmentIndex == 0)
					{
						FirstCLCTMatch++;
						FirstCLCTMatchesvsPt->Fill(pt)
					}					
					else if(closestCLCTtoSegmentIndex == 1)
					{
						SecondCLCTMatch++;
						SecondCLCTMatchesvsPt->Fill(pt);
					}
					else
					{
						OtherIndexCLCTMatch++;
						OtherCLCTmatchesvsPt->Fill(pt);
					}

				}		
								

			}						

        }

	}

	outF->Close();
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
