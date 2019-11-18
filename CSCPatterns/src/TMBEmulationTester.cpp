/*
 * PatternFinder.cpp
 *
 *  Created on: Nov. 7 2018
 *      Author: root
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


#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"
#include "../include/LUTClasses.h"

#include "../include/TMBEmulationTester.h"

using namespace std;

int main(int argc, char* argv[]){
	TMBEmulationTester p;
	return p.main(argc,argv);
}

int TMBEmulationTester::run(string inputfile, string outputfile, int start, int end) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

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

	//
	// MAKE ALL THE PATTERNS
	//


	//vector<CSCPattern>* newPatterns = createNewPatterns();
	vector<CSCPattern>* oldPatterns = createOldPatterns();

	//
	// TREE ITERATION
	//

	enum CLCT_EMUL_MATCH {
		real,
		emulated,
		noEmulated,
		noReal,
		match,
		perfectMatch,
		MATCH_SIZE
	};


	enum CLCT_OFFSET_ENUM {
		anyOffset, //strips
		offset2,
		offset1,
		noOffset,
		OFFSET_SIZE
	};

	enum CLCT_PATTERN_ENUM {
		anyPattern,
		samePattern,
		sameLayers,
		PATTERN_SIZE,
	};

	TH1F* emulationMatching = new TH1F("emulationMatching", "emulationMatching; ; CLCTs",MATCH_SIZE,0,MATCH_SIZE);
	emulationMatching->GetXaxis()->SetBinLabel(real+1, "real");
	emulationMatching->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	emulationMatching->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	emulationMatching->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	emulationMatching->GetXaxis()->SetBinLabel(match+1, "match");
	emulationMatching->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");
	
	TH1F* threelayer = new TH1F("threelayer", "threelayer; ; CLCTs",MATCH_SIZE,0,MATCH_SIZE);
	threelayer->GetXaxis()->SetBinLabel(real+1, "real");
	threelayer->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	threelayer->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	threelayer->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	threelayer->GetXaxis()->SetBinLabel(match+1, "match");
	threelayer->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");
	
	TH1F* threelayerA = new TH1F("threelayerA", "threelayerA; ; CLCTs",MATCH_SIZE,0,MATCH_SIZE);
	threelayer->GetXaxis()->SetBinLabel(real+1, "real");
	threelayer->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	threelayer->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	threelayer->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	threelayer->GetXaxis()->SetBinLabel(match+1, "match");
	threelayer->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");
	
	TH1F* threelayerB = new TH1F("threelayerB", "threelayerB; ; CLCTs",MATCH_SIZE,0,MATCH_SIZE);
	threelayer->GetXaxis()->SetBinLabel(real+1, "real");
	threelayer->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	threelayer->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	threelayer->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	threelayer->GetXaxis()->SetBinLabel(match+1, "match");
	threelayer->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");
	
	map<pair<int,int>, TH1F*> emulationMatchingByChamber; //map to break up output by chamber type
	for(unsigned int itr =0; itr < NCHAMBERS; itr++)
	{
		unsigned int station = CHAMBER_ST_RI[itr][0];
		unsigned int ring = CHAMBER_ST_RI[itr][1];
		auto key = make_pair(station,ring);
		string histname = "h_" + CHAMBER_NAMES[itr];
		auto ChamberHist = new TH1F(histname.c_str(), histname.c_str(), MATCH_SIZE,0,MATCH_SIZE);		
		ChamberHist->GetXaxis()->SetBinLabel(real+1, "real");
	    ChamberHist->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	    ChamberHist->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	    ChamberHist->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	    ChamberHist->GetXaxis()->SetBinLabel(match+1, "match");
	    ChamberHist->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");		
		emulationMatchingByChamber[key] = ChamberHist;
		
	}
	
	map<int, TH1F*> emulationMatchingByEC_ME11A; //map to break up output for ME11A by endcap value
	for(unsigned int i=0; i<2; i++)
	{
	TH1F* EC_hist = new TH1F(("ME11A_EC"+to_string(i+1)).c_str(), ("ME11A_EC"+to_string(i+1)).c_str(), MATCH_SIZE,0,MATCH_SIZE);
	EC_hist->GetXaxis()->SetBinLabel(real+1, "real");
	EC_hist->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	EC_hist->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	EC_hist->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	EC_hist->GetXaxis()->SetBinLabel(match+1, "match");
	EC_hist->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");	
	emulationMatchingByEC_ME11A[i] = EC_hist;
	}
	
	/*TH1F* EC1 = new TH1F("ME11A_EC1","ME11A_EC1",MATCH_SIZE,0,MATCH_SIZE);
	EC1->GetXaxis()->SetBinLabel(real+1, "real");
	EC1->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	EC1->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	EC1->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	EC1->GetXaxis()->SetBinLabel(match+1, "match");
	EC1->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");	
	TH1F* EC2 = new TH1F("ME11A_EC2","ME11A_EC2",MATCH_SIZE,0,MATCH_SIZE);
	EC2->GetXaxis()->SetBinLabel(real+1, "real");
	EC2->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	EC2->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	EC2->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	EC2->GetXaxis()->SetBinLabel(match+1, "match");
	EC2->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");	
	emulationMatchingByEC_ME11A[0] = EC1;
	emulationMatchingByEC_ME11A[1] = EC2;*/
	
	map<int, TH1F*> emulationMatchingByEC_ME11A_evenCH; //map to break up output for ME11A by endcap value
	for(unsigned int i=0; i<2; i++)
	{
	TH1F* EC_hist = new TH1F(("EvenCH_EC"+to_string(i)).c_str(), ("EvenCH_EC"+to_string(i)).c_str(), MATCH_SIZE,0,MATCH_SIZE);
	EC_hist->GetXaxis()->SetBinLabel(real+1, "real");
	EC_hist->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	EC_hist->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	EC_hist->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	EC_hist->GetXaxis()->SetBinLabel(match+1, "match");
	EC_hist->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");	
	emulationMatchingByEC_ME11A_evenCH[i] = EC_hist;
	}
	
	map<int, TH1F*> emulationMatchingByEC_ME11A_oddCH; //map to break up output for ME11A by endcap value
	for(unsigned int i=0; i<2; i++)
	{
	TH1F* EC_hist = new TH1F(("OddCH_EC"+to_string(i)).c_str(), ("OddCH_EC"+to_string(i)).c_str(), MATCH_SIZE,0,MATCH_SIZE);
	EC_hist->GetXaxis()->SetBinLabel(real+1, "real");
	EC_hist->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	EC_hist->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	EC_hist->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	EC_hist->GetXaxis()->SetBinLabel(match+1, "match");
	EC_hist->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");	
	emulationMatchingByEC_ME11A_oddCH[i] = EC_hist;
	}
	
	map<pair<int,int>, TH1F*> minDistanceToCLCTByChamber;
	for(unsigned int i=0; i<NCHAMBERS; i++)
	{
	unsigned int stationdist = CHAMBER_ST_RI[i][0];
	unsigned int ringdist = CHAMBER_ST_RI[i][1];
	auto key1 = make_pair(stationdist, ringdist);
	string histname1 = "mindist_" + CHAMBER_NAMES[i];
	auto Chamberhdist = new TH1F(histname1.c_str(), histname1.c_str(), 50, -10,10);
	Chamberhdist->GetXaxis()->SetTitle("(real CLCT position) - (emulated CLCT position)");
	Chamberhdist->GetYaxis()->SetTitle("CLCTs");
	minDistanceToCLCTByChamber[key1] = Chamberhdist;	  
	}
	
	TH2F* emulationPattVsOffset = new TH2F("emulationPattVsOffset", "emulationPattVsOffset;;;", OFFSET_SIZE, 0,OFFSET_SIZE,PATTERN_SIZE,0,PATTERN_SIZE);
	emulationPattVsOffset->GetXaxis()->SetBinLabel(anyOffset+1,"anyOffset");
	emulationPattVsOffset->GetXaxis()->SetBinLabel(offset2+1,"Offset <= 2 HS");
	emulationPattVsOffset->GetXaxis()->SetBinLabel(offset1+1,"Offset <= 1 HS");
	emulationPattVsOffset->GetXaxis()->SetBinLabel(noOffset+1,"No Offset");

	emulationPattVsOffset->GetYaxis()->SetBinLabel(anyPattern+1,"anyPattern");
	emulationPattVsOffset->GetYaxis()->SetBinLabel(samePattern+1,"samePattern");
	emulationPattVsOffset->GetYaxis()->SetBinLabel(sameLayers+1,"sameLayers");


	TH1F* emulationStripDiff = new TH1F("emulationStripDiff", "emulationStripDiff; Real - Emulated [HS]; CLCTs", 10,-5,5);
	TH1F* emulatedLayerCount = new TH1F("emulatedLayerCount","emulatedLayerCount; Layers; CLCTs",6,1,7);
	TH1F* realLayerCount = new TH1F("realLayerCount","realLayerCount; Layers; CLCTs",6,1,7);
	TH1F* emulatedMultiplicity = new TH1F("emulatedMultiplicity", "emulatedMultiplicity; CLCT Multiplicity; CLCTs", 10,0,10);
	TH1F* realMultiplicity = new TH1F("realMultiplicity", "realMultiplicity; CLCT Multiplicity; CLCTs", 10,0,10);

	//how many times we match to the first clct
	unsigned int clct0 = 0;
	unsigned int match_clct0 = 0;
	unsigned int pmatch_clct0 = 0;
	
	unsigned int threelayerclct0 = 0;
	unsigned int threelayermatch_clct0 = 0;
	unsigned int threelayerpmatch_clct0 = 0;
	
	unsigned int threelayerclct0A = 0;
	unsigned int threelayermatch_clct0A = 0;
	unsigned int threelayerpmatch_clct0A = 0;
	
	unsigned int threelayerclct0B = 0;
	unsigned int threelayermatch_clct0B = 0;
	unsigned int threelayerpmatch_clct0B = 0;	
	
	map<pair<int,int>,int> clct00;
	map<pair<int,int>,int> match_clct00;
	map<pair<int,int>,int> pmatch_clct00;
	for(unsigned int i=0; i<NCHAMBERS; i++)
	{
	unsigned int s = CHAMBER_ST_RI[i][0];
	unsigned int r = CHAMBER_ST_RI[i][1];
	auto key = make_pair(s,r);
	clct00[key]=0;
	match_clct00[key]=0;
	pmatch_clct00[key]=0;
	}	

	map<int,int> clct0_ME11A_EC;
	clct0_ME11A_EC[0]=0;
	clct0_ME11A_EC[1]=0;
	map<int,int> match_clct0_ME11A_EC;
	match_clct0_ME11A_EC[0]=0;
	match_clct0_ME11A_EC[1]=0;
	map<int,int> pmatch_clct0_ME11A_EC;
	pmatch_clct0_ME11A_EC[0]=0;
	pmatch_clct0_ME11A_EC[1]=0;
	
	map<pair<int,int>,int> clct0_ME11A_EC_CH;
	clct0_ME11A_EC_CH[make_pair(0,0)]=0;
	clct0_ME11A_EC_CH[make_pair(0,1)]=0;
	clct0_ME11A_EC_CH[make_pair(1,0)]=0;
	clct0_ME11A_EC_CH[make_pair(1,1)]=0;
	map<pair<int,int>,int> match_clct0_ME11A_EC_CH;
	match_clct0_ME11A_EC_CH[make_pair(0,0)]=0;
	match_clct0_ME11A_EC_CH[make_pair(0,1)]=0;
	match_clct0_ME11A_EC_CH[make_pair(1,0)]=0;
	match_clct0_ME11A_EC_CH[make_pair(1,1)]=0;
	map<pair<int,int>,int> pmatch_clct0_ME11A_EC_CH;
	pmatch_clct0_ME11A_EC_CH[make_pair(0,0)]=0;
	pmatch_clct0_ME11A_EC_CH[make_pair(0,1)]=0;
	pmatch_clct0_ME11A_EC_CH[make_pair(1,0)]=0;
	pmatch_clct0_ME11A_EC_CH[make_pair(1,1)]=0;
	
	int negativeKHSCLCT = 0;
	
	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	cout << endl;

	printf("Starting Event = %i, Ending Event = %i\n", start, end);           

	for(int i = start; i < end; i++)
	{
		if(!(i%100)) 
		{
			printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);
			cout << endl;
		}

		t->GetEntry(i);

		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		
		//cout << "CLCTs in event: " << i << " = " << clcts.size() << endl;

		//
		//Iterate through all possible chambers
		//
		for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++){
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);

			//
			// Emulate the TMB to find all the CLCTs
			//


			ChamberHits compHits(ST, RI, EC, CH);
			if(compHits.fill(comparators)) return -1;		

			vector<CLCTCandidate*> emulatedCLCTs;	

			if(searchForMatch(compHits, oldPatterns, emulatedCLCTs, true)){
				emulatedCLCTs.clear();
				//cout << "Something broke" << endl;
				//return;

				continue;
			} 		

			//remove 3 layer emulated clcts
			bool threeLayerChamber = (me11a || me11b) && CH == 11 && EC ==1;
			if(!threeLayerChamber) {
				for(unsigned int iemu =0; iemu < emulatedCLCTs.size(); iemu++){
					if(emulatedCLCTs.at(iemu)->layerCount() == 3) {
						emulatedCLCTs.erase(emulatedCLCTs.begin()+iemu);
						iemu--;
					}
				}
			}

			//only take two leading clcts
			while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();			


			for(auto emu : emulatedCLCTs){
				emulatedLayerCount->Fill(emu->layerCount());
			}


			vector<unsigned int> matchedIndices;
			vector<unsigned int> threelayermatchedIndices;
			vector<unsigned int> threelayermatchedIndicesA;
			vector<unsigned int> threelayermatchedIndicesB;

			//among the matches
			vector<unsigned int> match_offsetLE2; //less than or equal to 2 [hs]
			vector<unsigned int> match_offsetLE1;
			vector<unsigned int> match_noOffset;

			vector<unsigned int> match_samePattern;
			vector<unsigned int> match_sameLayers;

			vector<unsigned int> perfectMatches;
			vector<unsigned int> threelayerperfectMatches;
			vector<unsigned int> threelayerperfectMatchesA;
			vector<unsigned int> threelayerperfectMatchesB;


			unsigned int clctsInChamber = 0;
			unsigned int threelayerclctsInChamber =0;
			unsigned int threelayerclctsInChamberA =0;
			unsigned int threelayerclctsInChamberB =0;
			
			//
			// Iterate over real clcts
			//
			for(unsigned int iclct=0; iclct < clcts.size(); iclct++){
				int clctHash = clcts.ch_id->at(iclct);
				if(clctHash != chamberHash) continue;
				float clctHSPos = clcts.keyStrip->at(iclct); //key strip is in units of half strips...
				if(me11a) clctHSPos -= 32*4;
				if(clctHSPos >= float(0))
				{
				clctsInChamber++;
				if(threeLayerChamber && clcts.quality->at(iclct)==3)
				{
				threelayerclctsInChamber++;
				if(ST==1 && RI==1)
				threelayerclctsInChamberB++;
				if(ST==1 && RI==4)
				threelayerclctsInChamberA++;
				}
				if(clctsInChamber == 1) 
				{
				clct0++; //hope that the first one is ordered correctly...
				if(threeLayerChamber && clcts.quality->at(iclct)==3)
				{
				threelayerclct0++;
				if(ST==1 && RI==1)
				threelayerclct0B++;
				if(ST==1 && RI==4)
				threelayerclct0A++;						
				}
				clct00[make_pair(ST,RI)]++;
				if(ST==1 && RI==4 && EC==1)
				clct0_ME11A_EC[0]++;
				if(ST==1 && RI==4 && EC==2)
				clct0_ME11A_EC[1]++;
				if(ST==1 && RI==4 && EC==1 && (CH%2)==0)
				clct0_ME11A_EC_CH[make_pair(0,0)]++;
				if(ST==1 && RI==4 && EC==1 && (CH%2)==1)
				clct0_ME11A_EC_CH[make_pair(0,1)]++;
				if(ST==1 && RI==4 && EC==2 && (CH%2)==0)
				clct0_ME11A_EC_CH[make_pair(1,0)]++;
				if(ST==1 && RI==4 && EC==2 && (CH%2)==1)
				clct0_ME11A_EC_CH[make_pair(1,1)]++;
				}
				realLayerCount->Fill(clcts.quality->at(iclct));  				

				int closestEmu = -1;
				float minDistanceToCLCT = 1e5;
				for(unsigned int iemu=0; iemu < emulatedCLCTs.size(); iemu++){
					if(find(matchedIndices.begin(), matchedIndices.end(), iemu) != matchedIndices.end()) continue;
					
					float distance = clctHSPos - emulatedCLCTs.at(iemu)->keyHalfStrip();								
					if(abs(distance) <  abs(minDistanceToCLCT)){
						minDistanceToCLCT = distance;
						closestEmu = iemu;
					}
				}

				bool perfMatch = false; //if we found a perfect match
				if(closestEmu != -1){ //found a match
					matchedIndices.push_back(closestEmu);
					if(threeLayerChamber && clcts.quality->at(iclct)==3)
					{
					threelayermatchedIndices.push_back(closestEmu);
					if(ST==1 && RI==1)
					threelayermatchedIndicesB.push_back(closestEmu);
					if(ST==1 && RI==4)
					threelayermatchedIndicesA.push_back(closestEmu);
					}
					emulationStripDiff->Fill(minDistanceToCLCT);
					if(clctsInChamber == 1)
					{
					match_clct0++;
					if(threeLayerChamber && clcts.quality->at(iclct)==3)
					{
					threelayermatch_clct0++;
					if(ST==1 && RI==1)
					threelayermatch_clct0B++;
					if(ST==1 && RI==4)
					threelayermatch_clct0A++;
					}
					match_clct00[make_pair(ST,RI)]++;
				        if(ST==1 && RI==4 && EC==1)
				        match_clct0_ME11A_EC[0]++;
				        if(ST==1 && RI==4 && EC==2)
				        match_clct0_ME11A_EC[1]++;
				        if(ST==1 && RI==4 && EC==1 && (CH%2)==0)
				        match_clct0_ME11A_EC_CH[make_pair(0,0)]++;
				        if(ST==1 && RI==4 && EC==1 && (CH%2)==1)
				        match_clct0_ME11A_EC_CH[make_pair(0,1)]++;
				        if(ST==1 && RI==4 && EC==2 && (CH%2)==0)
				        match_clct0_ME11A_EC_CH[make_pair(1,0)]++;
				        if(ST==1 && RI==4 && EC==2 && (CH%2)==1)
				        match_clct0_ME11A_EC_CH[make_pair(1,1)]++;
					}					
						
					minDistanceToCLCTByChamber[make_pair(ST,RI)]->Fill(minDistanceToCLCT);
					
					if(abs(minDistanceToCLCT) <= 2) match_offsetLE2.push_back(closestEmu);
					if(abs(minDistanceToCLCT) <= 1) match_offsetLE1.push_back(closestEmu);
					if(abs(minDistanceToCLCT) == 0) match_noOffset.push_back(closestEmu);

					if(clcts.pattern->at(iclct) == emulatedCLCTs.at(closestEmu)->patternId()){
						match_samePattern.push_back(closestEmu);
						if(clcts.quality->at(iclct) == emulatedCLCTs.at(closestEmu)->layerCount())
						{
							match_sameLayers.push_back(closestEmu);
							if(minDistanceToCLCT == 0) 
							{
								perfectMatches.push_back(closestEmu);
								if(threeLayerChamber && clcts.quality->at(iclct)==3)
								{
								threelayerperfectMatches.push_back(closestEmu);
								if(ST==1 && RI==1)
								threelayerperfectMatchesB.push_back(closestEmu);
								if(ST==1 && RI==4)
								threelayerperfectMatchesA.push_back(closestEmu);
								}
								perfMatch = true;
								if(clctsInChamber==1)
								{
								pmatch_clct0++;
								if(threeLayerChamber && clcts.quality->at(iclct)==3)
								{
								threelayerpmatch_clct0++;
								if(ST==1 && RI==1)
								threelayerpmatch_clct0B++;
								if(ST==1 && RI==4)
								threelayerpmatch_clct0A++;
								}
								pmatch_clct00[make_pair(ST,RI)]++;
								if(ST==1 && RI==4 && EC==1)
				                                pmatch_clct0_ME11A_EC[0]++;
				                                if(ST==1 && RI==4 && EC==2)
				                                pmatch_clct0_ME11A_EC[1]++;
				                                if(ST==1 && RI==4 && EC==1 && (CH%2)==0)
				                                pmatch_clct0_ME11A_EC_CH[make_pair(0,0)]++;
				                                if(ST==1 && RI==4 && EC==1 && (CH%2)==1)
				                                pmatch_clct0_ME11A_EC_CH[make_pair(0,1)]++;
				                                if(ST==1 && RI==4 && EC==2 && (CH%2)==0)
				                                pmatch_clct0_ME11A_EC_CH[make_pair(1,0)]++;
				                                if(ST==1 && RI==4 && EC==2 && (CH%2)==1)
				                                pmatch_clct0_ME11A_EC_CH[make_pair(1,1)]++;
								}
							}
						}

					}
							
				}
					
				if(!perfMatch){				    
					cout << "~~~ No Perfect Match Found ~~~ " << endl;
					//STRI[ST-1][RI-1]++;                                   
					compHits.print();
					cout << "Real CLCT: pat: " << (int)clcts.pattern->at(iclct) << " layers: "<< clcts.quality->at(iclct)<< " pos: "<< clctHSPos <<" [hs]"<< endl;
					if(closestEmu != -1) {
						cout << "Emulated: pat: " << emulatedCLCTs.at(closestEmu)->patternId() << " layers: " <<  emulatedCLCTs.at(closestEmu)->layerCount() <<
								" pos: " << emulatedCLCTs.at(closestEmu)->keyHalfStrip() << " [hs]" << endl;
						printPattern(emulatedCLCTs.at(closestEmu)->_pattern);
					}else{
						cout << "No matching emu" << endl;
					}
					cout << "Using emulated CLCT: " << closestEmu+1 << " / " << emulatedCLCTs.size() << endl << endl;

				}

				//cout << "valid chamber" << endl << endl;

				}

				else
				{
					negativeKHSCLCT++;
				}
				
			}

			if(!emulatedCLCTs.size() && !clctsInChamber) continue;


			//go through all the matches and check out their characteristics
			for(unsigned int imatch=0; imatch < matchedIndices.size(); imatch++){
				unsigned int id = matchedIndices.at(imatch);

				bool matchLE2 = find(match_offsetLE2.begin(), match_offsetLE2.end(), id) != match_offsetLE2.end();
				bool matchLE1 = find(match_offsetLE1.begin(), match_offsetLE1.end(), id) != match_offsetLE1.end();
				bool matchNoOffset = find(match_noOffset.begin(), match_noOffset.end(), id) != match_noOffset.end();
				bool matchSamePattern= find(match_samePattern.begin(), match_samePattern.end(), id) != match_samePattern.end();
				bool matchSameLayers=find(match_sameLayers.begin(), match_sameLayers.end(), id) != match_sameLayers.end();

				emulationPattVsOffset->Fill(anyOffset,anyPattern, 1);
				emulationPattVsOffset->Fill(anyOffset,samePattern, matchSamePattern);
				emulationPattVsOffset->Fill(anyOffset,sameLayers, matchSameLayers);
				emulationPattVsOffset->Fill(offset2,anyPattern, matchLE2);
				emulationPattVsOffset->Fill(offset2,samePattern, matchLE2*matchSamePattern);
				emulationPattVsOffset->Fill(offset2,sameLayers, matchLE2*matchSameLayers);
				emulationPattVsOffset->Fill(offset1,anyPattern, matchLE1);
				emulationPattVsOffset->Fill(offset1,samePattern, matchLE1*matchSamePattern);
				emulationPattVsOffset->Fill(offset1,sameLayers, matchLE1*matchSameLayers);
				emulationPattVsOffset->Fill(noOffset,anyPattern, matchNoOffset);
				emulationPattVsOffset->Fill(noOffset,samePattern, matchNoOffset*matchSamePattern);
				emulationPattVsOffset->Fill(noOffset,sameLayers, matchNoOffset*matchSameLayers);

			}

			emulationMatching->Fill(real, clctsInChamber);
			emulationMatching->Fill(emulated, emulatedCLCTs.size());
			emulationMatching->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatching->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatching->Fill(match, matchedIndices.size());
			emulationMatching->Fill(perfectMatch,perfectMatches.size());
			
			threelayer->Fill(real, threelayerclctsInChamber);
			threelayer->Fill(match, threelayermatchedIndices.size());
			threelayer->Fill(perfectMatch,threelayerperfectMatches.size());
			
			threelayerA->Fill(real, threelayerclctsInChamberA);
			threelayerA->Fill(match, threelayermatchedIndicesA.size());
			threelayerA->Fill(perfectMatch,threelayerperfectMatchesA.size());
			
			threelayerB->Fill(real, threelayerclctsInChamberB);
			threelayerB->Fill(match, threelayermatchedIndicesB.size());
			threelayerB->Fill(perfectMatch,threelayerperfectMatchesB.size());

			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(real, clctsInChamber);
			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(match, matchedIndices.size());
			emulationMatchingByChamber[make_pair(ST,RI)]->Fill(perfectMatch,perfectMatches.size());
			
			if(ST==1 && RI==4)
			{
			if(EC==1)
			{
			emulationMatchingByEC_ME11A[0]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A[0]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A[0]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A[0]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A[0]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A[0]->Fill(perfectMatch,perfectMatches.size());
			if((CH%2)==0)
			{
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[0]->Fill(perfectMatch,perfectMatches.size());
			}
			if((CH%2)==1)
			{
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[0]->Fill(perfectMatch,perfectMatches.size());
			}
			}
			if(EC==2)
			{
			emulationMatchingByEC_ME11A[1]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A[1]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A[1]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A[1]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A[1]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A[1]->Fill(perfectMatch,perfectMatches.size());
			if((CH%2)==0)
			{
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A_evenCH[1]->Fill(perfectMatch,perfectMatches.size());
			}
			if((CH%2)==1)
			{
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(real, clctsInChamber);
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(emulated, emulatedCLCTs.size());
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(noEmulated, clctsInChamber - matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(noReal, emulatedCLCTs.size() - matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(match, matchedIndices.size());
			emulationMatchingByEC_ME11A_oddCH[1]->Fill(perfectMatch,perfectMatches.size());
			}
			}
			}

			emulatedMultiplicity->Fill(emulatedCLCTs.size());
			realMultiplicity->Fill(clctsInChamber);
			

		}
		
	}

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}


	outF->cd();
	emulationMatching->Write();
	emulationPattVsOffset->Write();
	emulationStripDiff->Write();
	emulatedLayerCount->Write();
	realLayerCount->Write();
	emulatedMultiplicity->Write();
	realMultiplicity->Write();	
	outF->Close();
	



	unsigned int realCLCTs = emulationMatching->GetBinContent(1);
	unsigned int matches = emulationMatching->GetBinContent(5);
	unsigned int perfectMatches = emulationMatching->GetBinContent(6);

	//cout << endl;

	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matches << " / " << realCLCTs << " = " << 1.*matches/realCLCTs << endl;
	cout << "Perfect Matches: " << perfectMatches << " / " << realCLCTs << " = " << 1.*perfectMatches/realCLCTs << endl;

	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0 << " / " << clct0 << " = " << 1.*match_clct0/clct0 << endl;
	cout << "Perfect Matches: " << pmatch_clct0 << " / " << clct0 << " = " << 1.*pmatch_clct0/clct0 << endl<<endl;
	
	for(unsigned int itr=0; itr < NCHAMBERS; itr++)
	{
	unsigned int station = CHAMBER_ST_RI[itr][0];
	unsigned int ring = CHAMBER_ST_RI[itr][1];
	auto key = make_pair(station,ring);
	
	unsigned int realCLCTsByChamber = emulationMatchingByChamber[key]->GetBinContent(1);
	unsigned int matchesByChamber = emulationMatchingByChamber[key]->GetBinContent(5);
	unsigned int perfectMatchesByChamber = emulationMatchingByChamber[key]->GetBinContent(6);
	
	cout<<"Station: "<<station<<", Ring: "<<ring<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber << " / " << realCLCTsByChamber << " = " << 1.*matchesByChamber/realCLCTsByChamber << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber << " / " << realCLCTsByChamber << " = " << 1.*perfectMatchesByChamber/realCLCTsByChamber << endl;
	
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct00[key] << " / " << clct00[key] << " = " << 1.*match_clct00[key]/clct00[key] << endl;
	cout << "Perfect Matches: " << pmatch_clct00[key] << " / " << clct00[key] << " = " << 1.*pmatch_clct00[key]/clct00[key] << endl<<endl;
	
	if(station==1 && ring==4)
	{
	cout<<"ME11A broken up by EC, even/odd CH"<<endl;
	cout<<"--------------------------------------------------"<<endl;
	unsigned int realCLCTsByChamber_EC1 = emulationMatchingByEC_ME11A[0]->GetBinContent(1);
	unsigned int matchesByChamber_EC1 = emulationMatchingByEC_ME11A[0]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC1 = emulationMatchingByEC_ME11A[0]->GetBinContent(6);
	cout<<"Endcap = 1"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC1 << " / " << realCLCTsByChamber_EC1 << " = " << 1.*matchesByChamber_EC1/realCLCTsByChamber_EC1 << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC1 << " / " << realCLCTsByChamber_EC1 << " = " << 1.*perfectMatchesByChamber_EC1/realCLCTsByChamber_EC1 << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC[0] << " / " << clct0_ME11A_EC[0] << " = " << 1.*match_clct0_ME11A_EC[0]/clct0_ME11A_EC[0] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC[0] << " / " << clct0_ME11A_EC[0] << " = " << 1.*pmatch_clct0_ME11A_EC[0]/clct0_ME11A_EC[0] << endl;
	cout<<"----------"<<endl;
	unsigned int realCLCTsByChamber_EC1_evenCH = emulationMatchingByEC_ME11A_evenCH[0]->GetBinContent(1);
	unsigned int matchesByChamber_EC1_evenCH = emulationMatchingByEC_ME11A_evenCH[0]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC1_evenCH = emulationMatchingByEC_ME11A_evenCH[0]->GetBinContent(6);	
	cout<<"Endcap = 1, Even Chambers"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC1_evenCH << " / " << realCLCTsByChamber_EC1_evenCH << " = " << 1.*matchesByChamber_EC1_evenCH/realCLCTsByChamber_EC1_evenCH << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC1_evenCH << " / " << realCLCTsByChamber_EC1_evenCH << " = " << 1.*perfectMatchesByChamber_EC1_evenCH/realCLCTsByChamber_EC1_evenCH << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC_CH[make_pair(0,0)] << " / " << clct0_ME11A_EC_CH[make_pair(0,0)] << " = " << 1.*match_clct0_ME11A_EC_CH[make_pair(0,0)]/clct0_ME11A_EC_CH[make_pair(0,0)] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC_CH[make_pair(0,0)] << " / " << clct0_ME11A_EC_CH[make_pair(0,0)] << " = " << 1.*pmatch_clct0_ME11A_EC_CH[make_pair(0,0)]/clct0_ME11A_EC_CH[make_pair(0,0)] << endl;
	cout<<"----------"<<endl;
	unsigned int realCLCTsByChamber_EC1_oddCH = emulationMatchingByEC_ME11A_oddCH[0]->GetBinContent(1);
	unsigned int matchesByChamber_EC1_oddCH = emulationMatchingByEC_ME11A_oddCH[0]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC1_oddCH = emulationMatchingByEC_ME11A_oddCH[0]->GetBinContent(6);	
	cout<<"Endcap = 1, Odd Chambers"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC1_oddCH << " / " << realCLCTsByChamber_EC1_oddCH << " = " << 1.*matchesByChamber_EC1_oddCH/realCLCTsByChamber_EC1_oddCH << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC1_oddCH << " / " << realCLCTsByChamber_EC1_oddCH << " = " << 1.*perfectMatchesByChamber_EC1_oddCH/realCLCTsByChamber_EC1_oddCH << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC_CH[make_pair(0,1)] << " / " << clct0_ME11A_EC_CH[make_pair(0,1)] << " = " << 1.*match_clct0_ME11A_EC_CH[make_pair(0,1)]/clct0_ME11A_EC_CH[make_pair(0,1)] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC_CH[make_pair(0,1)] << " / " << clct0_ME11A_EC_CH[make_pair(0,1)] << " = " << 1.*pmatch_clct0_ME11A_EC_CH[make_pair(0,1)]/clct0_ME11A_EC_CH[make_pair(0,1)] << endl;
	cout<<"----------"<<endl;
	unsigned int realCLCTsByChamber_EC2 = emulationMatchingByEC_ME11A[1]->GetBinContent(1);
	unsigned int matchesByChamber_EC2 = emulationMatchingByEC_ME11A[1]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC2 = emulationMatchingByEC_ME11A[1]->GetBinContent(6);
	cout<<"Endcap = 2"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC2 << " / " << realCLCTsByChamber_EC2 << " = " << 1.*matchesByChamber_EC2/realCLCTsByChamber_EC2 << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC2 << " / " << realCLCTsByChamber_EC2 << " = " << 1.*perfectMatchesByChamber_EC2/realCLCTsByChamber_EC2 << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC[1] << " / " << clct0_ME11A_EC[1] << " = " << 1.*match_clct0_ME11A_EC[1]/clct0_ME11A_EC[1] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC[1] << " / " << clct0_ME11A_EC[1] << " = " << 1.*pmatch_clct0_ME11A_EC[1]/clct0_ME11A_EC[1] << endl;
	cout<<"----------"<<endl;
	unsigned int realCLCTsByChamber_EC2_evenCH = emulationMatchingByEC_ME11A_evenCH[1]->GetBinContent(1);
	unsigned int matchesByChamber_EC2_evenCH = emulationMatchingByEC_ME11A_evenCH[1]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC2_evenCH = emulationMatchingByEC_ME11A_evenCH[1]->GetBinContent(6);
	cout<<"Endcap = 2, Even Chambers"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC2_evenCH << " / " << realCLCTsByChamber_EC2_evenCH << " = " << 1.*matchesByChamber_EC2_evenCH/realCLCTsByChamber_EC2_evenCH << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC2_evenCH << " / " << realCLCTsByChamber_EC2_evenCH << " = " << 1.*perfectMatchesByChamber_EC2_evenCH/realCLCTsByChamber_EC2_evenCH << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC_CH[make_pair(1,0)] << " / " << clct0_ME11A_EC_CH[make_pair(1,0)] << " = " << 1.*match_clct0_ME11A_EC_CH[make_pair(1,0)]/clct0_ME11A_EC_CH[make_pair(1,0)] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC_CH[make_pair(1,0)] << " / " << clct0_ME11A_EC_CH[make_pair(1,0)] << " = " << 1.*pmatch_clct0_ME11A_EC_CH[make_pair(1,0)]/clct0_ME11A_EC_CH[make_pair(1,0)] << endl;
	cout<<"----------"<<endl;
	unsigned int realCLCTsByChamber_EC2_oddCH = emulationMatchingByEC_ME11A_oddCH[1]->GetBinContent(1);
	unsigned int matchesByChamber_EC2_oddCH = emulationMatchingByEC_ME11A_oddCH[1]->GetBinContent(5);
	unsigned int perfectMatchesByChamber_EC2_oddCH = emulationMatchingByEC_ME11A_oddCH[1]->GetBinContent(6);	
	cout<<"Endcap = 2, Odd Chambers"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matchesByChamber_EC2_oddCH << " / " << realCLCTsByChamber_EC2_oddCH << " = " << 1.*matchesByChamber_EC2_oddCH/realCLCTsByChamber_EC2_oddCH << endl;
	cout << "Perfect Matches: " << perfectMatchesByChamber_EC2_oddCH << " / " << realCLCTsByChamber_EC2_oddCH << " = " << 1.*perfectMatchesByChamber_EC2_oddCH/realCLCTsByChamber_EC2_oddCH << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0_ME11A_EC_CH[make_pair(1,1)] << " / " << clct0_ME11A_EC_CH[make_pair(1,1)] << " = " << 1.*match_clct0_ME11A_EC_CH[make_pair(1,1)]/clct0_ME11A_EC_CH[make_pair(1,1)] << endl;
	cout << "Perfect Matches: " << pmatch_clct0_ME11A_EC_CH[make_pair(1,1)] << " / " << clct0_ME11A_EC_CH[make_pair(1,1)] << " = " << 1.*pmatch_clct0_ME11A_EC_CH[make_pair(1,1)]/clct0_ME11A_EC_CH[make_pair(1,1)] << endl;
	cout<<"--------------------------------------------------"<<endl<<endl;
	}	
	}	
	
	unsigned int threelayerrealCLCTs = threelayer->GetBinContent(1);
	unsigned int threelayermatches = threelayer->GetBinContent(5);
	unsigned int threelayerperfectMatches = threelayer->GetBinContent(6);
	
	unsigned int threelayerrealCLCTsA = threelayerA->GetBinContent(1);
	unsigned int threelayermatchesA = threelayerA->GetBinContent(5);
	unsigned int threelayerperfectMatchesA = threelayerA->GetBinContent(6);
	
	unsigned int threelayerrealCLCTsB = threelayerB->GetBinContent(1);
	unsigned int threelayermatchesB = threelayerB->GetBinContent(5);
	unsigned int threelayerperfectMatchesB = threelayerB->GetBinContent(6);
	
	cout<<"Three Layer Chambers"<<endl;	
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << threelayermatches << " / " << threelayerrealCLCTs << " = " << 1.*threelayermatches/threelayerrealCLCTs << endl;
	cout << "Perfect Matches: " << threelayerperfectMatches << " / " << threelayerrealCLCTs << " = " << 1.*threelayerperfectMatches/threelayerrealCLCTs << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << threelayermatch_clct0 << " / " << threelayerclct0 << " = " << 1.*threelayermatch_clct0/threelayerclct0 << endl;
	cout << "Perfect Matches: " << threelayerpmatch_clct0 << " / " << threelayerclct0 << " = " << 1.*threelayerpmatch_clct0/threelayerclct0 << endl<<endl;
	cout<<"--------------------------------------------------"<<endl;
	cout<<"ME11A:"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << threelayermatchesA << " / " << threelayerrealCLCTsA << " = " << 1.*threelayermatchesA/threelayerrealCLCTsA << endl;
	cout << "Perfect Matches: " << threelayerperfectMatchesA << " / " << threelayerrealCLCTsA << " = " << 1.*threelayerperfectMatchesA/threelayerrealCLCTsA << endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << threelayermatch_clct0A << " / " << threelayerclct0A << " = " << 1.*threelayermatch_clct0A/threelayerclct0A << endl;
	cout << "Perfect Matches: " << threelayerpmatch_clct0A << " / " << threelayerclct0A << " = " << 1.*threelayerpmatch_clct0A/threelayerclct0A << endl<<endl;
	cout<<"ME11B:"<<endl;
	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << threelayermatchesB<< " / " << threelayerrealCLCTsB<< " = " << 1.*threelayermatchesB/threelayerrealCLCTsB<< endl;
	cout << "Perfect Matches: " << threelayerperfectMatchesB<< " / " << threelayerrealCLCTsB<< " = " << 1.*threelayerperfectMatchesB/threelayerrealCLCTsB<< endl;
	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << threelayermatch_clct0B << " / " << threelayerclct0B << " = " << 1.*threelayermatch_clct0B/threelayerclct0B << endl;
	cout << "Perfect Matches: " << threelayerpmatch_clct0B << " / " << threelayerclct0B << " = " << 1.*threelayerpmatch_clct0B/threelayerclct0B << endl<<endl;
	cout<<"--------------------------------------------------" << endl << endl;

	cout << "Number of events that a CLCT with negative KHS was found = " << negativeKHSCLCT << endl << endl; 	

	cout << "Wrote to file: " << outputfile << endl;	
	
	/*TCanvas *c1 = new TCanvas("c1","c1"); //for visibility
	minDistanceToCLCTByChamber[make_pair(1,1)]->Draw();
	TCanvas *c2 = new TCanvas("c2","c2");
	minDistanceToCLCTByChamber[make_pair(1,4)]->Draw();
	TCanvas *c3 = new TCanvas("c3","c3");
	minDistanceToCLCTByChamber[make_pair(1,2)]->Draw();
	TCanvas *c4 = new TCanvas("c4","c4");
	minDistanceToCLCTByChamber[make_pair(1,3)]->Draw();
	TCanvas *c5 = new TCanvas("c5","c5");
	minDistanceToCLCTByChamber[make_pair(2,1)]->Draw();
	TCanvas *c6 = new TCanvas("c6","c6");
	minDistanceToCLCTByChamber[make_pair(2,2)]->Draw();
	TCanvas *c7 = new TCanvas("c7","c7");
	minDistanceToCLCTByChamber[make_pair(3,1)]->Draw();
	TCanvas *c8 = new TCanvas("c8","c8");
	minDistanceToCLCTByChamber[make_pair(3,2)]->Draw();
	TCanvas *c9 = new TCanvas("c9","c9");
	minDistanceToCLCTByChamber[make_pair(4,1)]->Draw();
	TCanvas *c10 = new TCanvas("c10","c10");
	minDistanceToCLCTByChamber[make_pair(4,2)]->Draw();*/
	
	/*TCanvas *c1 = new TCanvas("c1","c1"); //more concise
	c1->Divide(2,5);
	for(unsigned int k = 0; k<NCHAMBERS; k++)
	{
	unsigned int st = CHAMBER_ST_RI[k][0];
	unsigned int ri = CHAMBER_ST_RI[k][1];
	auto KEY = make_pair(st,ri);
	unsigned int s = k+1;
	c1->cd(s);
	minDistanceToCLCTByChamber[KEY]->Draw();
	}*/
	
	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;

}





