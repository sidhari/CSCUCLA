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

#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"


int ThreeLayerCLCTEmulationAnalyzer(string inputfile, string outputfile, int start=0, int end=-1) {

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



	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}


	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();


	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	cout << "Starting Event: " << start << " Ending Event: " << end << endl;

	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		/* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;


		//TODO: fill this in!
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
			return ThreeLayerCLCTEmulationAnalyzer(string(argv[1]), string(argv[2]));
		case 4:
			return ThreeLayerCLCTEmulationAnalyzer(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return ThreeLayerCLCTEmulationAnalyzer(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./ThreeLayerCLCTEmulationAnalyzer inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}
