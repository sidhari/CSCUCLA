/*
 * ThreeLayerCLCTEmulationAnalyzer.cpp
 *
 *  Created on: May 1, 2019
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

#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/ThreeLayerCLCTEmulationAnalyzer.h"


using namespace std;

//looks at a map of histograms, if it contains the key, fills the corresponding histogram with "histValue"
void fillHist(map<unsigned int, TH1F*> hists, unsigned int key, float histValue)
{
	//look to see if we care about this chamber
	auto it = hists.find(key);
	if(it != hists.end())
	{
		//fill the correct histogram
		it->second->Fill(histValue);
	}
}

map<unsigned int, TH1F*> makeHistPermutation(string name, string title, unsigned int bins, unsigned int low, unsigned int high)
{
	TH1F* h_me11a_plus = new TH1F((name+"_me_p11a_11").c_str(),("me_p11a_11 " +title).c_str(),bins,low,high);
	TH1F* h_me11b_plus = new TH1F((name+"_me_p11b_11").c_str(),("me_p11b_11" +title).c_str(),bins,low,high);
	TH1F* h_me11a_minus = new TH1F((name+"_me_m11a_11").c_str(),("me_m11a_11 "+title).c_str(),bins,low,high);
	TH1F* h_me11b_minus = new TH1F((name+"_me_m11b_11").c_str(),("me_m11b_11" +title).c_str(),bins,low,high);

	unsigned int me_p11a11 = CSCHelper::serialize(1,4,11,1);
	unsigned int me_p11b11 = CSCHelper::serialize(1,1,11,1);
	unsigned int me_m11a11 = CSCHelper::serialize(1,4,11,2);
	unsigned int me_m11b11 = CSCHelper::serialize(1,1,11,2);

	map<unsigned int, TH1F*> hists;
	hists[me_p11a11] = h_me11a_plus;
	hists[me_p11b11] = h_me11b_plus;
	hists[me_m11a11] = h_me11a_minus;
	hists[me_m11b11] = h_me11b_minus;
	return hists;
}

int main(int argc, char* argv[]){
	ThreeLayerCLCTEmulationAnalyzer p;
	return p.main(argc,argv);
}



int ThreeLayerCLCTEmulationAnalyzer::run(string inputfile, string outputfile, int start, int end) {

	cout << "Running over file: " << inputfile << endl;


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

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



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

	//vector<CSCPattern>* newPatterns = createNewPatterns();
	vector<CSCPattern>* oldPatterns = createOldPatterns();

	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	map<unsigned int,TH1F*> allClctLayerCounts = makeHistPermutation("h_allClctLayerCount", "h_allClctLayerCount;Layer Count; CLCTs",7,0,7);
	map<unsigned int,TH1F*> clctLayerCounts = makeHistPermutation("h_clctLayerCount", "h_clctLayerCount;Layer Count; Matched CLCTs",7,0,7);
	map<unsigned int,TH1F*> unmatchedClctLayerCounts = makeHistPermutation("h_unmatchedClctLayerCount", "h_unmatchedClctLayerCount;Layer Count; Unmatched CLCTs",7,0,7); 
	
	TH1F* RealvsEmulated_Layers1 = new TH1F ("Emulated > Real (Layer Count)", "Emulated > Real (Layer Count); Layer Count; CLCTs",7,0,7);
	TH1F* RealvsEmulated_Layers2 = new TH1F ("Emulated < Real (Layer Count)", "Emulated < Real (Layer Count); Layer Count; CLCTs",7,0,7);

	TH2D* RealVsEmulated = new TH2D ("All Real Vs Emulated", "Real Vs Emulated; Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);
	TH2D* RealVsEmulated_ME11A = new TH2D ("All Real Vs Emulated (ME11A)", "Real Vs Emulated (ME11A); Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);
	TH2D* RealVsEmulated_ME11B = new TH2D ("All Real Vs Emulated (ME11B)", "Real Vs Emulated (ME11B); Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);
	TH2D* RealVsEmulated_EC2 = new TH2D ("All Real Vs Emulated (EC=2)", "Real Vs Emulated (EC=2); Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);
	TH2D* RealVsEmulated_ME11A_EC2 = new TH2D ("All Real Vs Emulated (ME11A, EC=2)", "Real Vs Emulated (ME11A, EC=2); Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);
	TH2D* RealVsEmulated_ME11B_EC2 = new TH2D ("All Real Vs Emulated (ME11B, EC=2)", "Real Vs Emulated (ME11B, EC=2); Emulated CLCTs; Real CLCTs", 3, 0, 3, 3, 0, 3);

	int CounterMoreThanOneNegativeKHSCLCTInAChamber = 0;	
	int CountWhenKHSNegativeAndLessThan2CLCTsInChamber = 0;
	int NumberofFirstCLCTsWithNegativeKHS = 0;		

	for(int i = start; i < end; i++)
	{
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		/* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;					
				
		//loop over chambers, generate emulated CLCTs			
		for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
		{
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);
			bool me13 = (ST == 1 && RI == 3);

			bool threeLayerChamber = (me11a || me11b) && CH == 11;
			if(!threeLayerChamber) continue;

			//
			// Emulate the TMB to find all the CLCTs
			//

			ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> emulatedCLCTs;

			if(searchForMatch(compHits,oldPatterns, emulatedCLCTs,true))
			{
				emulatedCLCTs.clear();
				//cout << "Something broke" << endl;
				//return;

				continue;
			}

			while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();

			vector< unsigned int> matchedemuCLCTs;							
			int matchedemuCLCTcount = 0;			

			for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++)
			{
				int segId = segments.ch_id->at(thisSeg);
				if(segId != chamberHash) continue;
				float segmentX = segments.pos_x->at(thisSeg);

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

				float Pt = muons.pt->at(segments.mu_id->at(thisSeg));
				if (Pt < 25) continue;
				
				//WN - Compiler is saying these are set, but not used
				//bool found3LayCLCT = false;
				//bool matched3LayCLCT = false;

				int closestCLCTtoSegmentIndex = -1;				
				float minDistanceSegmentToClosestCLCT = 1e5;
				

				for(unsigned int iclct = 0; iclct < emulatedCLCTs.size(); iclct++)
				{
					//WN - Also not used...
					//unsigned int qual = emulatedCLCTs.at(iclct)->layerCount();

					//if(qual == 3) found3LayCLCT = true;

					if(std::find(matchedemuCLCTs.begin(), matchedemuCLCTs.end(), iclct) != matchedemuCLCTs.end()) continue;
					float clctStripPos = emulatedCLCTs.at(iclct)->keyStrip();
					//if(me11a) clctStripPos -= 16*4;
					if(abs(clctStripPos - segmentX) < minDistanceSegmentToClosestCLCT)
					{
						minDistanceSegmentToClosestCLCT = abs(clctStripPos - segmentX);
						closestCLCTtoSegmentIndex = iclct;
					}
				}
				
					if(closestCLCTtoSegmentIndex != -1) //found one
					{					
						matchedemuCLCTcount++;	
						matchedemuCLCTs.push_back((unsigned int)closestCLCTtoSegmentIndex);
						unsigned int qual = emulatedCLCTs.at(closestCLCTtoSegmentIndex)->layerCount();

						//if(qual == 3) matched3LayCLCT = true;
						
						fillHist(clctLayerCounts, segId, qual);													
					}
					else
					{
						fillHist(clctLayerCounts,segId,0); //no associated CLCT					
					}

			} 

			//unmatched CLCTs
			for(unsigned int iclct = 0; iclct < emulatedCLCTs.size(); iclct++)
			{				
				unsigned int qual = emulatedCLCTs.at(iclct)->layerCount();
				fillHist(allClctLayerCounts, chamberHash, qual);
				if(std::find(matchedemuCLCTs.begin(), matchedemuCLCTs.end(), iclct) != matchedemuCLCTs.end()) continue;
				fillHist(unmatchedClctLayerCounts, chamberHash, qual); 
			}

			unsigned int emulatedCLCTcount = emulatedCLCTs.size();
			unsigned int realCLCTcount = clcts.size(chamberHash);

			if(emulatedCLCTcount != realCLCTcount && EC == 2)
			{			
				cout << "Event: " << i << endl;				
				compHits.print();

				cout << "Emulated (" << emulatedCLCTcount << "): " << endl;
				for (unsigned int i = 0; i < emulatedCLCTcount; i++)
				{				 
					cout << i+1 << ". Layer Count: " << emulatedCLCTs.at(i)->layerCount() << ", Key Half Strip: " << emulatedCLCTs.at(i)->keyHalfStrip() << ", Pattern ID: " << emulatedCLCTs.at(i)->patternId() << endl;					
				}

				cout << "Real (" << realCLCTcount << "): " << endl;
				int j = 0;
				for(unsigned int i= 0; i < clcts.size(); i++)
				{
					if(chamberHash == clcts.ch_id->at(i)) 
					{
						int KHS = (int)clcts.keyStrip->at(i);
						if(me11a) KHS -= 128;
						if(me11a && KHS < 0)
						{
							cout << j+1 << ". Layer Count: " << clcts.quality->at(i) << ", Key Half Strip: " << KHS << " (or ME11B half strip " << KHS + 128 << "?) , Pattern ID: " << (unsigned int)clcts.pattern->at(i) << endl;
						}
						else
						{
							cout << j+1 << ". Layer Count: " << clcts.quality->at(i) << ", Key Half Strip: " << KHS << ", Pattern ID: " << (unsigned int)clcts.pattern->at(i) << endl;
						}  

					  j++; 

					} 					
				}		

				cout << endl; 

				if(emulatedCLCTs.size() > clcts.size(chamberHash))
				{
					for(unsigned int i = 0; i < emulatedCLCTs.size(); i++)
					{
						for(unsigned int j = 0; j < clcts.size(); j++)
						{
							if(chamberHash == clcts.ch_id->at(j))
							{
								if(emulatedCLCTs.at(i)->layerCount() != clcts.quality->at(j))
								{
									RealvsEmulated_Layers1->Fill(emulatedCLCTs.at(i)->layerCount());
								}
							}
						}
					}
				} 
				

				if(emulatedCLCTs.size() < clcts.size(chamberHash))
				{
					for(unsigned int i = 0; i < clcts.size(); i++)
					{			
						if(chamberHash == clcts.ch_id->at(i))
						{
							for(unsigned int j = 0; j < emulatedCLCTs.size(); j++)
							{
								if(emulatedCLCTs.at(j)->layerCount() != clcts.quality->at(i))
								{
									RealvsEmulated_Layers2->Fill(clcts.quality->at(i));
								}
							}
						}
					}
				} 							

			} 	

			if(me11a)
			{				
				int counter = 0;	

				int k = 0;

				for(unsigned int i = 0; i < clcts.size(); i++)
				{	 					
					if(chamberHash == clcts.ch_id->at(i))
					{ 						
						int KHS = (int)clcts.keyStrip->at(i) - 128;					
						if(KHS < 0)
						{						
							counter++;
							if(clcts.size(chamberHash) != 2) 
							{
								CountWhenKHSNegativeAndLessThan2CLCTsInChamber++;
								cout << endl << " error, less than 2 CLCTs" << endl;
							}
							if (k == 0) 
							{
								NumberofFirstCLCTsWithNegativeKHS++;
								cout << endl << "error, first CLCT with negative KHS" << endl;
							}

						} 

						k++;
					}					
				}

				if(counter > 1)
				{
					CounterMoreThanOneNegativeKHSCLCTInAChamber++;
					cout << endl << "error, more than one CLCT with negative KHS" << endl;
				}

			} 

			RealVsEmulated->Fill(emulatedCLCTcount,realCLCTcount);

			if(me11a) 
			{
				RealVsEmulated_ME11A->Fill(emulatedCLCTcount,realCLCTcount); 
				if(EC==2)
				{
					RealVsEmulated_ME11A_EC2->Fill(emulatedCLCTcount,realCLCTcount); 
				}
			}

			if(me11b) 
			{
				RealVsEmulated_ME11B->Fill(emulatedCLCTcount,realCLCTcount); 
				if(EC==2)
				{
					RealVsEmulated_ME11B_EC2->Fill(emulatedCLCTcount,realCLCTcount); 
				}
			}

			if(EC==2) RealVsEmulated_EC2->Fill(emulatedCLCTcount,realCLCTcount); 

	  }
	
  }

	for(auto hist : allClctLayerCounts) hist.second->Write();
	for(auto hist : clctLayerCounts) hist.second->Write();
	for(auto hist : unmatchedClctLayerCounts) hist.second->Write();
	RealVsEmulated->Write();
	RealVsEmulated_ME11B->Write();
	RealVsEmulated_ME11A->Write();  	
	RealVsEmulated_ME11B_EC2->Write(); 
	RealVsEmulated_ME11A_EC2->Write(); 
	RealVsEmulated_EC2->Write(); 
	RealvsEmulated_Layers1->Write();
	RealvsEmulated_Layers2->Write();
	outF->Close();


	cout << endl << "Number of times there were less than 2 CLCTs in a chamber containing a CLCT with negative KHS during an event: " << CountWhenKHSNegativeAndLessThan2CLCTsInChamber;
	cout << endl << "Number of times the first CLCT had negative KHS: " << NumberofFirstCLCTsWithNegativeKHS;
	cout << endl << "Number of times more than one CLCT with negative KHS was found in the same chamber during an event: " << CounterMoreThanOneNegativeKHSCLCTInAChamber << endl << endl;

	cout << "Wrote to file: " << outputfile << endl;

	return 0;
}
