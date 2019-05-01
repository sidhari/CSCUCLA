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

#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;




int testTMBEmulation(string inputfile, string outputfile, int start=0, int end=-1) {

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
	emulationMatching->GetXaxis()->SetBinLabel(emulated+1, "emulated");
	emulationMatching->GetXaxis()->SetBinLabel(noEmulated+1, "noEmulated");
	emulationMatching->GetXaxis()->SetBinLabel(noReal+1, "noReal");
	emulationMatching->GetXaxis()->SetBinLabel(match+1, "match");
	emulationMatching->GetXaxis()->SetBinLabel(perfectMatch+1, "perfectMatch");

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

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		/*
		if(evt.EventNumber != 648972225
				&& evt.EventNumber != 640297869
				&& evt.EventNumber != 650469080
				) continue;
		*/
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

			if(searchForMatch(compHits,oldPatterns, emulatedCLCTs,true)){
				emulatedCLCTs.clear();
				//cout << "Something broke" << endl;
				//return;

				continue;
			}


			//remove 3 layer emulated clcts from chambers that don't
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

			//among the matches
			vector<unsigned int> match_offsetLE2; //less than or equal to 2 [hs]
			vector<unsigned int> match_offsetLE1;
			vector<unsigned int> match_noOffset;

			vector<unsigned int> match_samePattern;
			vector<unsigned int> match_sameLayers;

			vector<unsigned int> perfectMatches;


			unsigned int clctsInChamber = 0;

			//
			// Iterate over real clcts
			//
			for(unsigned int iclct=0; iclct < clcts.size(); iclct++){
				int clctHash = clcts.ch_id->at(iclct);
				if(clctHash != chamberHash) continue;
				clctsInChamber++;
				if(clctsInChamber == 1) clct0++; //hope that the first one is ordered correctly...
				realLayerCount->Fill(clcts.quality->at(iclct));

				float clctHSPos = clcts.keyStrip->at(iclct); //key strip is in units of half strips...
				if(me11a) clctHSPos -= 32*4;

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
					emulationStripDiff->Fill(minDistanceToCLCT);
					if(clctsInChamber == 1) match_clct0++;

					if(abs(minDistanceToCLCT) <= 2) match_offsetLE2.push_back(closestEmu);
					if(abs(minDistanceToCLCT) <= 1) match_offsetLE1.push_back(closestEmu);
					if(abs(minDistanceToCLCT) == 0) match_noOffset.push_back(closestEmu);

					if(clcts.pattern->at(iclct) == emulatedCLCTs.at(closestEmu)->patternId()){
						match_samePattern.push_back(closestEmu);
						if(clcts.quality->at(iclct) == emulatedCLCTs.at(closestEmu)->layerCount()){
							match_sameLayers.push_back(closestEmu);
							if(minDistanceToCLCT == 0) {
								perfectMatches.push_back(closestEmu);
								perfMatch = true;
								if(clctsInChamber==1) pmatch_clct0++;
							}
						}

					}
				}
				if(!perfMatch){
					cout << "~~~ No Perfect Match Found ~~~ " << endl;
					compHits.print();
					cout << "Real CLCT: pat: " << (int)clcts.pattern->at(iclct) << " layers: "<< clcts.quality->at(iclct)<< " pos: "<< clctHSPos <<" [hs]"<< endl;
					if(closestEmu != -1) {
						cout << "Emulated: pat: " << emulatedCLCTs.at(closestEmu)->patternId() << " layers: " <<  emulatedCLCTs.at(closestEmu)->layerCount() <<
								" pos: " << emulatedCLCTs.at(closestEmu)->keyHalfStrip() << " [hs]" << endl;
						printPattern(emulatedCLCTs.at(closestEmu)->_pattern);
					}else{
						cout << "No matching emu" << endl;
					}
					cout << "Using emulated CLCT: " << closestEmu+1 << " / " << emulatedCLCTs.size() << endl;

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

			emulatedMultiplicity->Fill(emulatedCLCTs.size());
			realMultiplicity->Fill(clctsInChamber);

		}
	}

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
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

	cout << "-- All CLCTs --" << endl;
	cout << "        Matches: " << matches << " / " << realCLCTs << " = " << 1.*matches/realCLCTs << endl;
	cout << "Perfect Matches: " << perfectMatches << " / " << realCLCTs << " = " << 1.*perfectMatches/realCLCTs << endl;

	cout << "-- First CLCT --" << endl;
	cout << "        Matches: " << match_clct0 << " / " << clct0 << " = " << 1.*match_clct0/clct0 << endl;
	cout << "Perfect Matches: " << pmatch_clct0 << " / " << clct0 << " = " << 1.*pmatch_clct0/clct0 << endl;


	cout << "Wrote to file: " << outputfile << endl;

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;

}


int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return testTMBEmulation(string(argv[1]), string(argv[2]));
		case 4:
			return testTMBEmulation(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return testTMBEmulation(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./<name> inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}







