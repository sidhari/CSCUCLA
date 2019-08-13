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


#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/CLCTSelctor.h"

using namespace std;

int main(int argc, char* argv[])
{
	CLCTSelector p;
	return p.main(argc,argv);
}


int CLCTSelector::run(string inputfile, string outputfile, int start, int end) 
{

	cout << endl << "Running over file: " << inputfile << endl << endl;


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

    TFile* f_emu = TFile::Open("dat/Trees/EmulationResults.root");

    TTree* t_emu = (TTree*)f_emu->Get("EmulationResults");

	//load LUT

	LUT lut(string("lut"));
	lut.loadText(string("dat/luts/lpxke.root")); 
	if(lut.makeFinal())
	{
		return -1;
	}


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

	//
	//COUNTERS
	//
	
	unsigned long long int totalmuonsegments = 0;
	unsigned long long int totalmatches = 0;
	unsigned long long int firstclctmatches = 0;
	unsigned long long int secondclctmatches = 0;
	unsigned long long int otherindexclctmatches = 0;

	//
	//HISTOGRAMS
	//

	TH1F* matchespt = new TH1F ("matched_muon_segments_pt", "Matched Muon Segments: pt", 50, 0, 100);
	TH1F* firstclctmatchespt = new TH1F ("muon_segments_matched_to_first_CLCT_pt", "Muon Segments matched to first CLCT: pt", 50, 0, 100);
	TH1F* secondclctmatchespt = new TH1F ("muon_segments_matched_to_second_CLCT_pt", "Muon Segments matched to second CLCT: pt", 50, 0, 100);
	TH1F* otherindexclctmatchespt = new TH1F ("muon_segments_matched_to_other_CLCTs_pt", "Muon Segments matched to other CLCTs: pt", 50, 0, 100);
	TH1F* effeciency1 = new TH1F ("first_index_efficiency", "First Index Efficiency;Pt[GeV];Efficiency", 50, 0, 100);
	TH1F* effeciency2 = new TH1F ("second_index_efficiency", "Second Index Efficiency;Pt[GeV];Efficiency", 50, 0, 100);
	TH1F* effeciency3 = new TH1F ("other_indices_efficiency", "Other Indices Efficiency;Pt[GeV];Efficiency", 50, 0, 100);

	TH1F* matchedclctindex = new TH1F ("matched_clct_index", "Matched CLCT Index", 10, 0, 10);

	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();


	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	for(int i = start; i < end; i++) 
    {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
        t_emu->GetEntry(i);

        /* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;

		//iterate through all chambers

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

			vector<CLCTCandidate*> initialclctcandidates;

			//iterate through all emulated CLCTs in chamber, fill vector 

			for(unsigned int iclct = 0; iclct < (unsigned int)emulatedclcts.size(); iclct++)
			{	
				if(emulatedclcts.ch_id->at(iclct) != chamberHash) 
				continue;

				int PID = emulatedclcts.patternId->at(iclct);				
				ComparatorCode c((unsigned int)emulatedclcts.comparatorCodeId->at(iclct));

				switch (PID)
				{	
					case 100:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(0),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;
					}

					case 90:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(1),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;	
					}

					case 80:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(2),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);	
						break;
					}

					case 70:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(3),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);	
						break;
					}

					case 60:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(4),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;
					}

					default:
					{
						cout << "Invalid PID" << endl << endl;
						break;
					} 

				}

			}

			

			for(unsigned int temp = 0; temp < initialclctcandidates.size(); temp++)
			{
				const LUTEntry *e;
				int PID = initialclctcandidates.at(temp)->patternId();
				int CCID = initialclctcandidates.at(temp)->comparatorCodeId();
				LUTKey k(PID,CCID);
				lut.getEntry(k,e);
				initialclctcandidates.at(temp)->_lutEntry = e;
			}

			//print CLCTs pre sort here (TO DO)

			//split CLCTs by CFEB

			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit; 
			vector<int> CFEBpossibilities; //keeps track of which CFEBs have CLCTs

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{
				float keystrip = initialclctcandidates.at(iclct)->keyStrip();
				int whichCFEB = (keystrip-1)/16;
				CFEBsplit[whichCFEB].push_back(initialclctcandidates.at(iclct));
				if(std::find(CFEBpossibilities.begin(),CFEBpossibilities.end(),whichCFEB) == CFEBpossibilities.end())
				CFEBpossibilities.push_back(whichCFEB);
			}	

			//sort within each CFEB		

			for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
			{ 
				sort(CFEBsplit[*it].begin(),CFEBsplit[*it].end(),CLCTCandidate::cfebquality);
			}			

			//pick first from each CFEB, use LUT to find best

			vector<CLCTCandidate*> finalclctcandidates;

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit[*it].size() == 0)
					continue;
					finalclctcandidates.push_back(CFEBsplit[*it].at(0));
				}

				sort(finalclctcandidates.begin()+iclct,finalclctcandidates.end(),CLCTCandidate::LUTquality);

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit[*it].size() == 0)
					continue;
					CFEBsplit[*it].erase(CFEBsplit[*it].begin());
				}
			}	

			unsigned int segcount  = 0;

			for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
			{
				if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
				continue;

				if(segments.mu_id->at(iseg) == -1)
				continue;

				segcount++;

			}

			/*if(segcount != 1 || finalclctcandidates.size() != 2)
			continue;*/

			//print CLCTs post sort here (TO DO)	

			vector<int> matchedclctindices;

			//iterate through all segments in chamber

			for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
			{
				if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
				continue;

				if(segments.mu_id->at(iseg) == -1)
				continue;				

				float segmentx = segments.pos_x->at(iseg);
				float pt = muons.pt->at(segments.mu_id->at(iseg));

				if(me11a)
				{
					if(segmentx > 47) continue;
				}
				else if (me11b || me13) 
				{
					if(segmentx > 63) continue;
				} 
				else 
				{
					if(segmentx > 79) continue;
				}

				totalmuonsegments++;

				int closestclcttosegmentindex = -1;
				float closestclcttosegmentdistance = 1e5;

				//iterate through all CLCTs in chamber, find best match

				for(int iclct = 0; iclct < (int)finalclctcandidates.size(); iclct++)
				{
					if(std::find(matchedclctindices.begin(),matchedclctindices.end(),iclct) != matchedclctindices.end())
					continue;

					if(finalclctcandidates.at(iclct)->layerCount() < 3)
					continue;

					float clctx = finalclctcandidates.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance)
					{
						closestclcttosegmentdistance = abs(clctx - segmentx);
						closestclcttosegmentindex = iclct;
					}
				}

				if(closestclcttosegmentindex != -1) //found a match
				{
					matchedclctindices.push_back(closestclcttosegmentindex);
					totalmatches++;
					matchespt->Fill(pt);
					matchedclctindex->Fill(closestclcttosegmentindex);
					if(closestclcttosegmentindex == 0)
					{
						firstclctmatches++;
						firstclctmatchespt->Fill(pt);
					}
					if(closestclcttosegmentindex == 1)
					{
						secondclctmatches++;
						secondclctmatchespt->Fill(pt);
					}
					if(closestclcttosegmentindex > 1)
					{
						otherindexclctmatches++;
						otherindexclctmatchespt->Fill(pt);
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

	effeciency1->Divide(firstclctmatchespt,matchespt);
	effeciency2->Divide(secondclctmatchespt,matchespt);
	effeciency3->Divide(otherindexclctmatchespt,matchespt);

	outF->cd();
	matchespt->Write();
	firstclctmatchespt->Write();
	secondclctmatchespt->Write();
	otherindexclctmatchespt->Write();
	effeciency1->Write();
	effeciency2->Write();
	effeciency3->Write();
	matchedclctindex->Write();

	cout << totalmatches << " " << firstclctmatches << " " << secondclctmatches << " " << otherindexclctmatches << endl << endl;


	cout << "Wrote to file: " << outputfile << endl;

	return 0;
}

