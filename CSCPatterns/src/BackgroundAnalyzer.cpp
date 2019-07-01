/*
 * BackgroundAnalyzer.cpp
 *
 *  Created on: June 6, 2019
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

int BackgroundAnalyzer(string inputfile, string outputfile, int start=0, int end=-1) 
{

	auto t1 = std::chrono::high_resolution_clock::now();

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

  EmulatedCLCTs OldEmulatedclcts(t_emu,1);
	EmulatedCLCTs NewEmulatedclcts(t_emu,2);


	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
    {
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

	TH2D* SegNoMuon = new TH2D ("Segments not associated to a muon, Chamber Type", "Segments not associated to a muon, Chamber Type", 4,1,5,4,1,5);

	TH1F* OPMatchedPt = new TH1F ("CLCT matched to muon's segment, Pt (OP)", "CLCT matched to muon's segment, Pt (OP);Pt [GeV]; Count", 20, 0, 100);
	TH1F* OPMatchedEta = new TH1F ("CLCT matched to muon's segment, Eta (OP)", "CLCT matched to muon's segment, Eta (OP);Eta; Count", 50, -3, 3);
	TH1F* OPMatchedPhi = new TH1F ("CLCT matched to muon's segment, Phi (OP)", "CLCT matched to muon's segment, Phi (OP);Phi; Count", 50, -3, 3);
	TH2D* OPMuonBGCT = new TH2D ("Muon BG Chamber Type (OP)", "Muon BG Chamber Type (OP);Station;Ring", 4, 1, 5, 4, 1, 5);
	TH1F* OPOtherBGLC = new TH1F ("Other BG Layer Count (OP)", "Other BG Layer Count (OP);Layers;Count", 7, 0, 7);
	TH1F* OPOtherBGPI = new TH1F ("Other BG Pattern ID (OP)", "Other BG Pattern ID (OP);PID;Count", 9, 2, 11);
	TH2D* OPSegMoreThanCLCTs = new TH2D ("_____ Background Chamber Type (OP)", "_____ Background Chamber Type (OP);Station;Ring", 4, 1, 5, 4, 1, 5);
	TH2D* OPCLCTsMoreThanSeg = new TH2D ("Other Background Chamber Type (OP)", "Other Background Chamber Type (OP);Station;Ring", 4, 1, 5, 4, 1, 5);

	TH1F* NPMatchedPt = new TH1F ("CLCT matched to muon's segment, Pt (NP)", "CLCT matched to muon's segment, Pt (NP);Pt [GeV]; Count", 20, 0, 100);
	TH1F* NPMatchedEta = new TH1F ("CLCT matched to muon's segment, Eta (NP)", "CLCT matched to muon's segment, Eta (NP);Eta; Count", 50, -3, 3);
	TH1F* NPMatchedPhi = new TH1F ("CLCT matched to muon's segment, Phi (NP)", "CLCT matched to muon's segment, Phi (NP);Phi; Count", 50, -3, 3);
	TH2D* NPMuonBGCT = new TH2D ("Muon BG Chamber Type (NP)", "Muon BG Chamber Type (NP);Station;Ring", 4, 1, 5, 4, 1, 5);
	TH1F* NPOtherBGLC = new TH1F ("Other BG Layer Count (NP)", "Other BG Layer Count (NP);Layers;Count", 7, 0, 7);
	TH1F* NPOtherBGPI = new TH1F ("Other BG Pattern ID (NP)", "Other BG Pattern ID (NP);PID/10;Count", 5, 6, 11);
	TH2D* NPSegMoreThanCLCTs = new TH2D ("_____ Background Chamber Type (NP)", "_____ Background Chamber Type (NP);Station;Ring", 4, 1, 5, 4, 1, 5);
	TH2D* NPCLCTsMoreThanSeg = new TH2D ("Other Background Chamber Type (NP)", "Other Background Chamber Type (NP);Station;Ring", 4, 1, 5, 4, 1, 5);


	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();
	
	cout << "Starting Event: " << start << " Ending Event: " << endl << endl;

	unsigned long long int x = 0;
	unsigned long long int y = 0;

	unsigned long long int seg_muon = 0;
	unsigned long long int seg_nomuon = 0;

  unsigned long long int OPTotalThreeLayerMin = 0;
	unsigned long long int OPTotalMatches = 0;
	unsigned long long int OPSignal = 0;
	unsigned long long int OPMuonBGCounter = 0;
	unsigned long long int OPOtherBGCounter = 0;
	unsigned long long int OPMismatchCounter = 0;
	unsigned long long int OPCLCTsu = 0;
	unsigned long long int OPsegmorethanclcts = 0;
	unsigned long long int OPclctsmorethanseg = 0;

	unsigned long long int NPTotalThreeLayerMin = 0;
	unsigned long long int NPTotalMatches = 0;
	unsigned long long int NPSignal = 0;
	unsigned long long int NPMuonBGCounter = 0;
	unsigned long long int NPOtherBGCounter = 0;
	unsigned long long int NPMismatchCounter = 0;
	unsigned long long int NPCLCTsu = 0;
	unsigned long long int NPsegmorethanclcts = 0;
	unsigned long long int NPclctsmorethanseg = 0;

	for(int i = start; i < end; i++) 
  {
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

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
			int OPSignalCounter = 0;
			int OPBGCounter = 0;

			int NPSignalCounter = 0;
			int NPBGCounter = 0;

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
			bool me13 = (ST == 1 && RI == 3);

			vector<unsigned int> matchedOPseg;
			vector<unsigned int> matchedNPseg;

			vector<unsigned int> matchedOPCLCTs;
			vector<unsigned int> matchedNPCLCTs;
	
			//total segments in chamber
			unsigned int segcount = 0;
			for(unsigned int i = 0; i < segments.size(); i++)
			{
				if((unsigned int)segments.ch_id->at(i) != chamberHash)				
				continue;
				segcount++;
			}


			//segments associated to muons
			unsigned int segcount_muon = 0;
			for(unsigned int i = 0; i < segments.size(); i++)
			{
				if((unsigned int)segments.ch_id->at(i) != chamberHash)				
				continue;

				if(segments.mu_id->at(i) == -1)
				{
					seg_nomuon++;	
					SegNoMuon->Fill(ST,RI);
				}
				else
				{					
					seg_muon++;
				}	
				
			}


			//CLCT count in chamber
			unsigned int OPCLCTCount = 0;
			unsigned int NPCLCTCount = 0;

			int counter = 0;
			
			for(unsigned int i = 0; i < OldEmulatedclcts.size(); i++)
			{
				if(OldEmulatedclcts.ch_id->at(i) != chamberHash)
				continue;

				if(OldEmulatedclcts.layerCount->at(i) < 3)
				continue;

				if(counter > 1)
				{
					continue;
				}
				else
				{
					counter++;
				}

				OPCLCTCount++;
			}

			counter = 0;

			for(unsigned int i = 0; i < NewEmulatedclcts.size(); i++)
			{
				if(NewEmulatedclcts.ch_id->at(i) != chamberHash)
				continue;

				if(NewEmulatedclcts.layerCount->at(i) < 3)
				continue;

				if(counter > 1)
				{
					continue;
				}
				else
				{
					counter++;
				}

				NPCLCTCount++;
			}

			OPTotalThreeLayerMin+=OPCLCTCount;
			NPTotalThreeLayerMin+=NPCLCTCount;

			//_____ Background: segcount > CLCTcount, Other Background: CLCTCount > segcount

			if(segcount > OPCLCTCount)
			{
				OPsegmorethanclcts++;
				OPSegMoreThanCLCTs->Fill(ST,RI);				
			}

			if(segcount < OPCLCTCount)
			{
				OPclctsmorethanseg++;
				OPCLCTsMoreThanSeg->Fill(ST,RI);		
			}			

			if(segcount > NPCLCTCount)
			{
				NPsegmorethanclcts++;
				NPSegMoreThanCLCTs->Fill(ST,RI);				
			}
			
			if(segcount < NPCLCTCount)
			{
				NPclctsmorethanseg++;
				NPCLCTsMoreThanSeg->Fill(ST,RI);
			}

			if(OPCLCTCount > NPCLCTCount)
			{
				y++;
				/*compHits.print();
				cout << OPCLCTCount << " " << NPCLCTCount << endl;
				cout << endl;*/
			}
			

			for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
			{

				if((unsigned int)segments.ch_id->at(iseg) != chamberHash)				
				continue;
				
				float segmentX = segments.pos_x->at(iseg);			
					
				//ignore segments at edges of chamber
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

				int closestOPCLCTtoSegmentIndex = -1;
				float minDistanceSegmentToClosestOPCLCT = 1e5;

				int counter0 = 0; //2 CLCTs sent out

				for(unsigned int iclct = 0; iclct < OldEmulatedclcts.size(); iclct++)
				{
					
					if(OldEmulatedclcts.ch_id->at(iclct) != chamberHash)					
					continue;					
				
					if(OldEmulatedclcts.layerCount->at(iclct) < 3) 					
					continue;

					if(counter0 > 1)
					continue;
					else
					{
						counter0++;	
					} 				 					
					
					if(std::find(matchedOPCLCTs.begin(), matchedOPCLCTs.end(), iclct) != matchedOPCLCTs.end())
					continue;
					
					float OPclctStripPos = OldEmulatedclcts.keyStrip->at(iclct);
										
					if(abs(OPclctStripPos - segmentX) < minDistanceSegmentToClosestOPCLCT)
					{
						minDistanceSegmentToClosestOPCLCT = abs(OPclctStripPos - segmentX);
						closestOPCLCTtoSegmentIndex = iclct;
					}

				}

				if(closestOPCLCTtoSegmentIndex != -1) //match
				{
					matchedOPCLCTs.push_back(closestOPCLCTtoSegmentIndex);
					matchedOPseg.push_back(iseg);

					OPTotalMatches++;
				 
					if(segments.mu_id->at(iseg) == -1) //not a muon
					{
						OPBGCounter++;
						OPMuonBGCounter++;
						OPMuonBGCT->Fill(ST,RI);
					}					
					else
					{
						OPSignal++;		
						OPSignalCounter++;				
						float Pt = muons.pt->at(segments.mu_id->at(iseg));
						OPMatchedPt->Fill(Pt);
						float eta = muons.eta->at(segments.mu_id->at(iseg));
						OPMatchedEta->Fill(eta);
						float phi = muons.phi->at(segments.mu_id->at(iseg));
						OPMatchedPhi->Fill(phi);
					}	
					
				}
				else
				{
					if(segcount > 0 && OldEmulatedclcts.size(chamberHash) > 0 && segcount == OldEmulatedclcts.size(chamberHash))
					OPMismatchCounter++;
				}

				int closestNPCLCTtoSegmentIndex = -1;
				float minDistanceSegmenttoClosestNPCLCT = 1e5;

				int counter1 = 0; //2 CLCTs sent out

				for(unsigned int iclct = 0; iclct < NewEmulatedclcts.size(); iclct++)
				{					
					if(NewEmulatedclcts.ch_id->at(iclct) != chamberHash)
					continue;				
					
					if(NewEmulatedclcts.layerCount->at(iclct) < 3)
					continue;

					if(counter1 > 1)
					continue;
					else
					{
						counter1++;
					}						

					if(std::find(matchedNPCLCTs.begin(), matchedNPCLCTs.end(), iclct) != matchedNPCLCTs.end())
					continue;
				
					float NPclctStripPos = NewEmulatedclcts.keyStrip->at(iclct);
					if(abs(NPclctStripPos - segmentX) < minDistanceSegmenttoClosestNPCLCT)
					{
						minDistanceSegmenttoClosestNPCLCT = abs(NPclctStripPos - segmentX);
						closestNPCLCTtoSegmentIndex = iclct;
					}

				}

				if(closestNPCLCTtoSegmentIndex != -1) //match
				{
					matchedNPCLCTs.push_back(closestNPCLCTtoSegmentIndex);
					matchedNPseg.push_back(iseg);

					NPTotalMatches++;
					
					if(segments.mu_id->at(iseg) == -1)
					{
						NPBGCounter++;
						NPMuonBGCounter++;
						NPMuonBGCT->Fill(ST,RI);
					}
					else
					{
						NPSignalCounter++;
						NPSignal++;						
						float Pt = muons.pt->at(segments.mu_id->at(iseg));
						NPMatchedPt->Fill(Pt);
						float eta = muons.eta->at(segments.mu_id->at(iseg));
						NPMatchedEta->Fill(eta);
						float phi = muons.phi->at(segments.mu_id->at(iseg));
						NPMatchedPhi->Fill(phi);						
					}

				}
				else
				{
					if(segcount > 0 && NewEmulatedclcts.size(chamberHash) > 0 && segcount == NewEmulatedclcts.size(chamberHash))
					NPMismatchCounter++;
				}			

				//printing out chambers with Old Patterns matching to muon segments but not the New ones (try to explain the difference in total matches to muon segments)

				/*if(closestOPCLCTtoSegmentIndex != -1 && closestNPCLCTtoSegmentIndex == -1 && segments.mu_id->at(iseg) != -1)
				{					
					compHits.print();
					cout << segmentX << " " << segcount << " " << OldEmulatedclcts.size(chamberHash) << " " << NewEmulatedclcts.size(chamberHash);
					cout << endl << endl;
				}*/


			}

			int counter0 = 0;

			if(OldEmulatedclcts.size(chamberHash) > segcount) //see how many of the CLCTs went unmatched (max 2)
			{				
				for(unsigned int iclct = 0; iclct < OldEmulatedclcts.size(); iclct++)
				{
					if(OldEmulatedclcts.ch_id->at(iclct) != chamberHash)
					continue;			

					if(OldEmulatedclcts.layerCount->at(iclct) < 3)
					continue;

					if(counter0 > 1)
					continue;
					else
					{
						counter0++;
					} 					

					if(std::find(matchedOPCLCTs.begin(), matchedOPCLCTs.end(), iclct) != matchedOPCLCTs.end())
					continue;
									
					OPOtherBGCounter++;
					OPOtherBGLC->Fill(int(OldEmulatedclcts.layerCount->at(iclct)));
					OPOtherBGPI->Fill(int(OldEmulatedclcts.patternId->at(iclct)));
					
				}

			}

			if(segcount > OldEmulatedclcts.size(chamberHash)) //see how many of the segments went unmatched
			{			

				for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
				{
					if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
					continue;

					if(std::find(matchedOPseg.begin(), matchedOPseg.end(), iseg) != matchedOPseg.end())
					continue;					
					
					OPCLCTsu++;					

				}				
				
			}

			
			int counter1 = 0;

			if(NewEmulatedclcts.size(chamberHash) > segcount) //see how many of the CLCTs went unmatched (max 2)
			{
				for(unsigned int i = 0; i < NewEmulatedclcts.size(); i++)
				{
					if(NewEmulatedclcts.ch_id->at(i) != chamberHash)
					continue;

					if(NewEmulatedclcts.layerCount->at(i) < 3)
					continue;

					if(counter1 > 1)
					continue;
					else
					{
						counter1++;
					} 				

					if(std::find(matchedNPCLCTs.begin(), matchedNPCLCTs.end(), i) != matchedNPCLCTs.end())
					continue;

					NPOtherBGCounter++;					
					NPOtherBGLC->Fill(int(NewEmulatedclcts.layerCount->at(i)));					
					NPOtherBGPI->Fill(int(NewEmulatedclcts.patternId->at(i))/10);

				}

			}

			if(segcount > NewEmulatedclcts.size(chamberHash)) //see how many of the segments went unmatched
			{
				for(unsigned int i = 0; i < segments.size(); i++)
				{					
					if((unsigned int)segments.ch_id->at(i) != chamberHash)
					continue;

					if(std::find(matchedNPseg.begin(), matchedNPseg.end(), i) != matchedNPseg.end())
					continue;

					NPCLCTsu++;

				}

			}					

		}	

		if(y != 0)
		x++;

	}

	outF->cd();
	SegNoMuon->Write();
	OPMatchedPt->Write();
	OPMatchedEta->Write();
	OPMatchedPhi->Write();
	OPMuonBGCT->Write();
	OPOtherBGLC->Write();
	OPOtherBGPI->Write();
	OPCLCTsMoreThanSeg->Write();
	OPSegMoreThanCLCTs->Write();
	NPMatchedPt->Write();
	NPMatchedEta->Write();
	NPMatchedPhi->Write();
	NPMuonBGCT->Write();
	NPOtherBGLC->Write();
	NPOtherBGPI->Write();
	NPCLCTsMoreThanSeg->Write();
	NPSegMoreThanCLCTs->Write();

	cout << "Segments associated to muons: " << seg_muon << endl;
	cout << "Segments not associated to muons: " << seg_nomuon << endl << endl;

	cout << "Old Patterns: " << endl;
	cout << "--------------------------------------------------" << endl;
	//cout << "Total # of CLCTs: " << OPTotal << endl;
	cout << "Total # of CLCTs (3 Layer Min): " << OPTotalThreeLayerMin << endl;
	cout << "Total # of segment matches: " << OPTotalMatches << endl;
	cout << "Matches to muon segments: " << OPSignal << endl;
	cout << "Muon Background: " << OPMuonBGCounter << endl;
	cout << "# chambers with n(CLCTs) > n(segments): " << OPclctsmorethanseg << endl;
	cout << "# excess CLCTs in chambers with n(CLCTs) > n(segments): " << OPOtherBGCounter << endl;
	//cout << "Mismatches: " << OPMismatchCounter << endl;
	cout << "# chambers with n(segments) > n(CLCTs): " << OPsegmorethanclcts << endl;
	cout << "# excess segments in chambers with n(segments) > n(CLCTs): " << OPCLCTsu << endl << endl;

	cout << "New Patterns: " << endl;
	cout << "--------------------------------------------------" << endl;
	//cout << "Total # of CLCTs: " << NPTotal << endl;
	cout << "Total # of CLCTs (3 Layer Min): " << NPTotalThreeLayerMin << endl;
	cout << "Total # of segment matches: " << NPTotalMatches << endl;
	cout << "Matches to muon segments: " << NPSignal << endl;
	cout << "Muon Background: " << NPMuonBGCounter << endl;
	cout << "# chambers with n(CLCTs) > n(segments): " << NPclctsmorethanseg << endl;
	cout << "# excess CLCTs in chambers with n(CLCTs) > n(segments): " << NPOtherBGCounter << endl;
	//cout << "Mismatches: " << NPMismatchCounter << endl;
	cout << "# chambers with n(segments) > n(CLCTs): " << NPsegmorethanclcts << endl;
	cout << "# excess segments in chambers with n(segments) > n(CLCTs): " << NPCLCTsu << endl << endl;	 

	cout <<"Events with atleast one chamber having more than one hit on a layer within 3 halfstrips: " << y << endl << endl;
	
	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << endl << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;
	return 0;
}



int main(int argc, char* argv[])
{
	try 
    {
		switch(argc)
        {
		case 3:
			return BackgroundAnalyzer(string(argv[1]), string(argv[2]));
		case 4:
			return BackgroundAnalyzer(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return BackgroundAnalyzer(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./BackgroundAnalyzer inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) 
    {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}