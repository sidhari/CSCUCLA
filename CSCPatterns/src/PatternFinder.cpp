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


//#include <ROOT/TTreeProcessorMT.hxx>
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



// TODO: Check for TMB Headers

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


int PatternFinder(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";



	//
	// MAKE LUT
	//

	string dataset = "Charmonium/charmonium2016F+2017BCEF";
	//string dataset = "SingleMuon/zskim2018D";

	DetectorLUTs newLUTs;
	DetectorLUTs legacyLUTs(true);

	/*
	 * TEMPORARY, should be /luts/ not /linefitluts/
	 *
	 *
	 */
	const string newLutPath = "dat/"+dataset+"/luts/";
	const string legacyLutPath = "dat/"+dataset+"/luts/";

	cout << "Loading Luts..." << endl;
	//check if we have made .lut files already
	if(newLUTs.loadAll(newLutPath) ||
			legacyLUTs.loadAll(legacyLutPath)){
		printf("Could not find .lut files, recreating them...\n");
		//string lutFilepath = "/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/"+dataset+"/CLCTMatch-Full.root";
		string lutFilepath = "/uscms/home/wnash/CSCUCLA/CSCPatterns/dat/"+dataset+"/CLCTMatch-Full.root";
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

		newLUTs.writeAll("dat/"+dataset+"/luts/");
		legacyLUTs.writeAll("dat/"+dataset+"/luts/");
	} else {
		newLUTs.makeFinal();
		legacyLUTs.makeFinal();
	}

	cout << "Loaded LUTS" << endl;

	//pointers used to look at different LUT's
	//const LUT* thisLUT = 0;
	//const LUTEntry* thisEntry = 0;


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


	vector<CSCPattern>* newPatterns= createNewPatterns();
	vector<CSCPattern>* oldPatterns = createOldPatterns();

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
	float pt = 0;
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
	plotTree->Branch("pt", &pt, "pt/F");
	plotTree->Branch("segmentX", &segmentX, "segmentX/F");
	plotTree->Branch("segmentdXdZ", &segmentdXdZ, "segmentdXdZ/F");
	plotTree->Branch("patX", &patX, "patX/F");
	plotTree->Branch("legacyLctX", &legacyLctX, "legacyLctX/F");

	TH1F* lutSegmentPosDiff = new TH1F("h_lutSegmentPosDiff", "h_lutSegmentPosDiff", 100, -1, 1);
	TH1F* lutSegmentPosDiff_halfStrip = new TH1F("h_lutSegmentPosDiff_halfStrip", "h_lutSegmentPosDiff_halfStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_quarterStrip = new TH1F("h_lutSegmentPosDiff_quarterStrip", "h_lutSegmentPosDiff_quarterStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_eighthStrip = new TH1F("h_lutSegmentPosDiff_eighthStrip", "h_lutSegmentPosDiff_eighthStrip", 100, -1, 1);
	TH1F* lutSegmentPosDiff_sixteenthStrip = new TH1F("h_lutSegmentPosDiff_sixteenthStrip", "h_lutSegmentPosDiff_sixteenthStrip", 100, -1, 1);
	TH1F* lutSegmentSlopeDiff = new TH1F("h_lutSegmentSlopeDiff", "h_lutSegmentSlopeDiff", 100, -1, 1);


	map<pair<int,int>, TH1F*> lutChamberPlots_Pos;
	map<pair<int,int>, TH1F*> lutChamberPlots_legacy_Pos; //emulated
	//map<pair<int,int>, TH1F*> lutChamberPlots_legacy_Pos_real; //actual recorded clcts
	map<pair<int,int>, TH1F*> lutChamberPlots_Slope;
	map<pair<int,int>, TH1F*> lutChamberPlots_legacy_Slope;
	for(unsigned int i=0; i < NCHAMBERS; i++){
		unsigned int station = CHAMBER_ST_RI[i][0];
		unsigned int ring = CHAMBER_ST_RI[i][1];
		auto key = make_pair(station,ring);

		string posName = string("h_"+CHAMBER_NAMES[i]) + "_posDiff";
		string posLegacyName = string("h_"+CHAMBER_NAMES[i]) + "_legacy_posDiff";
		//string posLegacyRealName = string("h_"+CHAMBER_NAMES[i]) + "_legacy_posDiff_real";
		string slopeName = string("h_"+CHAMBER_NAMES[i]) + "_legacy_slopeDiff";
		string slopeLegacyName = string("h_"+CHAMBER_NAMES[i]) + "_slopeDiff";
		auto posHist = new TH1F(posName.c_str(), string(posName+string("; Seg- LUT [strips]; CLCTs")).c_str(), 100,-1,1);
		auto posLegacyHist = new TH1F(posLegacyName.c_str(), string(posLegacyName+string("; Seg- LUT [strips]; CLCTs")).c_str(), 100,-1,1);
		//auto posLegacyRealHist = new TH1F(posLegacyRealName.c_str(), string(posLegacyRealName+string("; Seg- KS [strips]; CLCTs")).c_str(), 100,-1,1);
		auto slopeHist = new TH1F(slopeName.c_str(), string(slopeName+string("; Seg - LUT [strips]; CLCTs")).c_str(), 100,-1,1);
		auto slopeLegacyHist = new TH1F(slopeLegacyName.c_str(), string(slopeLegacyName+string("; Seg - LUT [strips]; CLCTs")).c_str(), 100,-1,1);
		lutChamberPlots_Pos[key] = posHist;
		lutChamberPlots_legacy_Pos[key] = posLegacyHist;
		//lutChamberPlots_legacy_Pos_real[key] = posLegacyRealHist;
		lutChamberPlots_Slope[key] = slopeHist;
		lutChamberPlots_legacy_Slope[key] = slopeLegacyHist;
	}
	map<int, TH1F*> legacyPatterns_pos; //emulated
	map<int, TH1F*> legacyPatterns_pos_real;
	map<int, TH1F*> legacyPatterns_slope;
	for(unsigned int i =0; i < NLEGACYPATTERNS; i++){
		string name = string("h_legacy_") + to_string(LEGACY_PATTERN_IDS[i]);
		string posName = name + "_pos";
		string posRealName = name + "_real_pos";
		string slopeName = name + "_slope";
		auto pos = new TH1F(posName.c_str(), posName.c_str(), 100, -1,1);
		auto posReal = new TH1F(posRealName.c_str(), posName.c_str(), 100, -1,1);
		auto slope = new TH1F(slopeName.c_str(), slopeName.c_str(), 100, -1,1);
		legacyPatterns_pos[LEGACY_PATTERN_IDS[i]] = pos;
		legacyPatterns_pos_real[LEGACY_PATTERN_IDS[i]] = posReal;
		legacyPatterns_slope[LEGACY_PATTERN_IDS[i]] = slope;
	}

	TH1F* legacyLUTSegmentPosDiff = new TH1F("h_legacyPosDiff", "h_legacyPosDiff; lut - seg[strips]; segments",100,-1,1);
	//TH1F* legacyLUTSegmentPosDiff = new TH1F("h_legacyPosDiff", "h_legacyPosDiff; lut - seg[strips]; segments",400,-4,4);
	TH1F* legacyLUTSegmentSlopeDiff = new TH1F("h_legacySlopeDiff", "h_legacySlopeDiff; lut - seg [strips/lay]", 100, -1,1);
	TH1F* h_clctLayerCount = new TH1F("h_clctLayerCount", "h_clctlayerCount", 7,0,7);

	map<float, TH1F*> ccPos_pt;
	map<float, TH1F*> ccSlope_pt;
	map<float, TH1F*> legacyPos_pt;
	map<float, TH1F*> legacySlope_pt;

	//looks at region of pt [0 - entry1, entry1- entry2, etc]
	vector<float> ptRanges;
	ptRanges.push_back(20);
	ptRanges.push_back(50);
	ptRanges.push_back(100);
	ptRanges.push_back(200);
	ptRanges.push_back(500);

	for(unsigned int ipt = 0; ipt < ptRanges.size(); ipt++){
		string lower = (ipt == 0) ? "0" : to_string(round(ptRanges.at(ipt-1)));
		float thisPt = ptRanges.at(ipt);
		string upper = to_string(round(thisPt));
		string rangeStr = "_"+lower+"_"+upper;

		TH1F* ccPos = new TH1F(("h_ccPos"+rangeStr).c_str(), ("h_ccPos"+rangeStr+";Segment-LUT[strips]; Segments").c_str(), 100, -1,1);
		TH1F* ccSlope = new TH1F(("h_ccSlope"+rangeStr).c_str(), ("h_ccSlope"+rangeStr+";Segment-LUT[strips/layers]; Segments").c_str(), 100, -1,1);
		TH1F* legPos = new TH1F(("h_legPos"+rangeStr).c_str(), ("h_legPos"+rangeStr+";Segment-LUT[strips]; Segments").c_str(), 100, -1,1);
		TH1F* legSlope = new TH1F(("h_legSlope"+rangeStr).c_str(), ("h_legSlope"+rangeStr+";Segment-LUT[strips/layer]; Segments").c_str(), 100, -1,1);
		ccPos_pt[thisPt] = ccPos;
		ccSlope_pt[thisPt] = ccSlope;
		legacyPos_pt[thisPt] = legPos;
		legacySlope_pt[thisPt] = legSlope;
	}


	//t->SetBranchAddress("Event_EventNumber", &evt.EventNumber);

	//CSCInfo::Muons muons;
	//t->SetBranchAddress("muon_pt", &muons.pt);

	/*
	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
		//printf("EventNum = %llu\n", evt.EventNumber);
		for(unsigned int j = 0; j < muons.size(); j++) {
			printf("muon_pt: %f\n", muons.pt[j]);
		}

	}
	return 0;
*/

	//
	// TREE ITERATION
	//

	unsigned int nChambersRanOver = 0;
	unsigned int nChambersMultipleInOneLayer = 0;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);
	//t->SetImplicitMT(true);


	for(int i = start; i < end; i++) {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++){
			int chamberHash = segments.ch_id->at(thisSeg);
			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			EC = c.endcap;
			ST = c.station;
			RI = c.ring;
			CH = c.chamber;

			segmentX = segments.pos_x->at(thisSeg); //strips
			segmentdXdZ = segments.dxdz->at(thisSeg);
			pt = muons.pt->at(segments.mu_id->at(thisSeg));
			//if(pt > 12) continue; //REMOVE ME
			//if(pt < 40) continue; //REMOVE ME



			// IGNORE SEGMENTS AT THE EDGES OF THE CHAMBERS
			if(CSCHelper::segmentIsOnEdgeOfChamber(segmentX, ST,RI)) continue;


			ChamberHits theseRHHits(ST, RI, EC, CH,false);
			ChamberHits theseCompHits(ST, RI, EC, CH);

			if(theseCompHits.fill(comparators)) return -1;

			if (!USE_COMP_HITS && DEBUG > 0) if(fillRecHits(theseRHHits,recHits)) return -1;

			vector<CLCTCandidate*> newSetMatch;
			vector<CLCTCandidate*> oldSetMatch;

			ChamberHits* testChamber;
			testChamber = USE_COMP_HITS ? &theseCompHits : &theseRHHits;

			nChambersRanOver++;

			//now run on comparator hits
			if(DEBUG > 0) printf("~~~~ Matches for Muon: %i,  Segment %i ~~~\n",i,  thisSeg);
			if(searchForMatch(*testChamber, oldPatterns,oldSetMatch) || searchForMatch(*testChamber, newPatterns,newSetMatch)) {
			/*Temporary, to test if busy window is effecting strange behavior with pattersn 8 and 9
			 *
			 */
			//if(searchForMatch(*testChamber, oldPatterns,oldSetMatch,true) || searchForMatch(*testChamber, newPatterns,newSetMatch,true)) {
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

			/* NEW LUTS
			 *
			 */

			if(setLUTEntries(newSetMatch, newLUTs, ST, RI)) return -1;


			/* TODO: use this sorting thing to see what gets you the first candidate as the right segment
			 * each time a la Nick
			 */
			//sort the matches
			//sort(newSetMatch.begin(), newSetMatch.end(), CLCTCandidate::quality);



			// fill the numerator if it is within our capture window
			//float posCaptureWindow = 0.30; //strips
			//float slopeCaptureWindow = 0.25; //strips/layer


			bool foundMatchingCandidate = false;
			float minX = 1e5;
			float minX_halfStrip = 1e5;
			float minX_quarterStrip = 1e5;
			float minX_eighthStrip = 1e5;
			float minX_sixteenthStrip = 1e5;
			float mindXdZ = 1e5;

			//look through all the candidates, until we find the first match
			for(auto& clct: newSetMatch){
				//depending on how many clcts were allowed to look at,
				// look until we find one
				//const LUTEntry* iEntry = newSetMatch.at(iclct)->_lutEntry;


				float lutX = clct->position();
				float lutdXdZ =clct->slope();


				float xDiff = segmentX - lutX;
				float xDiff_halfStrip = segmentX-round(2.*lutX)/2.;
				float xDiff_quarterStrip =  segmentX- round(4.*lutX)/4.;
				float xDiff_eighthStrip = segmentX - round(8.*lutX)/8.;
				float xDiff_sixteenthStrip = segmentX - round(16.*lutX)/16.;
				float dxdzDiff = segmentdXdZ - lutdXdZ;
				//cout << " EC " << EC << "ST: " <<ST << endl;
				//cout << "xDiff " << xDiff << "nsegs " << newSetMatch.at(iclct)->_lutEntry->nsegments() <<endl;

				if(abs(xDiff) < abs(minX)){
					minX = xDiff;
					minX_halfStrip = xDiff_halfStrip;
					minX_quarterStrip = xDiff_quarterStrip;
					minX_eighthStrip = xDiff_eighthStrip;
					minX_sixteenthStrip = xDiff_sixteenthStrip;
					mindXdZ = dxdzDiff;
					foundMatchingCandidate = true;
				}

			}
			if(foundMatchingCandidate ){
				lutSegmentPosDiff->Fill(minX);
				lutSegmentPosDiff_halfStrip->Fill(minX_halfStrip);
				lutSegmentPosDiff_quarterStrip->Fill(minX_quarterStrip);
				lutSegmentPosDiff_eighthStrip->Fill(minX_eighthStrip);
				lutSegmentPosDiff_sixteenthStrip->Fill(minX_sixteenthStrip);
				lutSegmentSlopeDiff->Fill(mindXdZ);

				//fill chamber specific stuff
				auto chamberKey = make_pair(ST,RI);
				lutChamberPlots_Pos[chamberKey]->Fill(minX);
				lutChamberPlots_Slope[chamberKey]->Fill(mindXdZ);

				for(unsigned int ipt = 0; ipt < ptRanges.size(); ipt++){
					float lower = (ipt == 0) ? 0 : ptRanges.at(ipt-1);
					float upper = ptRanges.at(ipt);
					if(pt >= lower && pt < upper){
						ccPos_pt[upper]->Fill(minX);
						ccSlope_pt[upper]->Fill(mindXdZ);
					}
				}


			}

			//if(foundMatchingCandidate) foundOneMatchEffNum->Fill(Pt);

			/*
			 *  OLD LUTS
			 */
			if(setLUTEntries(oldSetMatch, legacyLUTs, ST,RI)) return -1;

			bool foundMatchingCandidate_legacy = false;
			float minX_legacy = 1e5;
			float mindXdZ_legacy = 1e5;

			int bestLegacyPattern = -1;
			for(auto& clct: oldSetMatch){
				float lutX = clct->position();
				float lutdXdZ = clct->slope();

				float xDiff = segmentX-lutX;
				float dxdzDiff = segmentdXdZ - lutdXdZ;
				if(abs(xDiff) < abs(minX_legacy)){
				//if(abs(xDiff) < abs(minX_legacy) && abs(dxdzDiff) < abs(mindXdZ_legacy)){
						minX_legacy = xDiff;
						mindXdZ_legacy = dxdzDiff;
						bestLegacyPattern = clct->_pattern._id;
						foundMatchingCandidate_legacy = true;
					}
			}
			if(foundMatchingCandidate_legacy){
				legacyLUTSegmentPosDiff->Fill(minX_legacy);
				legacyLUTSegmentSlopeDiff->Fill(mindXdZ_legacy);

				auto chamberKey = make_pair(ST,RI);
				lutChamberPlots_legacy_Pos[chamberKey]->Fill(minX_legacy);
				lutChamberPlots_legacy_Slope[chamberKey]->Fill(mindXdZ_legacy);

				legacyPatterns_pos[bestLegacyPattern]->Fill(minX_legacy);
				legacyPatterns_slope[bestLegacyPattern]->Fill(mindXdZ_legacy);

				for(unsigned int ipt = 0; ipt < ptRanges.size(); ipt++){
					float lower = (ipt == 0) ? 0 : ptRanges.at(ipt-1);
					float upper = ptRanges.at(ipt);
					if(pt >= lower && pt < upper){
						legacyPos_pt[upper]->Fill(minX_legacy);
						legacySlope_pt[upper]->Fill(mindXdZ_legacy);
					}
				}


				//compare 21 and 31, one should be flipped, the other not
				/*
				if((ST == 2 && RI == 1) || (ST == 3 && RI == 1)) {
					if(abs(minX_legacy) > 0.3){
						theseCompHits.print();
						for(auto& clct: oldSetMatch){
							printPattern(clct->_pattern);
						}
						cout << "--- Segment Position: " << segmentX << " [strips] Slope: " << segmentdXdZ << " [strips/layer]---" << endl;
						cout << "Legacy Match: (";
						for(auto& clct: oldSetMatch){
							cout << clct->keyStrip() << ", ";
						}
						cout << ") [strips]" << endl;
						cout << "Legacy Match LUT: (";
						for(auto& clct: oldSetMatch){
							cout << "[" << clct->position() << ", " << clct->slope() << "], ";
						}
						cout << ") [strips]" << endl;

					}
				}
				*/
			}


			//
			// Iterate over real clcts
			//
			bool foundMatchingCandidate_real = false;
			float minX_real = 1e5;
			int bestRealPattern = -1;
			for(unsigned int iclct=0; iclct < clcts.size(); iclct++){
				int clctHash = clcts.ch_id->at(iclct);
				if(clctHash != chamberHash) continue;
				float clctHSPos = clcts.keyStrip->at(iclct);
				float clctSPos = clctHSPos/2. + 1; //conver to real strips...

				float xDiff= segmentX - clctSPos;
				if(abs(xDiff) < abs(minX_real)){
					minX_real = xDiff;
					bestRealPattern = clcts.pattern->at(iclct);
					foundMatchingCandidate_real = true;
				}
			}
			if(foundMatchingCandidate_real){
				legacyPatterns_pos_real[bestRealPattern]->Fill(minX_real);
			}


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

			h_clctLayerCount->Fill(newSetMatch.at(closestNewMatchIndex)->layerCount());

			//CLCTCandidate* bestCLCT = newSetMatch.at(closestNewMatchIndex);



			//Clear everything

			oldSetMatch.clear();
			newSetMatch.clear();
		}

	}



	printf("fraction with >1 in layer is %i/%i = %f\n", nChambersMultipleInOneLayer, nChambersRanOver, 1.*nChambersMultipleInOneLayer/nChambersRanOver);


	outF->cd();
	plotTree->Write();
	lutSegmentPosDiff->Write();
	lutSegmentPosDiff_halfStrip->Write();
	lutSegmentPosDiff_quarterStrip->Write();
	lutSegmentPosDiff_eighthStrip->Write();
	lutSegmentPosDiff_sixteenthStrip->Write();
	lutSegmentSlopeDiff->Write();

	legacyLUTSegmentPosDiff->Write();
	legacyLUTSegmentSlopeDiff->Write();

	for(auto entry: ccPos_pt) entry.second->Write();
	for(auto entry: ccSlope_pt) entry.second->Write();
	for(auto entry: legacyPos_pt) entry.second->Write();
	for(auto entry: legacySlope_pt) entry.second->Write();

	for(auto entry: lutChamberPlots_Pos) entry.second->Write();
	for(auto entry: lutChamberPlots_legacy_Pos) entry.second->Write();
	for(auto entry: lutChamberPlots_Slope) entry.second->Write();
	for(auto entry: lutChamberPlots_legacy_Slope) entry.second->Write();
	for(auto entry: legacyPatterns_pos) entry.second->Write();
	for(auto entry: legacyPatterns_pos_real) entry.second->Write();
	for(auto entry: legacyPatterns_slope) entry.second->Write();
	h_clctLayerCount->Write();
	/*
	lutSegmentPosDiff->Write();
	lutSegmentSlopeDiff->Write();

	for(unsigned int isegeff = 0; isegeff < segEffNums.size(); isegeff++){
		segEffNums.at(isegeff)->Write();
	}
	segEffDen->Write();

	//defined as : for every segment, we have at least one clct matched within the range
	foundOneMatchEffNum->Write();
	foundOneMatchEffDen->Write();
*/
	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());


	// print program timing information
	//cout << "Time elapsed: " << float(clock()- c_start) / CLOCKS_PER_SEC << " s" << endl;

	//return 0;

//	cout << "Fraction with >1 in layer is " <<  (*nChambersMultipleInOneLayer.Get()) << "/" <<
//			(*nChambersRanOver.Get()) << " = " << 1.*(*nChambersMultipleInOneLayer.Get())/(*nChambersRanOver.Get()) << endl;

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	return 0;

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







