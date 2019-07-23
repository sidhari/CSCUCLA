/*
 * ThreeLayerCLCTEmulationAnalyzer.cpp
 *
 *  Created on: May 1, 2019
 *      Author: wnash
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

#include "../include/ThreeLayerCLCTEmulationAnalyzer.h"

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


	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();


	cout << "Starting Event: " << start << " Ending Event: " << end << endl;

	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		// First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		// installed on September 12

		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		// Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		//if(evt.RunNumber <= 323362) continue;


		//TODO: fill this in!
	}




	outF->Close();

	cout << "Wrote to file: " << outputfile << endl;

	return 0;
}
