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
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <THStack.h>
#include <TString.h>

#include <iostream>

#include "../include/PatternFinder.h"
#include "../include/PatternFitter.h"

using namespace std;

//secret sauce to make vectors of vectors work in root
#ifdef __MAKECINT__
#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<vector<int> > >+;
#endif


int PatternFinder() {
	TFile* f = TFile::Open("../../../data/CSCDigiTreeCharmonium2016F.root");
	//TFile* f = TFile::Open("/Users/williamnash/rootPrograms/src/CSCDigiTree0.root");
	if(!f)
	{
		printf("Can't open file\n");
		return -1;
	}

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t){
		printf("Can't find tree\n");
		return -1;
	}

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

    vector<int>* lctId = 0;
    vector<vector<int>>* lctPat = 0;
    vector<vector<int>>* lctKHS = 0;

    //comparators
    vector<vector<int>>* compStr = 0; //comparator strip #
    vector<vector<int>>* compHS = 0; //comparator half strip #
    vector<vector<vector<int>>>* compTimeOn = 0;
    vector<int>* compLay = 0; // y axis
    vector<int>* compId = 0; // index of what ring/station you are on
    vector<int>* tmbId = 0;

	t->SetBranchAddress("segEc", &segEc);
	t->SetBranchAddress("segSt",&segSt);
	t->SetBranchAddress("segRi", &segRi);
	t->SetBranchAddress("segCh", &segCh);
	t->SetBranchAddress("segX", &segX);
	t->SetBranchAddress("compId",&compId);
	t->SetBranchAddress("compLay",&compLay);
	t->SetBranchAddress("compStr",&compStr);
	t->SetBranchAddress("compHS",&compHS);
	t->SetBranchAddress("compTimeOn", &compTimeOn);
	t->SetBranchAddress("Pt", &Pt);
	t->SetBranchAddress("eta", &eta);
	t->SetBranchAddress("rhId",&rhId);
	t->SetBranchAddress("rhLay", &rhLay);
	t->SetBranchAddress("rhPos", &rhPos);
	t->SetBranchAddress("rhE", &rhE);
	t->SetBranchAddress("lctId", &lctId);
	t->SetBranchAddress("lctPat",&lctPat);
	t->SetBranchAddress("lctKHS",&lctKHS);
	t->SetBranchAddress("tmbId",&tmbId);
	t->SetBranchAddress("os", &os);


	//
	// HISTOGRAMS
	//

	int maxPt = 80;

	TH1F* matched = new TH1F("h1","h1",2*maxPt, 0, maxPt);
	matched->SetFillColor(kBlue);
	matched->SetLineColor(kBlue);
	TH1F* missingOne = new TH1F("h2","h2",2*maxPt,0,maxPt);
	missingOne->SetFillColor(kGray);
	missingOne->SetLineColor(kGray);
	TH1F* missingTwo = new TH1F("h3","h3",2*maxPt,0,maxPt);
	missingTwo->SetFillColor(kOrange);
	missingTwo->SetLineColor(kOrange);
	TH1F* missingThree = new TH1F("h4","h4",2*maxPt,0,maxPt);
	missingThree->SetFillColor(kRed);
	missingThree->SetLineColor(kRed);
	TH1F* missed = new TH1F("h5","h5",2*maxPt,0,maxPt);
	missed->SetFillColor(kBlack);
	missed->SetLineColor(kBlack);
	TH1F* denominator = new TH1F("div","div", 2*maxPt, 0,maxPt);


	TH1F* lct1plct1 = new TH1F("lctplct11", "LCT && PLCT",2*maxPt, 0, maxPt);
	lct1plct1->SetFillColor(kBlue);
	lct1plct1->SetLineColor(kBlue);
	TH1F* lct1plct0 = new TH1F("lctplct10", "LCT && !PLCT",2*maxPt, 0, maxPt);
	lct1plct0->SetFillColor(kRed);
	lct1plct0->SetLineColor(kRed);
	TH1F* lct0plct1 = new TH1F("lctplct01", "!LCT && PLCT",2*maxPt, 0, maxPt);
	lct0plct1->SetFillColor(kYellow);
	lct0plct1->SetLineColor(kYellow);
	TH1F* lct0plct0 = new TH1F("lctplct00", "!LCT && !PLCT",2*maxPt, 0, maxPt);
	lct0plct0->SetFillColor(kBlack);
	lct0plct0->SetLineColor(kBlack);
	TH1F* lctPlctDenom = new TH1F("lctplct000", "!LCT && !PLCT",2*maxPt, 0, maxPt);


	TH1F* idHist = new TH1F("id","id", 7, -0.5, 6.5);


	// CREATE PID EFFICIENCY PLOTS
	vector<TH1F*> lctPidEff; //pid efficiency of LCTs
	vector<TH1F*> lctPidEff4; //pid efficiency of LCTs with RH >=4
	vector<TH1F*> plctPidEff;
	vector<TH1F*> plctPidEff4;

	const unsigned int nPidDivisions = 6;
	unsigned int colors [nPidDivisions] = {kRed,kBlue,kBlack,kGreen,kOrange,kBlack};
	for(unsigned int i  = 0; i < nPidDivisions; i++){
		TH1F* thisPlot11 = new TH1F(string("pt"+to_string(11)+to_string(i)).c_str(),
				string("pid >= " + to_string(2*(i+1))).c_str(),40,0,20);
		TH1F* thisPlot10 = new TH1F(string("pt"+to_string(10)+to_string(i)).c_str(),
						string("pid >= " + to_string(2*(i+1))).c_str(),40,0,20);
		TH1F* thisPlot01 = new TH1F(string("pt"+to_string(01)+to_string(i)).c_str(),
						string("pid >= " + to_string(2*(i+1))).c_str(),40,0,20);
		TH1F* thisPlot00 = new TH1F(string("pt"+to_string(00)+to_string(i)).c_str(),
						string("pid >= " + to_string(2*(i+1))).c_str(),40,0,20);
		TH1F* plotzzz[4] = {thisPlot11,thisPlot10,thisPlot01,thisPlot00};

		//quickly do the rest of the formatting
		for(unsigned int j = 0; j < 4; j++){
			TH1F* currPlot = plotzzz[j];
			if(i == nPidDivisions - 2) currPlot->SetTitle("pid = 10");
			currPlot->GetXaxis()->SetTitle("Pt [GeV]");
			currPlot->GetXaxis()->CenterTitle();
			currPlot->GetYaxis()->SetTitle("Efficiency/0.5 GeV");
			currPlot->GetYaxis()->CenterTitle();
			currPlot->SetLineColor(colors[i]);
		}
		lctPidEff.push_back(thisPlot11);
		lctPidEff4.push_back(thisPlot10);
		plctPidEff.push_back(thisPlot01);
		plctPidEff4.push_back(thisPlot00);
	}


	TH2F* matchComparison = new TH2F("layermatch","RH vs CH Layer Match",7,-0.5, 6.5,7,-0.5, 6.5);
	matchComparison->GetXaxis()->SetTitle("CompHits Layers Matched");
	matchComparison->GetYaxis()->SetTitle("RecHits Layers Matched");

	const int nStations = 4;
	const int nRings = 3;
	const int nEndcaps = 2;


	/*
	TH1F* patternIdPlot("subpattern", "SubPattern Id Frequency", 1000, 0, pow(2,12));
	patternIdPlot->GetXaxis()->SetTitle("Pattern ID Code");
	patternIdPlot->GetYaxis()->SetTitle("Entries");
	 */


	//
	// SET UP GROUPS
	//


	const unsigned int nMatchGroups = 4;
	PatternIDMatchPlots matchGroups[nMatchGroups] = {
			PatternIDMatchPlots("ME11A & ME12", createGroup1Pattern()),
			PatternIDMatchPlots("ME11B & ME13",createGroup2Pattern()),
			PatternIDMatchPlots("ME21 & ME22",createGroup3Pattern()),
			PatternIDMatchPlots("ME31 & ME32 & ME41 & ME42",createGroup4Pattern())};

	int idColors[18] = {2,3,4,5,6,7,8,9,40,41,42,43,44,45,30,31,32,33}; //easy, non-elegant way to handle coloring


	for(unsigned int igroup = 0; igroup < nMatchGroups; igroup++){
		vector<ChargeSuperPattern>* thisPatternSet = matchGroups[igroup].m_patterns;
		for(unsigned int i = 0; i < thisPatternSet->size(); i++){
			TH1F* thisHist;
			vector<int> thisHistIds;

			//if we just have 1 ida
			if(i ==0 || i+1 == thisPatternSet->size()){ //if its the first one, or if the next one doesnt exist
				thisHist = new TH1F(string(thisPatternSet->at(i).name() + to_string(igroup)).c_str(),
						thisPatternSet->at(i).name().c_str(), 2*maxPt, 0, maxPt);
				thisHist->SetFillColor(idColors[i]);
				thisHist->SetLineColor(idColors[i]);
				thisHistIds.push_back(thisPatternSet->at(i).m_id);
			}else{ //if we have at least two left
				thisHist = new TH1F(string(thisPatternSet->at(i).name() + to_string(igroup)).c_str(),
						string(thisPatternSet->at(i).name()+" "+thisPatternSet->at(i+1).name()).c_str(),2*maxPt,0,maxPt);
				thisHist->SetFillColor(idColors[i]);
				thisHist->SetLineColor(idColors[i]);
				thisHistIds.push_back(thisPatternSet->at(i).m_id);
				thisHistIds.push_back(thisPatternSet->at(i+1).m_id);
				i++; //skip the next one
			}
			matchGroups[igroup].m_plots.push_back(thisHist);
			matchGroups[igroup].m_matchIds.push_back(thisHistIds);

		}
		TH1F* missedID = new TH1F(string("missedID"+to_string(igroup)).c_str(), "missedID", 2*maxPt, 0,maxPt);
		missedID->SetFillColor(kBlack);
		missedID->SetLineColor(kBlack);
		matchGroups[igroup].m_plots.push_back(missedID);

		TH1F* thisDenom = new TH1F(string("div"+to_string(igroup)).c_str(),string("div"+to_string(igroup)).c_str(),2*maxPt, 0,maxPt);
		matchGroups[igroup].m_denominator = thisDenom;
	}



		TH2F* patternOverlap = new TH2F("patternOverlap", string("Pattern Overlap of Group " + to_string(TESTING_GROUP_INDEX+1) + " (" +
				matchGroups[TESTING_GROUP_INDEX].m_name + ")").c_str(),
				matchGroups[TESTING_GROUP_INDEX].m_patterns->size(), -0.5, matchGroups[TESTING_GROUP_INDEX].m_patterns->size() - 0.5,
				matchGroups[TESTING_GROUP_INDEX].m_patterns->size(), -0.5, matchGroups[TESTING_GROUP_INDEX].m_patterns->size() - 0.5);
		for(unsigned int i = 0; i < matchGroups[TESTING_GROUP_INDEX].m_patterns->size(); i++){
			patternOverlap->GetXaxis()->SetBinLabel(i+1, matchGroups[TESTING_GROUP_INDEX].m_patterns->at(i).name().c_str());
			patternOverlap->GetYaxis()->SetBinLabel(i+1, matchGroups[TESTING_GROUP_INDEX].m_patterns->at(i).name().c_str());
		}


	//
	// TREE ITERATION
	//

	for(unsigned int i = 0; i < MAX_ENTRY; i++) {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*i/MAX_ENTRY, i);
		t->GetEntry(i);

		if(!os) continue;

		//iterate through segments
		for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

			int EC = segEc->at(thisSeg);
			int ST = segSt->at(thisSeg);
			int RI = segRi->at(thisSeg);
			int CH = segCh->at(thisSeg);
			int chSid = chamberSerial(EC, ST, RI, CH);

			//if(ST != 2 && RI != 2) continue;

			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);


			ChamberHits theseRHHits;
			ChamberHits theseCompHits;
			//initialize them to zero
			for(int x=0; x < N_MAX_HALF_STRIPS; x++){
				for(int y = 0; y < NLAYERS; y++){
					theseRHHits.hits[x][y] = 0;
					theseCompHits.hits[x][y] = 0;
				}
			}

			theseRHHits.isComparator = false;
			theseCompHits.isComparator = true;

			//label their station, ring , endcap
			theseRHHits.station = ST;
			theseCompHits.station = ST;
			theseRHHits.ring = RI;
			theseCompHits.ring = RI;
			theseRHHits.endcap = EC;
			theseCompHits.endcap = EC;

			bool compLayers[NLAYERS] = {0,0,0,0,0,0};

			for(unsigned int icomp = 0; icomp < compId->size(); icomp++){
				if(chSid != compId->at(icomp)) continue; //only look at where we are now

				unsigned int thisCompLay = compLay->at(icomp)-1;
				for(unsigned int icompstr = 0; icompstr < compStr->at(icomp).size(); icompstr++){
					//goes from 1-80
					int compStrip = compStr->at(icomp).at(icompstr);
					int compHStrip = compHS->at(icomp).at(icompstr);

					int timeOn = 0;

					//look at the time on value, to fill the chamber array
					if(!compTimeOn->at(icomp)[icompstr].size()){
						printf("Error dimensions of comparator time on vector are incorrect. size %lu= \n",
								compTimeOn->at(icomp)[icompstr].size());
						return -1;
					} else {
						timeOn = compTimeOn->at(icomp)[icompstr].front();
					}

					//account for weird me11a/b
					if((me11a || me11b) && compStrip > 64) compStrip -= 64;


					int halfStripVal;
					if(me11a ||me11b || !(thisCompLay%2)){ //if we are in me11 or an even layer (opposite from Cameron's code, since I shift to zero)
						halfStripVal = 2*(compStrip-1)+compHStrip+1;
					} else { //odd layers shift down an extra half strip
						halfStripVal = 2*(compStrip-1)+compHStrip;
					}


					if(halfStripVal >= N_MAX_HALF_STRIPS || halfStripVal < 0) {
						printf("ERROR: For compId = %i, ST = %i, RI = %i Comp Half Strip Value out of range index = %i - me11b = %i\n",
								chSid,ST,RI, halfStripVal, me11b);
						return -1;
					} else {
						if(timeOn < 0 || timeOn >= 16) {
							printf("Error timeOn is an invalid number: %i\n", timeOn);
							return -1;
						} else {
							theseCompHits.hits[halfStripVal][thisCompLay] = timeOn+1; //store +1, so we dont run into trouble with hexadecimal
							if(validComparatorTime(theseCompHits.isComparator, theseCompHits.hits[halfStripVal][thisCompLay])){ //only consider a layer to be "on" if it is in our time window
								compLayers[thisCompLay] = true;
							}
						}
					}
				}
			}
			int nCh = 0; //number of comp hits
			for(int ilay = 0; ilay < NLAYERS; ilay++) nCh += compLayers[ilay];



			//lct = local charge track,  alct = anode (wire), clct = cathode (strip)
			//these are all the lct's that were recorded in the run.
			unsigned int LCT_ID = 0;
			for(unsigned int ilct = 0; ilct < lctId->size(); ilct++)
			{
				//skip until we find a match
				if(lctId->at(ilct) != chSid) continue;
				if(!lctPat->at(ilct).size()) break; //if its not in this one, it wont be in any

				int bestLCTFit = 0; //index of the best fit lct to the segment
				float minDiff = 999999;
				for(unsigned int chLct = 0; chLct < lctKHS->at(ilct).size(); chLct++)
				{
					//lct - 0.5 - 2*N_S + 0.5 , segX - 0 -N_S
					float lctStrip = 0.5*(lctKHS->at(ilct).at(chLct)- 0.5);
					//printf("lctKHS = %i - lctStrip = %f ---- segX = %f\n", lctKHS->at(ilct).at(chLct), lctStrip,  segX->at(iseg));
					float thisDiff = lctStrip -segX->at(thisSeg);
					if(abs(thisDiff) < abs(minDiff)) {
						minDiff = thisDiff;
						bestLCTFit = chLct; //assign the best index to the one we are on now
					}
				}
				LCT_ID = lctPat->at(ilct).at(bestLCTFit);
				break;
			}



			unsigned int nRh = 0;

			//find out where the reconstructed hits are for this segment
			for(unsigned int thisRh = 0; thisRh < rhId->size(); thisRh++)
			{
				int thisId = rhId->at(thisRh);

				if(chSid != thisId) continue; //just look at matches
				//rhLay goes 1-6
				unsigned int iLay = rhLay->at(thisRh)-1;

				//goes 1-80
				float thisRhPos = rhPos->at(thisRh);

				int iRhStrip = round(2.*thisRhPos-.5)-1; //round and shift to start at zero
				if(me11a ||me11b || !(iLay%2)) iRhStrip++; // add one to account for staggering, if even layer

				if(iRhStrip >= N_MAX_HALF_STRIPS || iRhStrip < 0){
					printf("ERROR: recHit index %i invalid\n", iRhStrip);
					return -1;
				}

				theseRHHits.hits[iRhStrip][iLay] = true;
				nRh++;
			}





			//find which group the current chamber you are looking at belongs to
			unsigned int groupIndex;
			if((ST == 1 && RI == 4 )||(ST == 1 && RI == 2)) groupIndex = 0; //ME11a || ME12
			else if((ST == 1 && RI == 1) || (ST == 1 && RI == 3)) groupIndex = 1;
			else if((ST == 2 && RI == 1) || (ST == 2 && RI == 2)) groupIndex = 2;
			else if(ST == 3 || ST == 4) groupIndex = 3;
			else{
				printf("ERROR: can find group\n");
				return -1;
			}



			///ALSO NEED TO CHECK TMB DATA, to make sure you use valid data
			if(MAKE_MATCH_LAYER_COMPARISON){

				SuperPatternSetMatchInfo* compMatchInfo = new SuperPatternSetMatchInfo();
				SuperPatternSetMatchInfo* recMatchInfo = new SuperPatternSetMatchInfo();

				bool hasTmbId = find(tmbId->begin(), tmbId->end(), chSid) != tmbId->end();


				//look only if it has TMB data, AND only if data is in the group we care about
				if(hasTmbId && (groupIndex == TESTING_GROUP_INDEX)) {
					if(searchForMatch(theseCompHits, matchGroups[groupIndex].m_patterns,compMatchInfo)) return -1;
					if(searchForMatch(theseRHHits, matchGroups[groupIndex].m_patterns,recMatchInfo)) return -1;
					matchComparison->Fill(compMatchInfo->bestLayerCount(),recMatchInfo->bestLayerCount());
				}
				delete compMatchInfo;
				delete recMatchInfo;
			}



			SuperPatternSetMatchInfo* thisSetMatch = new SuperPatternSetMatchInfo();
			ChamberHits* testChamber;
			unsigned int nHits; //number of hits, counts at max one from each layer
			if(USE_COMP_HITS){
				testChamber = &theseCompHits;
				nHits = nCh;
				//nHits = nRh; //using the number of rec hits here, as they are closest to the true value of actual hits in the chamber
			} else {
				testChamber = &theseRHHits;
				nHits = nRh;
			}

			//now run on comparator hits
			if(searchForMatch(*testChamber, matchGroups[groupIndex].m_patterns,thisSetMatch)) return -1;


			unsigned int maxLayerMatchCount = thisSetMatch->bestLayerCount();
			unsigned int maxLayerId = thisSetMatch->bestPatternId();
			//unsigned int maxLayerBaseSetIndex = thisSetMatch.bestSetIndex();

			//patternIdPlot->Fill(thisSetMatch)

			delete thisSetMatch;


			//skip every event that has no hits in it
			if(!maxLayerMatchCount) continue;

			matchGroups[groupIndex].m_denominator->Fill(Pt);


			if(maxLayerMatchCount >= N_LAYER_REQUIREMENT){

				bool found = false;

				for(unsigned int ihist = 0; ihist < matchGroups[groupIndex].m_matchIds.size() && !found; ihist++){
					for(unsigned int ihistIds = 0; ihistIds < matchGroups[groupIndex].m_matchIds.at(ihist).size() && !found; ihistIds++){
						if(maxLayerId == (unsigned int)matchGroups[groupIndex].m_matchIds[ihist][ihistIds]){
							matchGroups[groupIndex].m_plots[ihist]->Fill(Pt);
							found = true;
						}
					}
				}
				if(!found) {
					printf("ERROR: No matching ID Found\n");
					return -1;
				}

				/*
				if(groupIndex == TESTING_GROUP_INDEX){
					for(unsigned int ipat = 0; ipat < matchGroups[groupIndex].m_patterns->size(); ipat++){
						if(thisSetMatch.patternMatchCount[ipat] == maxLayerMatchCount){
							for(unsigned int iipat = 0; iipat < matchGroups[groupIndex].m_patterns->size(); iipat++){
								if(thisSetMatch.patternMatchCount[iipat] == maxLayerMatchCount){
									patternOverlap->Fill(ipat, iipat);
								}
							}
						}
					}
				}
				*/

			} else {
				//missed
				matchGroups[groupIndex].m_plots.back()->Fill(Pt);


			}

			if(groupIndex == TESTING_GROUP_INDEX){
				denominator->Fill(Pt);
				if(maxLayerMatchCount >= nHits){
					matched->Fill(Pt);
				} else if(maxLayerMatchCount < N_LAYER_REQUIREMENT){
					missed->Fill(Pt);
				} else if(maxLayerMatchCount == nHits - 1){
					missingOne->Fill(Pt);
				} else if(maxLayerMatchCount == nHits - 2){
					missingTwo->Fill(Pt);
				} else if(maxLayerMatchCount == nHits - 3){
					missingThree->Fill(Pt);
				}
			}

			bool PLCT_TRACK = (maxLayerMatchCount >= N_LAYER_REQUIREMENT);

			//if((maxLayerMatchCount >= 4 && !LCT_ID) || (maxLayerMatchCount < 3 && LCT_ID)){
			//if(!PLCT_TRACK && LCT_ID && false){
			if(false){ //feel free to change this
				printf("=-=-=-= For Event %i - Chamber ID = %i =-=-=-=\n", i,chSid);
				printf("~~~ Comp Hits ~~~\n");
				printChamber(theseCompHits);
				if(LCT_ID) {
					printf("LCT_ID = %i\n", LCT_ID);
				} else {
					printf("No LCT found for this Segment\n");
				}
				printf("~~~ Reconstructed Hits ~~~\n");
				printChamber(theseRHHits);
				printf("MatchCount = %i\n", maxLayerMatchCount);
				printf("Best RH PID = %i\n", maxLayerId);
			}

			lctPidEff.back()->Fill(Pt);
			if(LCT_ID >= 2) lctPidEff.at(0)->Fill(Pt);
			if(LCT_ID >= 4) lctPidEff.at(1)->Fill(Pt);
			if(LCT_ID >= 6) lctPidEff.at(2)->Fill(Pt);
			if(LCT_ID >= 8) lctPidEff.at(3)->Fill(Pt);
			if(LCT_ID == 10) lctPidEff.at(4)->Fill(Pt);

			plctPidEff.back()->Fill(Pt);
			if(PLCT_TRACK){
				if(maxLayerId >= 2) plctPidEff.at(0)->Fill(Pt);
				if(maxLayerId >= 4) plctPidEff.at(1)->Fill(Pt);
				if(maxLayerId >= 6) plctPidEff.at(2)->Fill(Pt);
				if(maxLayerId >= 8) plctPidEff.at(3)->Fill(Pt);
				if(maxLayerId == 10) plctPidEff.at(4)->Fill(Pt);
			}
			if(nHits >= 4){
				lctPidEff4.back()->Fill(Pt);
				if(LCT_ID >= 2) lctPidEff4.at(0)->Fill(Pt);
				if(LCT_ID >= 4) lctPidEff4.at(1)->Fill(Pt);
				if(LCT_ID >= 6) lctPidEff4.at(2)->Fill(Pt);
				if(LCT_ID >= 8) lctPidEff4.at(3)->Fill(Pt);
				if(LCT_ID == 10) lctPidEff4.at(4)->Fill(Pt);

				plctPidEff4.back()->Fill(Pt);
				if(PLCT_TRACK){
					if(maxLayerId >= 2) plctPidEff4.at(0)->Fill(Pt);
					if(maxLayerId >= 4) plctPidEff4.at(1)->Fill(Pt);
					if(maxLayerId >= 6) plctPidEff4.at(2)->Fill(Pt);
					if(maxLayerId >= 8) plctPidEff4.at(3)->Fill(Pt);
					if(maxLayerId == 10) plctPidEff4.at(4)->Fill(Pt);
				}
			}

			if(nHits >= N_LAYER_REQUIREMENT){

				lctPlctDenom->Fill(Pt);
				if(LCT_ID && PLCT_TRACK){
					lct1plct1->Fill(Pt);
				}else if(LCT_ID && !PLCT_TRACK){
					lct1plct0->Fill(Pt);
				} else if( !LCT_ID && PLCT_TRACK){
					lct0plct1->Fill(Pt);
				} else {
					lct0plct0->Fill(Pt);
				}
			}
		}
	}

	TCanvas* c = new TCanvas("c","comp",1200,900);
	c->SetLogy();


	//divide them all
	missed->Divide(denominator);
	missingThree->Divide(denominator);
	missingTwo->Divide(denominator);
	missingOne->Divide(denominator);
	matched->Divide(denominator);


	THStack* stack = new THStack("hs", string("Pattern Matched Distribution for 3-Wide Pattern (" +
			matchGroups[TESTING_GROUP_INDEX].m_name + ")").c_str());
	stack->Add(missed);
	stack->Add(missingThree);
	stack->Add(missingTwo);
	stack->Add(missingOne);
	stack->Add(matched);
	stack->Draw();
	stack->GetXaxis()->SetTitle("Transverse Momentum [GeV]");
	stack->GetXaxis()->CenterTitle();
	stack->GetYaxis()->SetTitle("Entries/0.5 GeV");
	stack->GetYaxis()->CenterTitle();


	auto legend = new TLegend(0.5,0.7,0.85,0.85);
	legend->AddEntry(matched, "Perfect Match");
	legend->AddEntry(missingOne,"Patterns Missed 1 Layer");
	legend->AddEntry(missingTwo,"Patterns Missed 2 Layers");
	legend->AddEntry(missingThree,"Patterns Missed 3 Layers");
	legend->AddEntry(missed,"Matched < 3 hits");
	legend->Draw();


	c->Modified();
	c->Update();


	TCanvas* c3 = new TCanvas("c3","comp3",1200,900);
	c3->Divide(2,2);


	for(unsigned int igroup = 0; igroup < nMatchGroups; igroup++){

		c3->cd(igroup+1);
		gPad->SetLogy();

		// sort by number of entries, first putting the missingID plot first
		vector<TH1F*>* idMatchPlots = &matchGroups[igroup].m_plots; //just so i dont have to rewrite all this
		reverse(idMatchPlots->begin(), idMatchPlots->end());

		THStack* stack2 = new THStack(string("hs2"+to_string(igroup)).c_str(),
				string("Matched (>= " + to_string(N_LAYER_REQUIREMENT) +
				" hits) for 3-Wide Pattern Set ("+
				matchGroups[igroup].m_name + ")").c_str());
		for(unsigned int ihist = 0; ihist < idMatchPlots->size(); ihist++){
			idMatchPlots->at(ihist)->Divide(matchGroups[igroup].m_denominator);
			stack2->Add(idMatchPlots->at(ihist));
		}

		stack2->Draw();
		stack2->GetXaxis()->SetTitle("Transverse Momentum [GeV]");
		stack2->GetXaxis()->CenterTitle();
		stack2->GetYaxis()->SetTitle("Entries/0.5 GeV");
		stack2->GetYaxis()->CenterTitle();


		auto legend2 = new TLegend(0.5,0.7,0.85,0.85);
		for(unsigned int ihist = 0; ihist < idMatchPlots->size(); ihist++){
			legend2->AddEntry(idMatchPlots->at(ihist));
		}
		legend2->Draw();

		c3->Modified();
		c3->Update();
	}

	if(MAKE_MATCH_LAYER_COMPARISON){
		TCanvas* c4 = new TCanvas("c4","comp4", 1200, 900);
		c4->cd();
		c4->SetLogz();
		matchComparison->Draw("COLZ");
	}




	return 0;
}
