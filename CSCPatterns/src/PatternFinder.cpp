/*
 * PatternFinder.cpp
 *
 *  Created on: Sep 27, 2017
 *      Author: root
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
//#include <TROOT.h>



#include <ROOT/TTreeProcessorMT.hxx>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <time.h>


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

using namespace std;


/* TODO: Multithreading
 *
 * - open files in main thread
 * - make histograms in main thread (following recipe here: https://root.cern.ch/doc/v612/imt101__parTreeProcessing_8C.html)
 * - make tree in main thread, using same recipe book
 * - pass tree to new function PatternFinderThread(ttree*(?)), should return a ttree as well
 * - join threads
 * - combine output trees, and write output file: https://root-forum.cern.ch/t/merging-ttrees-on-the-fly/1833
 *
 */



//
//int PatternFinder(string inputfile, string outputfile, int start=0, int end=-1) {
//
//	unsigned int nthreads = std::thread::hardware_concurrency();
//	ROOT::EnableImplicitMT(nthreads);
//	ROOT::EnableThreadSafety();
//	ROOT::TTreeProcessorMT tp(inputfile.c_str(), "CSCDigiTree");
//
//	auto processPatternFinder = [&](TTreeReader &r) {
//		cout << "start: " << start << " end: " << end << endl;
//		r.SetEntriesRange(start, end);
//
//
//		while(r.Next()){
//			cout << "r.Entry: " << r.GetCurrentEntry() << endl;
//		}
//	};
//
//	try {
//		tp.Process(processPatternFinder);
//	} catch( const char* msg) {
//		cerr << "ERROR: " << msg << endl;
//		return -1;
//	}
//	return 0;
//}


int PatternFinder(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());

	unsigned int nthreads = std::thread::hardware_concurrency();
	//unsigned int nthreads = 1;
	ROOT::EnableImplicitMT(nthreads);
	ROOT::EnableThreadSafety();
	printf("Parallelizing into %i threads\n", nthreads);
/*

	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";
*/
	ROOT::TTreeProcessorMT tp(inputfile.c_str(), "CSCDigiTree");
	//tp.treeView.
	//tp.SetEntriesRange(0,1);



	//
	// MAKE LUT
	//

	string dataset = "Charmonium/charmonium2016F+2017BCEF";


	//ROOT::TThreadedObject<DetectorLUTs> t_newLUTs;
	//ROOT::TThreadedObject<DetectorLUTs> t_legacyLUTs(true);

	/*
	//make shortcut pointers to the thread function
	shared_ptr<DetectorLUTs> newLUTs = t_newLUTs.Get();
	shared_ptr<DetectorLUTs> legacyLUTs = t_legacyLUTs.Get();

	//check if we have made .lut files already
	if(newLUTs->loadAll("data/"+dataset+"/luts/") ||
			legacyLUTs->loadAll("data/"+dataset+"/luts/")){
		cout << "Could not find .lut files, recreating them..." << endl;
		string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/data/"+dataset+"/CLCTMatch-Full.root";
		TFile* lutFile = new TFile(lutFilepath.c_str());
		if(!lutFile){
			throw "Failed to open lut file: %s\n", lutFilepath.c_str();
		}

		//TODO: change the name of the tree!
		TTree* lutTree = (TTree*)lutFile->Get("plotTree");
		if(!lutTree){
			throw "Can't find lutTree";
		}
		if(makeLUT(lutTree, newLUTs, legacyLUTs)){
			throw "Couldn't create LUT";
		}

		newLUTs->writeAll("data/"+dataset+"/luts/");
		legacyLUTs->writeAll("data/"+dataset+"/luts/");
	} else {
		newLUTs->makeFinal();
		legacyLUTs->makeFinal();
	}
*/
	DetectorLUTs newLUTs;
	DetectorLUTs legacyLUTs(true);


	//check if we have made .lut files already
	if(newLUTs.loadAll("data/"+dataset+"/luts/") ||
			legacyLUTs.loadAll("data/"+dataset+"/luts/")){
		printf("Could not find .lut files, recreating them...\n");
		string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/data/"+dataset+"/CLCTMatch-Full.root";
		TFile* lutFile = new TFile(lutFilepath.c_str());
		if(!lutFile){
			printf("Failed to open lut file: %s\n", lutFilepath.c_str());
			return -1;
		}

		//TODO: change the name of the tree!
		TTree* lutTree = (TTree*)lutFile->Get("plotTree");
		if(!lutTree){
			printf("Can't find lutTree\n");
			return -1;
		}
		if(makeLUT(lutTree, newLUTs, legacyLUTs)){
			cout << "Error: couldn't create LUT" << endl;
			return -1;
		}

		newLUTs.writeAll("data/"+dataset+"/luts/");
		legacyLUTs.writeAll("data/"+dataset+"/luts/");
	} else {
		newLUTs.makeFinal();
		legacyLUTs.makeFinal();
	}


	//
	// TEMP
	//

	//LUT* myLut =0;
	//newLUTs.editLUT(1,1,myLut);
	//myLut->writeDPSLUTs("test");

	LUT testLUT("test", "/home/wnash/workspace/CSCUCLA/CSCPatterns/data/linearFits.lut");
	testLUT.writeDPSLUTs("LineFits");

	return 0;

	//pointers used to look at different LUT's
	//shared_ptr<LUT> thisLUT;
	//shared_ptr<const LUTEntry> thisEntry;


	//
	// MAKE ALL THE PATTERNS
	//

	//TODO: these should be const, otherwise threads might mess stuff up!
	vector<CSCPattern>* newPatterns = createNewPatterns();
	vector<CSCPattern>* oldPatterns = createOldPatterns();


	ROOT::TThreadedObject<unsigned int> nTreeEntriessRanOver(0);
	ROOT::TThreadedObject<unsigned int> nChambersRanOver(0);
	ROOT::TThreadedObject<unsigned int> nChambersMultipleInOneLayer(0);


	//
	// Parallelized Tree Function
	//

	auto processPatternFinder = [&](TTreeReader &r) {
		cout << "start: " << start << " end: " << end << endl;
		r.SetEntriesRange(start, end);



		TTreeReaderValue<double> RV_Pt(r, "Pt");
		TTreeReaderValue<double> RV_eta(r, "eta");
		TTreeReaderValue<bool> RV_os(r, "os");

		//reconstructed offline hits (rechits)
		TTreeReaderValue<vector<int>> RV_rhId(r, "rhId");
		TTreeReaderValue<vector<int>> RV_rhLay(r, "rhLay");
		TTreeReaderValue<vector<float>> RV_rhPos(r,"rhPos");

		//segments
		TTreeReaderValue<vector<int>> RV_segEc(r,"segEc");
		TTreeReaderValue<vector<int>> RV_segSt(r,"segSt");
		TTreeReaderValue<vector<int>> RV_segRi(r,"segRi");
		TTreeReaderValue<vector<int>> RV_segCh(r,"segCh");
		TTreeReaderValue<vector<float>> RV_segX(r,"segX");
		TTreeReaderValue<vector<float>> RV_segdXdZ(r,"segdXdZ");

		//lcts
		TTreeReaderValue<vector<int>> RV_lctId(r,"lctId");
		TTreeReaderValue<vector<vector<int>>> RV_lctPat(r,"lctPat");
		TTreeReaderValue<vector<vector<int>>> RV_lctKHS(r,"lctKHS");

		//comparators
		TTreeReaderValue<vector<int>> RV_compLay(r,"compLay"); // y axis
		TTreeReaderValue<vector<int>> RV_compId(r,"compId"); // index of what ring/station you are on
		TTreeReaderValue<vector<vector<int>>> RV_compStr(r,"compStr");//comparator strip #
		TTreeReaderValue<vector<vector<int>>> RV_compHS(r,"compHS");  //comparator half strip #
		TTreeReaderValue<vector<vector<vector<int>>>> RV_compTimeOn(r,"compTimeOn");

		//at present, just create a tree in each thread, then add them together in the main thread, maybe there is a better way to do this?

		//
		// OUTPUT TREE
		//

		int patternId = 0;
		int ccId = 0;
		int legacyLctId = 0;
		int EC = 0; // 1-2
		int ST = 0; // 1-4
		int RI = 0; // 1-4
		int CH = 0;
		float segmentX = 0;
		float segmentdXdZ = 0;
		float patX = 0;
		float legacyLctX = 0;

		unsigned int TEMP_COUNTER = 0;


		while(r.Next()){
			cout << TEMP_COUNTER << endl;
			TEMP_COUNTER++;

			//
			// Make pointers on the stack to the thread safe objects
			//

			double Pt  = *RV_Pt;
			double eta = *RV_eta;
			bool os = *RV_os;

			vector<int>* rhId = &(*RV_rhId);
			vector<int>* rhLay = &(*RV_rhLay);
			vector<float>* rhPos = &(*RV_rhPos);
		//	vector<float>* rhE = &(*RV_rhE);

			vector<int>     *segEc = &(*RV_segEc);
			vector<int>     *segSt = &(*RV_segSt);
			vector<int>     *segRi = &(*RV_segRi);
			vector<int>     *segCh = &(*RV_segCh);
			vector<float>   *segX = &(*RV_segX);
			vector<float>	*segdXdZ = &(*RV_segdXdZ);

			vector<int>* compLay = &(*RV_compLay);
			vector<int>* compId = &(*RV_compId);
			vector< vector<int> >* compStr = &(*RV_compStr);
			vector< vector<int> >* compHS = &(*RV_compHS);
			vector< vector< vector<int> > >* compTimeOn = &(*RV_compTimeOn);

			if(!os) continue;

			//iterate through segments
			for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

				EC = (*segEc)[thisSeg];
				ST = (*segSt)[thisSeg];
				RI = (*segRi)[thisSeg];
				CH = (*segCh)[thisSeg];

				segmentX = segX->at(thisSeg); //strips
				segmentdXdZ = segdXdZ->at(thisSeg);


				// IGNORE SEGMENTS AT THE EDGES OF THE CHAMBERS
				if(segmentX < 1) continue;
				bool me11a = (ST == 1 && RI == 4);
				bool me11b = (ST == 1 && RI == 1);
				bool me13 = (ST == 1 && RI == 3);
				if(me11a){
					if(segmentX > 47) continue;
				} else if (me11b || me13) {
					if(segmentX > 63) continue;
				} else {
					if(segmentX > 79) continue;
				}


				ChamberHits theseRHHits(0, ST, RI, EC, CH);
				ChamberHits theseCompHits(1, ST, RI, EC, CH);

				if(fillCompHits(theseCompHits, compStr,compHS,compTimeOn, compLay,compId)){
					throw "Couldn't fill Comparator hits";
				}


				//if(fillCompHits(theseCompHits, compStr,compHS,compTimeOn, compLay,compId)) return -1;

				if (!USE_COMP_HITS || DEBUG) {
				//if (!USE_COMP_HITS || DEBUG >=0) {
					if(fillRecHits(theseRHHits,rhId, rhLay,rhPos)) {
						throw "Couldn't fill recHits";
					}
				}


				vector<CLCTCandidate*> newSetMatch;
				vector<CLCTCandidate*> oldSetMatch;

				ChamberHits* testChamber;
				testChamber = USE_COMP_HITS ? &theseCompHits : &theseRHHits;

				(*nChambersRanOver.Get())++;

				//now run on comparator hits
				//if(DEBUG > 0) printf("~~~~ Matches for Muon: %i,  Segment %i ~~~\n",i,  thisSeg);
				if(searchForMatch(*testChamber, oldPatterns,oldSetMatch) || searchForMatch(*testChamber, newPatterns,newSetMatch)) {
					oldSetMatch.clear();
					newSetMatch.clear();
					(*nChambersMultipleInOneLayer.Get())++;
					continue;
				}

				//TODO: currently no implementation dealing with cases where we find one and not other
				if(!oldSetMatch.size() || !newSetMatch.size()) {
					oldSetMatch.clear();
					newSetMatch.clear();
					continue;
				}

				//Now compare with LUT data
				const LUT* thisLUT = 0;
				const LUTEntry* thisEntry = 0;

				if(newLUTs.getLUT(ST,RI,thisLUT)) {
					throw ("Can't access LUT for: " + to_string(ST) + ", " + to_string(RI)).c_str();
					//return -1;
				}

				//TODO: make debug printout of this stuff
				for(auto & clct: newSetMatch){
					if(thisLUT->getEntry(clct->key(), thisEntry)){
						throw "Unable to get entry for clct";
						//return -1;
					}
					//assign the clct the LUT entry we found to be associated with it
					clct->_lutEntry = thisEntry;
				}

			}
		}
	};

	//TTreeReader tr;
	//tr.SetEntriesRange(start+1,end+1);
	//(*processPatternFinder)
	//processPatternFinder(tr);

//	auto fp = bind(processPatternFinder, tr);

	try {

	//	tp.Process(processPatternFinder);
		tp.Process(processPatternFinder);
	} catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}

	auto t2 = std::chrono::high_resolution_clock::now();

	cout << "Fraction with >1 in layer is " <<  (*nChambersMultipleInOneLayer.Get()) << "/" <<
			(*nChambersRanOver.Get()) << " = " << 1.*(*nChambersMultipleInOneLayer.Get())/(*nChambersRanOver.Get()) << endl;

	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;
/*



	double Pt = 0;
	double eta = 0;
	bool os = 0;

	//reconstructed offline hits
	vector<int>* rhId = 0; //id/serial
	vector<int>* rhLay = 0;
	vector<float>* rhPos = 0;
	vector<float>* rhE = 0; //energy

	//segments
	vector<int>     *segEc = 0;
	vector<int>     *segSt = 0;
	vector<int>     *segRi = 0;
	vector<int>     *segCh = 0;
	vector<float>   *segX = 0;
	vector<float>	*segdXdZ = 0;

	vector<int>* lctId = 0;
	vector< vector<int> >* lctPat = 0;
	vector< vector<int> >* lctKHS = 0;

	//comparators
	vector<int>* compLay = 0; // y axis
	vector<int>* compId = 0; // index of what ring/station you are on
	vector< vector<int> >* compStr = 0; //comparator strip #
	vector< vector<int> >* compHS = 0; //comparator half strip #
	vector< vector< vector<int> > >* compTimeOn = 0;

	t->SetBranchAddress("Pt",         &Pt);
	t->SetBranchAddress("eta",        &eta);
	t->SetBranchAddress("os",         &os);
	t->SetBranchAddress("rhId",       &rhId);
	t->SetBranchAddress("rhLay",      &rhLay);
	t->SetBranchAddress("rhPos",      &rhPos);
	t->SetBranchAddress("rhE",        &rhE);
	t->SetBranchAddress("segEc",      &segEc);
	t->SetBranchAddress("segSt",      &segSt);
	t->SetBranchAddress("segRi",      &segRi);
	t->SetBranchAddress("segCh",      &segCh);
	t->SetBranchAddress("segX",       &segX);
	t->SetBranchAddress("segdXdZ",    &segdXdZ);
	t->SetBranchAddress("lctId",      &lctId);
	t->SetBranchAddress("lctPat",     &lctPat);
	t->SetBranchAddress("lctKHS",     &lctKHS);
	t->SetBranchAddress("compId",     &compId);
	t->SetBranchAddress("compLay",    &compLay);
	t->SetBranchAddress("compStr",    &compStr);
	t->SetBranchAddress("compHS",     &compHS);
	t->SetBranchAddress("compTimeOn", &compTimeOn);


	//
	// MAKE ALL THE PATTERNS
	//


	vector<CSCPattern>* newEnvelopes = createNewPatterns();
	vector<CSCPattern>* oldEnvelopes = createOldPatterns();

	//
	// OUTPUT TREE
	//

	int patternId = 0;
	int ccId = 0;
	int legacyLctId = 0;
	int EC = 0; // 1-2
	int ST = 0; // 1-4
	int RI = 0; // 1-4
	int CH = 0;
	float segmentX = 0;
	float segmentdXdZ = 0;
	float patX = 0;
	float legacyLctX = 0;

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	TTree * plotTree = new TTree("plotTree","TTree holding processed info for CSCPatterns studies");
	plotTree->Branch("EC",&EC,"EC/I");
	plotTree->Branch("ST",&ST,"ST/I");
	plotTree->Branch("RI",&RI,"RI/I");
	plotTree->Branch("CH",&CH,"CH/I");
	plotTree->Branch("patternId", &patternId, "patternId/I");
	plotTree->Branch("ccId", &ccId, "ccId/I");
	plotTree->Branch("legacyLctId", &legacyLctId, "legacyLctId/I");
	plotTree->Branch("segmentX", &segmentX, "segmentX/F");
	plotTree->Branch("segmentdXdZ", &segmentdXdZ, "segmentdXdZ/F");
	plotTree->Branch("patX", &patX, "patX/F");
	plotTree->Branch("legacyLctX", &legacyLctX, "legacyLctX/F");


	TH1F* lutSegmentPosDiff = new TH1F("lutSegmentPosDiff", "lutSegmentPosDiff", 100, -1, 1);
	TH1F* lutSegmentSlopeDiff = new TH1F("lutSegmentSlopeDiff", "lutSegmentSlopeDiff", 100, -1, 1);

	vector<TH1F*> segEffNums;  //segment efficiency histograms, based on ranking of LUT Entry
	TH1F* segEffDen = new TH1F("segEffDen", "segEffDen", 30,0,150);


	segEffDen->GetXaxis()->SetTitle("Pt [GeV]");
	segEffDen->GetYaxis()->SetTitle("Count / 5 GeV");
	for(unsigned int clctRank = 0; clctRank < 6; clctRank++){
		segEffNums.push_back(new TH1F(("segEffNum"+to_string(clctRank)).c_str(),
				string("segEffNum"+to_string(clctRank)).c_str(), 30, 0, 150));
		segEffNums.back()->GetXaxis()->SetTitle("Pt [GeV]");
		segEffNums.back()->GetYaxis()->SetTitle("Count / 5 GeV");
	}

	TH1F* foundOneMatchEffNum = new TH1F("foundOneMatchEffNum", "foundOneMatchEffNum", 30,0,150);
	TH1F* foundOneMatchEffDen = new TH1F("foundOneMatchEffDen", "foundOneMatchEffDen", 30,0,150);

	foundOneMatchEffNum->GetXaxis()->SetTitle("Pt [GeV]");
	foundOneMatchEffNum->GetYaxis()->SetTitle("Count / 5 Gev");

	foundOneMatchEffDen->GetXaxis()->SetTitle("Pt [GeV]");
	foundOneMatchEffDen->GetXaxis()->SetTitle("Count / 5 GeV");


	vector<TH1F*> chi2Distributions; // Sum_layers [ (comparator half strip - segment position extrapolated to layer)^2 / (expected error)^2 ]
	vector<TH2F*> chi2VsSlope;
	for(unsigned int i = N_LAYER_REQUIREMENT; i < NLAYERS+1; i++){
		chi2Distributions.push_back(new TH1F(("h_"+to_string(i)+"layer_Chi2").c_str(),
				("h_"+to_string(i)+"layer_Chi2").c_str(),100, 0, 30));
		chi2Distributions.back()->GetXaxis()->SetTitle("#chi^2");
		chi2Distributions.back()->GetYaxis()->SetTitle("Counts");
		chi2VsSlope.push_back(new TH2F(("chi2VsSlope_"+to_string(i)).c_str(),
				("chi2VsSlope"+to_string(i)).c_str(),100, -1, 1, 100, 0, 30));
		chi2VsSlope.back()->GetXaxis()->SetTitle("Segment Slope [strip / layer]");
		chi2VsSlope.back()->GetYaxis()->SetTitle("#chi^{2}");


	}

	//temp - TO REMOVE
	vector<TH1F*> chi2PosDiffs;
	for(int i =0; i < 6; i++){
		chi2PosDiffs.push_back(new TH1F(("chi2posdiff_"+to_string(i)).c_str(),
				("chi2posdiff_"+to_string(i)).c_str(), 100, -5, 5));
	}
	//vector<TH2F*> chi2VsSlope = new TH2F("chi2VsSlope", "chi2VsSlope",100, -1, 1, 100, 0, 30);
	//TH1F* chi2PosDiff = new TH1F("chi2posdiff", "chi2posdiff", 100, -5, 5);


	//
	// MAKE LUT
	//

	string dataset = "Charmonium/charmonium2016F+2017BCEF";

	DetectorLUTs newLUTs;
	DetectorLUTs legacyLUTs(true);


	//check if we have made .lut files already
	if(newLUTs.loadAll("data/"+dataset+"/luts/") ||
			legacyLUTs.loadAll("data/"+dataset+"/luts/")){
		printf("Could not find .lut files, recreating them...\n");
		string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/data/"+dataset+"/CLCTMatch-Full.root";
		TFile* lutFile = new TFile(lutFilepath.c_str());
		if(!lutFile){
			printf("Failed to open lut file: %s\n", lutFilepath.c_str());
			return -1;
		}

		//TODO: change the name of the tree!
		TTree* lutTree = (TTree*)lutFile->Get("plotTree");
		if(!lutTree){
			printf("Can't find lutTree\n");
			return -1;
		}
		if(makeLUT(lutTree, newLUTs, legacyLUTs)){
			cout << "Error: couldn't create LUT" << endl;
			return -1;
		}

		newLUTs.writeAll("data/"+dataset+"/luts/");
		legacyLUTs.writeAll("data/"+dataset+"/luts/");
	} else {
		newLUTs.makeFinal();
		legacyLUTs.makeFinal();
	}


	//pointers used to look at different LUT's
	const LUT* thisLUT = 0;
	const LUTEntry* thisEntry = 0;

	//
	// TREE ITERATION
	//

	unsigned int nChambersRanOver = 0;
	unsigned int nChambersMultipleInOneLayer = 0;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	t->SetImplicitMT(true);


	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		if(!os) continue;

		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

			EC = (*segEc)[thisSeg];
			ST = (*segSt)[thisSeg];
			RI = (*segRi)[thisSeg];
			CH = (*segCh)[thisSeg];

			segmentX = segX->at(thisSeg); //strips
			segmentdXdZ = segdXdZ->at(thisSeg);


			// IGNORE SEGMENTS AT THE EDGES OF THE CHAMBERS
			if(segmentX < 1) continue;
			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);
			bool me13 = (ST == 1 && RI == 3);
			if(me11a){
				if(segmentX > 47) continue;
			} else if (me11b || me13) {
				if(segmentX > 63) continue;
			} else {
				if(segmentX > 79) continue;
			}


			//TODO: REMOVE THISSSSSSSSSSSSSS
			if(!me11b) continue;


			ChamberHits theseRHHits(0, ST, RI, EC, CH);
			ChamberHits theseCompHits(1, ST, RI, EC, CH);

			if(fillCompHits(theseCompHits, compStr,compHS,compTimeOn, compLay,compId)) return -1;

			if (!USE_COMP_HITS || DEBUG) if(fillRecHits(theseRHHits,rhId, rhLay,rhPos)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			ChamberHits* testChamber;
			testChamber = USE_COMP_HITS ? &theseCompHits : &theseRHHits;

			nChambersRanOver++;
			foundOneMatchEffDen->Fill(Pt);

			//now run on comparator hits
			if(DEBUG > 0) printf("~~~~ Matches for Muon: %i,  Segment %i ~~~\n",i,  thisSeg);
			if(searchForMatch(*testChamber, oldEnvelopes,oldSetMatch) || searchForMatch(*testChamber, newEnvelopes,newSetMatch)) {
				oldSetMatch.clear();
				newSetMatch.clear();
				nChambersMultipleInOneLayer++;
				continue;
			}

			//TODO: currently no implementation dealing with cases where we find one and not other
			if(!oldSetMatch.size() || !newSetMatch.size()) {
				oldSetMatch.clear();
				newSetMatch.clear();
				continue;
			}

			//Now compare with LUT data

			if(newLUTs.getLUT(ST,RI,thisLUT)) {
				printf("Error: can't access LUT for: %i %i\n", ST,RI);
				return -1;
			}

			//TODO: make debug printout of this stuff
			for(auto & clct: newSetMatch){
				if(thisLUT->getEntry(clct->key(), thisEntry)){
					printf("Error: unable to get entry for clct\n");
					return -1;
				}
				//assign the clct the LUT entry we found to be associated with it
				clct->_lutEntry = thisEntry;
			}


			if(newSetMatch.size() > 1){
				sort(newSetMatch.begin(), newSetMatch.end(), CLCTCandidate::quality);
			}

			if(DEBUG > 0){
				printf("segmentX: %f - segmentdXdZ: %f\n", segmentX,segmentdXdZ);
				for(auto & clct : newSetMatch){
					thisEntry = clct->_lutEntry;

					float thisLutX = clct->keyStrip() + thisEntry->position();
					float thisLutSlope = thisEntry->slope();
					printf("\t\tlutx: %f, lut dxdz: %f layers: %i, chi2: %f, slope: %f\n",
							thisLutX, thisLutSlope, thisEntry->_layers, thisEntry->_chi2, thisEntry->slope());
				}
			}

			// fill the numerator if it is within our capture window
			float posCaptureWindow = 0.30; //strips
			float slopeCaptureWindow = 0.25; //strips/layer

			bool foundMatchingCandidate = false;

			//look through all the candidates, until we find the first match
			for(unsigned int iclct = 0; !foundMatchingCandidate && iclct < newSetMatch.size() && iclct < segEffNums.size(); iclct++){
				//depending on how many clcts were allowed to look at,
				// look until we find one
				const LUTEntry* iEntry = newSetMatch.at(iclct)->_lutEntry;


				float lutX = newSetMatch.at(iclct)->keyStrip() + iEntry->position();
				float lutdXdZ =iEntry->slope();

				//only fill the best candidate
				if(iclct == 0){
					lutSegmentPosDiff->Fill(lutX - segmentX);
					lutSegmentSlopeDiff->Fill(lutdXdZ - segmentdXdZ);
				}


				if(abs(lutX - segmentX) < posCaptureWindow &&
						abs(lutdXdZ -segmentdXdZ) < slopeCaptureWindow){

					foundMatchingCandidate = true;
					segEffNums.at(iclct)->Fill(Pt);
					segEffDen->Fill(Pt);
				}
				//segEffNums.at(isegeff)->Fill(Pt);
			}

			if(foundMatchingCandidate) foundOneMatchEffNum->Fill(Pt);


			if(DEBUG > 0) cout << "--- Segment Position: " << segmentX << " [strips] ---" << endl;
			if(DEBUG > 0) cout << "Legacy Match: (";
			int closestOldMatchIndex = findClosestToSegment(oldSetMatch,segmentX);
			if(DEBUG > 0) cout << ") [strips]" << endl;


			if(DEBUG > 0)cout << "New Match: (";
			int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);
			if(DEBUG > 0) cout << ") [strips]" << endl;

			// Fill Tree Data

			patX = newSetMatch.at(closestNewMatchIndex)->keyStrip();
			ccId = newSetMatch.at(closestNewMatchIndex)->comparatorCodeId();
			patternId = newSetMatch.at(closestNewMatchIndex)->patternId();
			legacyLctId = oldSetMatch.at(closestOldMatchIndex)->patternId();
			legacyLctX = oldSetMatch.at(closestOldMatchIndex)->keyStrip();

			plotTree->Fill();

			CLCTCandidate* bestCLCT = newSetMatch.at(closestNewMatchIndex);


			unsigned int layers = bestCLCT->_lutEntry->_layers;


			int code_hits [MAX_PATTERN_WIDTH][NLAYERS];
			if(bestCLCT->getHits(code_hits)){
				printf("Error: can't recover hits\n");
				return -1;
			}

			float hs_clctkeyhs = 2*bestCLCT->keyStrip();



			//if(patternId != 100 || ccId != 1365) continue;
			//if(!me11b) continue;
			//printf("new segment\n");
			//calculate chi^2
			float clctChi2 = 0;
			for(int ilay = 0; ilay < (int)NLAYERS; ilay++){
				float hs_segPosOnThisLayer = 2.*(segmentX + segmentdXdZ*(2-ilay));
				//if(me11a || me11b) hs_segPosOnThisLayer += 1.;
				//printf("hs_segPos: %3.2f segX: %3.2f segdXdZ: %3.2f: ilay: %i\n",hs_segPosOnThisLayer, segmentX, segmentdXdZ, ilay);
				for(unsigned int hs = 0; hs < MAX_PATTERN_WIDTH; hs++){
					//printf("%i", code_hits[hs][ilay]);
					if(code_hits[hs][ilay]){
						float pattMinusSeg = (hs-0.5*(MAX_PATTERN_WIDTH)+1) + hs_clctkeyhs -hs_segPosOnThisLayer;
						chi2PosDiffs.at(ilay)->Fill(pattMinusSeg);
						float width = 1./sqrt(12);
						//float width = 1.;
						float thisChi2 = pow(pattMinusSeg/width, 2);
						//if(DEBUG){
							//printf("\tkeyhs - segpos: %3.2f   hs_in_pat = %f\n",hs_clctkeyhs -hs_segPosOnThisLayer,hs-0.5*(MAX_PATTERN_WIDTH)+1);
							//printf("\tthisChi2: %3.2f hs: %u hs_clctkeyhs: %3.2f  hs_segpos: %3.2f, patt-seg = %3.2f\n", thisChi2,hs, hs_clctkeyhs, hs_segPosOnThisLayer,pattMinusSeg);
						//}
						clctChi2 += thisChi2;
						break; //only one comp hit per layer
					}
				}
			}
			if(layers >= N_LAYER_REQUIREMENT) {
				chi2Distributions.at(layers-(N_LAYER_REQUIREMENT))->Fill(clctChi2);
				chi2VsSlope.at(layers-N_LAYER_REQUIREMENT)->Fill(segmentdXdZ,clctChi2);
			}
			//bestCLCT->printCodeInPattern();




			//Clear everything

			oldSetMatch.clear();
			newSetMatch.clear();

			//temp
			//return 0;
		}

	}



	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);

	outF->cd();
	plotTree->Write();
	lutSegmentPosDiff->Write();
	lutSegmentSlopeDiff->Write();

	for(unsigned int isegeff = 0; isegeff < segEffNums.size(); isegeff++){
		segEffNums.at(isegeff)->Write();
	}
	segEffDen->Write();

	//defined as : for every segment, we have at least one clct matched within the range
	foundOneMatchEffNum->Write();
	foundOneMatchEffDen->Write();

	for(auto hist : chi2PosDiffs) hist->Write();
	for(auto hist : chi2Distributions) hist->Write();
	for(auto hist : chi2VsSlope) hist->Write();

	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());


	// print program timing information
	//cout << "Time elapsed: " << float(clock()- c_start) / CLOCKS_PER_SEC << " s" << endl;

	//return 0;
	auto t2 = std::chrono::high_resolution_clock::now();

//	cout << "Fraction with >1 in layer is " <<  (*nChambersMultipleInOneLayer.Get()) << "/" <<
//			(*nChambersRanOver.Get()) << " = " << 1.*(*nChambersMultipleInOneLayer.Get())/(*nChambersRanOver.Get()) << endl;

	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;
	*/
}


int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return PatternFinder(string(argv[1]), string(argv[2]));
		case 4:
			return PatternFinder(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return PatternFinder(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







