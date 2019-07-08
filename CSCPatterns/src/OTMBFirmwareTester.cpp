/*
 * OTMBFirmwareTeter.cpp
 *
 *  Created on: June 27, 2019
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

#include "../include/OTMBFirmwareTester.h"

int main(int argc, char* argv[]){
	OTMBFirmwareTester p;
	return p.main(argc,argv);
}


int OTMBFirmwareTester::run(string inputfile, string outputfile, int start, int end) {

	cout << "Not using file: " << inputfile << endl;

	vector<CSCPattern>* newPatterns = createNewPatterns();

	ChamberHits compHits;


	CSCInfo::Comparators comps;
	comps.ch_id = new std::vector<int>();
	comps.lay = new std::vector<size8>();
	comps.strip = new std::vector<size8>();
	comps.halfStrip = new std::vector<size8>();
	comps.bestTime = new std::vector<size8>();
	comps.nTimeOn = new std::vector<size8>();

	//comparator output file
	std::ofstream compFile;
	compFile.open("comparators.txt");
	//clct output file
	std::ofstream clctFile;
	clctFile.open("clcts.txt");
	/*
A single line would be, for example,

000000001111111122222222333333334444444455555555

Where 00000000 are the 32 halfstrips in ly0, 11111111 are the 32 halfstrips in ly1, etc...

So for example:

    000000010000000100000001000000010000000100000001

corresponds to a 6 layer straight pattern on hs0.

This array stores the correct number for each layer
	 */
	int comparatorLocationNumberEncoding[NLAYERS][CFEB_HS/4];

	for(unsigned int ihs=0; ihs < CFEB_HS; ihs++){
		//clear it
		for(unsigned int i=0; i < NLAYERS;i++){
			for(unsigned int j=0; j < CFEB_HS/4; j++){
				comparatorLocationNumberEncoding[i][j] = 0;
			}
		}
		ChamberHits compHits;
		for(unsigned int ilay=0; ilay < NLAYERS; ilay++){

			comparatorLocationNumberEncoding[ilay][(CFEB_HS-(ihs+1))/4] = pow(2,ihs%4);

			comps.ch_id->push_back(CSCHelper::serialize(0,0,0,0));
			comps.lay->push_back(ilay+1);
			comps.strip->push_back((ihs+2)/2);
			//comps.strip->push_back(ihs+1);
			comps.halfStrip->push_back(ihs%2);
			comps.bestTime->push_back(7);
			comps.nTimeOn->push_back(1);

		}
		compHits.fill(comps);
		compHits.print();

		vector<CLCTCandidate*> emulatedCLCTs;

		if(searchForMatch(compHits,newPatterns, emulatedCLCTs,true)){
			emulatedCLCTs.clear();
			//cout << "Something broke" << endl;
			//return;

			return -1;
		}

		std::cout << "comparators:" << endl;

		std::cout << noshowbase;
		//std::cout << showbase << internal << setfill('0');
		for(unsigned int i=0; i < NLAYERS;i++){
			for(unsigned int j=0; j < CFEB_HS/4; j++){
				std::cout << dec <<comparatorLocationNumberEncoding[i][j];
				compFile <<dec << comparatorLocationNumberEncoding[i][j];
			}
		}
		std::cout <<std::endl;
		compFile <<std::endl;


		std::cout << "khs, patt, ccode" << std::endl;
		std::cout << internal << setfill('0');
		for(auto& clct: emulatedCLCTs){
			std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
			std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
			std::cout << setw(4) << clct->patternId() << endl;
			std::cout << setw(4) << clct->comparatorCodeId() << endl;
			clctFile << internal << setfill('0')  <<hex << setw(4) << clct->keyHalfStrip() << setw(4) << clct->patternId() << setw(4) << clct->comparatorCodeId() << endl;
			//clct->print3x6Pattern();
			//printf("hs: %hhx, patt: %hhx, cc: %hhx\n",clct->keyHalfStrip(), clct->patternId(), clct->comparatorCodeId());
		}



		comps.ch_id->clear();
		comps.lay->clear();
		comps.strip->clear();
		comps.halfStrip->clear();
		comps.bestTime->clear();
		comps.nTimeOn->clear();
	}







	delete comps.ch_id;
	delete comps.lay;
	delete comps.strip;
	delete comps.halfStrip;
	delete comps.bestTime;
	delete comps.nTimeOn;


	return 0;
}
