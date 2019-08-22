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

	//are we running over real data or fake stuff for testing?
	const bool fakeData = false;



	//initialize output files
	std::ofstream CFEBFiles[MAX_CFEBS];
	for(unsigned int i=0; i < MAX_CFEBS; i++){
		CFEBFiles[i].open("CFEB"+to_string(i)+".mem");
	}

	const unsigned int nCLCTs = 2;
	ofstream CLCTFiles[nCLCTs]; //2 clcts
	for(unsigned int i=0; i < nCLCTs; i++){
		CLCTFiles[i].open("expected"+to_string(i)+".mem");
	}

	vector<CSCPattern>* newPatterns = createNewPatterns();

	//TODO: break up into seperate functions
	if(fakeData){
		CSCInfo::Comparators comps;
		comps.ch_id = new std::vector<int>();
		comps.lay = new std::vector<size8>();
		comps.strip = new std::vector<size8>();
		comps.halfStrip = new std::vector<size8>();
		comps.bestTime = new std::vector<size8>();
		comps.nTimeOn = new std::vector<size8>();
		cout << "Not using file: " << inputfile << endl;
		for(unsigned int ihs=0; ihs < 4*CFEB_HS; ihs++){

			ChamberHits compHits(1,1,1,1); //fake me11b chamber
			for(unsigned int ilay=0; ilay < NLAYERS; ilay++){

				comps.ch_id->push_back(CSCHelper::serialize(1,1,1,1));
				comps.lay->push_back(ilay+1);
				comps.strip->push_back((ihs+2)/2);
				//comps.strip->push_back(ihs+1);
				comps.halfStrip->push_back(ihs%2);
				comps.bestTime->push_back(7);
				comps.nTimeOn->push_back(1);

			}
			compHits.fill(comps);
			compHits.print();

			writeToMEMFiles(compHits,CFEBFiles);


			vector<CLCTCandidate*> emulatedCLCTs;

			if(searchForMatch(compHits,newPatterns, emulatedCLCTs,true)){
				emulatedCLCTs.clear();
				return -1;
			}
			//keep only first two
			while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();



			std::cout << "khs, patt, ccode" << std::endl;
			std::cout << internal << setfill('0');
			for(unsigned int i=0; i < nCLCTs; i++){
				for(unsigned int ib=0; ib < 7; ib++){ //put in blank lines
					CLCTFiles[i] << "000000000000" << endl;
				}
				if(i < emulatedCLCTs.size()){

					auto& clct = emulatedCLCTs.at(i);
					std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
					std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
					std::cout << setw(4) << clct->patternId() << endl;
					std::cout << setw(4) << clct->comparatorCodeId() << endl;
					CLCTFiles[i] << internal << setfill('0')  <<hex << setw(4) << clct->keyHalfStrip() << setw(4) << clct->patternId() << setw(4) << clct->comparatorCodeId() << endl;
				}else {
					CLCTFiles[i] << "000000000000" << endl;
				}
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
	}else{
		cout << "Running over file: " << inputfile << endl;


		TFile* f = TFile::Open(inputfile.c_str());
		if(!f) throw "Can't open file";

		TTree* t =  (TTree*)f->Get("CSCDigiTree");
		if(!t) throw "Can't find tree";

		//
		// SET INPUT BRANCHES
		//

		CSCInfo::Event evt(t);
		//CSCInfo::Muons muons(t);
		//CSCInfo::Segments segments(t);
		//CSCInfo::RecHits recHits(t);
		//CSCInfo::LCTs lcts(t);
		//CSCInfo::CLCTs clcts(t);
		CSCInfo::Comparators comparators(t);
		if(end > t->GetEntries() || end < 0) end = t->GetEntries();


		cout << "Starting Event: " << start << " Ending Event: " << end << endl;


		for(int i = start; i < end; i++) {
			if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

			t->GetEntry(i);
			cout << evt.EventNumber << endl;

			for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++){
				CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

				unsigned int EC = c.endcap;
				unsigned int ST = c.station;
				unsigned int RI = c.ring;
				unsigned int CH = c.chamber;

				if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;
				//bool me11a = (ST == 1 && RI == 4);
				bool me11b = (ST == 1 && RI == 1);
				//only look at ME11B chambers for now
				if(!me11b) continue;

				ChamberHits compHits(ST,RI,EC,CH);
				if(compHits.fill(comparators)) return -1;
				if(!compHits.nhits()) continue; //skip if its empty
				compHits.print();

				writeToMEMFiles(compHits, CFEBFiles);

				vector<CLCTCandidate*> emulatedCLCTs;

				if(searchForMatch(compHits,newPatterns, emulatedCLCTs,true)){
					emulatedCLCTs.clear();

					continue;
				}
				while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();

				std::cout << "khs, patt, ccode" << std::endl;
				std::cout << internal << setfill('0');
				for(unsigned int i=0; i < nCLCTs; i++){
					for(unsigned int ib=0; ib < 7; ib++){ //put in blank lines
						CLCTFiles[i] << "000000000000" << endl;
					}
					if(i < emulatedCLCTs.size()){

						auto& clct = emulatedCLCTs.at(i);
						std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
						std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
						std::cout << setw(4) << clct->patternId() << endl;
						std::cout << setw(4) << clct->comparatorCodeId() << endl;
						CLCTFiles[i] << internal << setfill('0')  <<hex << setw(4) << clct->keyHalfStrip() << setw(4) << clct->patternId() << setw(4) << clct->comparatorCodeId() << endl;
					}else {
						CLCTFiles[i] << "000000000000" << endl;
					}
				}

			}
		}

	}

	for(auto& cfebFile : CFEBFiles) cfebFile.close();
	for(auto& clctFile : CLCTFiles) clctFile.close();

	return 0;
}
