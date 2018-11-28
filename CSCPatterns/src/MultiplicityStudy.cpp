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


/* Calculates probability of a muon given a comparator code.
 * See slides: https://indico.cern.ch/event/744948/
 */

int MultiplicityStudy(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


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


	vector<CSCPattern>* newPatterns = createNewPatterns();

	//
	// Map to look at probability
	// TODO: incorporate this functionality into LUT class once it is more figured out
	//
	//LUT bayesLUT("bayes", LINEFIT_LUT_PATH);

	//
	// OUTPUT TREE
	//

	//x = ring, y = stat
	//TH2F* multiplicityByChamber = new TH2F("multByCham", "mumltByCham; Ring; Station", 4, 1,5, 3, 1,4);

	vector<TH1F*> multiplicities;
	for(auto& name : CHAMBER_NAMES){
		TH1F* cham = new TH1F(name.c_str(), (name+"; CLCT Multiplicity; CLCTs").c_str(), 10, 1,11);
		multiplicities.push_back(cham);
	}

	//int patternId = 0;
	//int ccId = 0;
	//int layers = 0;
	//float closestCLCT
	//bool matchedSegment = false;
	//int nMultiplicity = 0;

	/*
	TTree* outTree = new TTree("clctTree", "TTree for analysis of probability if a CLCT will result in a real muon or not");
	outTree->Branch("patternId", &patternId, "patternId/I");
	outTree->Branch("ccId", &ccId, "ccId/I");
	outTree->Branch("matchedSegment", &matchedSegment);
	outTree->Branch("nMultiplicity", &nMultiplicity, "nMultiplicity/I");
	*/



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	//TH1F* h_clctPatterns_real = new TH1F("h_clctPatterns_real","Recorded CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);
	//TH1F* h_clctPatterns_emulated = new TH1F("h_clctPatterns_emulated","Emulated CLCT Pattern IDs; Pattern ID; CLCTs", 11,0,11);

	//
	// TREE ITERATION
	//


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%100)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

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

			//
			// Emulate the TMB to find all the CLCTs
			//

			ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> eclcts;


			//get all the clcts in the chamber
			if(searchForMatch(compHits, newPatterns,eclcts)){
				eclcts.clear();
				continue;
			}

			if(!eclcts.size()) continue;

			for(unsigned int c =0; c < NCHAMBERS; c++){
				auto& st_ri = CHAMBER_ST_RI[c];
				//this is the chamber we are in right now
				if(ST == st_ri[0] && RI == st_ri[1]){
					multiplicities.at(c)->Fill(eclcts.size());
					break;
				}

			}

			eclcts.clear();
		}


	}
	outF->cd();
	for (auto& hist : multiplicities) {
		double norm = hist->GetEntries();
		if(norm) hist->Scale(1./norm);
		hist->Write();
	}
	outF->Close();


	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;

}


int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return MultiplicityStudy(string(argv[1]), string(argv[2]));
		case 4:
			return MultiplicityStudy(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return MultiplicityStudy(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./PatternFinder inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}







