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

//TODO: add to LUTEntry class? also need to implement for slope, etc
// compresses a signed offset to nthStrips
unsigned int compressToNStripBits(float offsetStrips, unsigned int nBits=5){
	bool debug = false;
	/* From linear fits estimation of span of offsets with layers >= 3
	 *
	 * Units in strips
		minOffset = -2.41667
		Pattern: 90
		Code: 3392
		--___------
		---___-----
		----___----
		----X__----
		-----X__---
		------__X--
		0123456789a

		maxOffset = 0.916667
		Pattern: 80
		Code: 1984
		------___--
		-----___---
		----___----
		----__X----
		---__X-----
		--X__------
		0123456789a

		TODO:verify

	 *
	 * minOffset = -2.41667
     * maxOffset = 0.916667
	 * -> 2.41 + 0.91 = 3.32 strips span
	 *
	 * call this 4 strips
	 *
	 * for 8th strip resolution offsets over 4 strips, we need
	 *  4*8 = 32 possibilities = 2^5 -> 5 bits
	 */

	if(offsetStrips >=2) offsetStrips = 1.999;
	if(offsetStrips < -2) offsetStrips = -2;

	if(debug) cout << "offsetStrips: " << offsetStrips << endl;

	//have range of 4, and have n total bits, so have
	// 2^n/2^2 = 2^(n-2) sections per strip
	unsigned int nStripSections = (unsigned int)pow(2,nBits-2);

	//put in 8th strips, and center
	unsigned int offsetNthStrips = round(nStripSections*(offsetStrips+2));
	if(debug) cout << "offsetNthStrips: " << offsetNthStrips << endl;

	return offsetNthStrips;
}

unsigned int compressToNSlopeBits(float slope, unsigned int nBits=5){
	bool debug = true;


	//max slope taken as +/- 1 strip per layer (see EMTF Meeting slides Dec, 13, 2018)
	if(slope >= 1) slope = 0.999;
	if(slope < -1) slope = -1;

	//have range of 2 strips/layer, have n total bits, so have
	// 2^n/2 = 2^(n-1) sections per strips/layer
	unsigned int nStripsPerLayerSections = (unsigned int)pow(2,nBits-1);

	if(debug) cout << "slopeOffset: " << slope << endl;
	unsigned int slopeNthStripsPerLayer = round(nStripsPerLayerSections*(slope+1));
	if(debug) cout << "slopeNthStripsPerLayer: " << slopeNthStripsPerLayer<<endl;

	return slopeNthStripsPerLayer;
}


int main(int argc, char* argv[]){
	OTMBFirmwareTester p;
	return p.main(argc,argv);
}


int OTMBFirmwareTester::run(string inputfile, string outputfile, int start, int end) {

	//are we running over real data or fake stuff for testing?
	const bool fakeData = true;


	//load demo lookup table
	LUT lut("linearFits","dat/linearFits.lut");
	lut.makeFinal();



	//initialize output files
	std::ofstream CFEBFiles[MAX_CFEBS];
	for(unsigned int i=0; i < MAX_CFEBS; i++){
		//CFEBFiles[i].open("CFEB"+to_string(i)+"-fake.mem"); //TODO smart naming
		CFEBFiles[i].open("cfeb"+to_string(i)+".mem");
	}

	const unsigned int nCLCTs = 2;
	ofstream CLCTFiles[nCLCTs]; //2 clcts
	CLCTFiles[0].open("expected_1st.mem");
	CLCTFiles[1].open("expected_2nd.mem");

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

		//
		// Iterate over a set number of fake chambers to write to
		//
		unsigned int fakeChambers = 100;
		for(unsigned int ie=0; ie < fakeChambers;ie++){

			comps.ch_id->clear();
			comps.lay->clear();
			comps.strip->clear();
			comps.halfStrip->clear();
			comps.bestTime->clear();
			comps.nTimeOn->clear();

			ChamberHits compHits(1,1,1,1); //fake me11b chamber
			unsigned int nChamberHalfStrips = compHits.maxHs() - compHits.minHs();

			//
			// Create some arrangement of the comarators for this fake chamber
			//
			for(unsigned int ilay=0; ilay < NLAYERS; ilay++){

				//have loop for each individual clct we are simulating
				unsigned int nFakeClcts = 10;
				for(unsigned int iclct=0; iclct < nFakeClcts; iclct++){

					comps.ch_id->push_back(CSCHelper::serialize(1,1,1,1));
					comps.lay->push_back(ilay+1);
					comps.bestTime->push_back(7);
					comps.nTimeOn->push_back(1);

					unsigned int halfStrip = iclct %2 ?
							(ie+ilay+iclct*nChamberHalfStrips/nFakeClcts)%nChamberHalfStrips
							:(iclct*nChamberHalfStrips/nFakeClcts- ie-ilay)%nChamberHalfStrips;


					comps.strip->push_back((halfStrip+2)/2);
					comps.halfStrip->push_back(halfStrip%2);

				}
			}
			compHits.fill(comps);
			compHits.print();

			writeToMEMFiles(compHits,CFEBFiles);


			vector<CLCTCandidate*> emulatedCLCTs;

			if(searchForMatch(compHits,newPatterns, emulatedCLCTs,true)){
				emulatedCLCTs.clear();
				continue;
			}
			//keep only first two
			while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();



			std::cout << "khs, patt, ccode" << std::endl;
			std::cout << internal << setfill('0');
			for(unsigned int i=0; i < nCLCTs; i++){
				for(unsigned int ib=0; ib < 7; ib++){ //put in blank lines
					CLCTFiles[i] << "0000000000000000" << endl;
				}
				if(i < emulatedCLCTs.size()){

					auto& clct = emulatedCLCTs.at(i);

					//get LUT value
					const LUTEntry* entry =0;
					if(lut.getEntry(clct->key(),entry)){
						cout << "Error: No LUT Entry"<< endl;
						return -1;
					}


					std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
					std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
					std::cout << setw(4) << clct->patternId() << endl;
					std::cout << setw(4) << clct->comparatorCodeId() << endl;
					std::cout << "pos offset: " << entry->position() << " in binary: " << std::bitset<5>(compressToNStripBits(entry->position())) <<  endl;
					std::cout << "slope offset: " << entry->slope() << "in binary: " << std::bitset<5>(compressToNSlopeBits(entry->slope())) <<  endl;
					CLCTFiles[i] << internal << setfill('0')  <<hex <<
							setw(4) << clct->keyHalfStrip() <<
							setw(4) << clct->patternId() <<
							setw(4) << clct->comparatorCodeId() <<
							setw(2) << compressToNStripBits(entry->position()) <<
							setw(2) << compressToNSlopeBits(entry->slope()) << endl;
				}else {
					CLCTFiles[i] << "0000000000000000" << endl;
				}
			}
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

		unsigned int writtenChamberCounter = 0;

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

				vector<CLCTCandidate*> emulatedCLCTs;

				if(searchForMatch(compHits,newPatterns, emulatedCLCTs,true)){
					emulatedCLCTs.clear();
					continue;
				}

				writeToMEMFiles(compHits, CFEBFiles);

				while(emulatedCLCTs.size() > 2) emulatedCLCTs.pop_back();

				std::cout << "khs, patt, ccode" << std::endl;
				std::cout << internal << setfill('0');
				for(unsigned int j=0; j < nCLCTs; j++){
					for(unsigned int ib=0; ib < 7; ib++){ //put in blank lines
						CLCTFiles[j] << "000000000000" << endl;
					}
					if(j < emulatedCLCTs.size()){//

						auto& clct = emulatedCLCTs.at(j);
						std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
						std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
						std::cout << setw(4) << clct->patternId()/10 << endl; //divide by 10 to have the amount of bits firmware expects
						std::cout << setw(4) << clct->comparatorCodeId() << endl;
						//divide by 10 to have the amount of bits firmware expects
						CLCTFiles[j] << internal << setfill('0')  <<hex << setw(4) << clct->keyHalfStrip() << setw(4) << clct->patternId()/10 << setw(4) << clct->comparatorCodeId() << endl;
					}else {
						CLCTFiles[j] << "000000000000" << endl;
					}
				}
				writtenChamberCounter++;
				cout <<dec << "writtenToFileCounter = " << writtenChamberCounter
						<<" -> line number: " << 8*writtenChamberCounter << endl;

				if(8*writtenChamberCounter == 216)return 0;
			}
		}

	}

	for(auto& cfebFile : CFEBFiles) cfebFile.close();
	for(auto& clctFile : CLCTFiles) clctFile.close();

	return 0;
}
