/*
 * CLCTSelector.cpp
 *
 *  Created on: June 27, 2019
 *      Author: Siddharth Hariprakash
 */

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include<TCanvas.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <map>


#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/CLCTSelctor.h"

using namespace std;

int main(int argc, char* argv[])
{
	CLCTSelector p;
	return p.main(argc,argv);
}

int CLCTSelector::run(string inputfile, string outputfile, int start, int end) 
{

	cout << endl << "Running over file: " << inputfile << endl << endl;


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

    TFile* f_emu = TFile::Open("dat/Trees/EmulationResults_EverythingEndingIn1_1.root");

    TTree* t_emu = (TTree*)f_emu->Get("EmulationResults");

	//load LUT

	LUT lut(string("lut"));
	lut.loadText_bayes(string("dat/luts/lbxk_test.lut")); 
	if(lut.makeFinal())
	{
		return -1;
	}

	LUT linearfits(string("linfits"));
	linearfits.loadText(string("dat/luts/linearFits.lut"));
	if(linearfits.makeFinal())
	{
		return -1;
	}


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

    EmulatedCLCTs emulatedclcts(t_emu,2);	

	vector<CSCPattern>* newEnvelopes = createNewPatterns();

	//
	//COUNTERS
	//
	
	unsigned long long int totalmuonsegments = 0;
	unsigned long long int totalmatches = 0;
	unsigned long long int firstclctmatches = 0;
	unsigned long long int secondclctmatches = 0;
	unsigned long long int otherindexclctmatches = 0;

	//
	//HISTOGRAMS
	//

	TH1F* matchespt = new TH1F ("matched_muon_segments_pt", "Matched Muon Segments: pt;Pt[GeV];Count", 70, 0, 100);
	TH1F* firstclctmatchespt = new TH1F ("muon_segments_matched_to_first_CLCT_pt", "Muon Segments matched to first CLCT: pt", 70, 0, 100);
	TH1F* firstclctmatchespt_oldalgo = new TH1F ("muon_segments_matched_to_first_CLCT_oldalgo", "Muon Segments matched to first CLCT (Old Algo): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_layers = new TH1F ("muon_segments_matched_to_first_CLCT_pt_layers", "Muon Segments matched to first CLCT (Layers): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_PID = new TH1F ("muon_segments_matched_to_first_CLCT_pt_PID", "Muon Segments matched to first CLCT (PID): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_bayes = new TH1F ("muon_segments_matched_to_first_CLCT_pt_bayes", "Muon Segments matched to first CLCT (Bayes): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_chi2 = new TH1F ("muon_segments_matched_to_first_CLCT_pt_chi2", "Muon Segments matched to first CLCT (Chi2): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_KHS = new TH1F ("muon_segments_matched_to_first_CLCT_pt_KHS", "Muon Segments matched to first CLCT (KHS): pt", 70, 0, 100);
	TH1F* firstclctmatchespt_slope = new TH1F("muon_segments_matched_to_first_CLCT_pt_slope", "Muon Segments matched to first CLCT (slope): pt", 70, 0, 100);
	TH1F* secondclctmatchespt = new TH1F ("muon_segments_matched_to_second_CLCT_pt", "Muon Segments matched to second CLCT: pt", 70, 0, 100);
	TH1F* otherindexclctmatchespt = new TH1F ("muon_segments_matched_to_other_CLCTs_pt", "Muon Segments matched to other CLCTs: pt", 70, 0, 100);
	TH1F* effeciency1 = new TH1F ("first_index_efficiency", "First CLCT Matches;Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_oldalgo = new TH1F ("first_index_efficiency_oldalgo", "First CLCT Matches (Old Algo);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_layers = new TH1F ("first_index_efficiency_layers", "First CLCT Matches (Layers);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_PID = new TH1F ("first_index_efficiency_PID", "First CLCT Matches (PID);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_bayes = new TH1F ("first_index_efficiency_bayes", "First CLCT Matches (Bayes);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_chi2 = new TH1F ("first_index_efficiency_chi2", "First CLCT Matches (Chi2);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_KHS = new TH1F ("first_index_efficiency_KHS", "First CLCT Matches (KHS);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency1_slope = new TH1F ("first_index_efficiency_slope", "First CLCT Matches (slope);Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency2 = new TH1F ("second_index_efficiency", "Second CLCT Matches;Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* effeciency3 = new TH1F ("other_indices_efficiency", "Other CLCT Matches;Pt[GeV];Efficiency", 70, 0, 100);
	TH1F* matchedclctindex = new TH1F ("matched_clct_index", "Matched CLCT Indices", 10, 0, 10);
	
	//
	// EVENT LOOP
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();


	cout << "Starting Event: " << start << " Ending Event: " << end << endl << endl;

	for(int i = start; i < end; i++) 
    {
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);
        t_emu->GetEntry(i);

        /* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		 * installed on September 12
		 */
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;

		//iterate through all chambers

        for(unsigned int chamberHash = 0; chamberHash < (unsigned int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++)
        {		

			CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

			unsigned int EC = c.endcap;
			unsigned int ST = c.station;
			unsigned int RI = c.ring;
			unsigned int CH = c.chamber;

			if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

            bool me11a = (ST==1 && RI==4);
            bool me11b = (ST==1 && RI == 1);
            bool me13 = (ST==1 && RI==3);	

            ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;

			vector<CLCTCandidate*> initialclctcandidates;

			//iterate through all emulated CLCTs in chamber, fill vector 

			for(unsigned int iclct = 0; iclct < (unsigned int)emulatedclcts.size(); iclct++)
			{	
				if(emulatedclcts.ch_id->at(iclct) != chamberHash) 
				continue;

				if(emulatedclcts.layerCount->at(iclct) < 3)
				continue;

				int PID = emulatedclcts.patternId->at(iclct);				
				ComparatorCode c((unsigned int)emulatedclcts.comparatorCodeId->at(iclct));

				switch (PID)
				{	
					case 100:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(0),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;
					}

					case 90:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(1),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;	
					}

					case 80:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(2),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);	
						break;
					}

					case 70:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(3),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);	
						break;
					}

					case 60:
					{
						CLCTCandidate *clct = new CLCTCandidate(newEnvelopes->at(4),c,(int)emulatedclcts._horizontalIndex->at(iclct),(int)emulatedclcts._startTime->at(iclct));
						initialclctcandidates.push_back(clct);
						break;
					}

					default:
					{
						cout << "Invalid PID" << endl << endl;
						break;
					} 

				}

			}

			vector<CLCTCandidate*> initialclctcandidates_temp = initialclctcandidates;

			unsigned int segcount = 0;

			for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
			{
				if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
				continue;

				if(segments.mu_id->at(iseg) == -1)
				continue;

				segcount++;
			}

			if(segcount != 1)
			continue;
			if(initialclctcandidates.size() < 2)
			continue;

			for(unsigned int temp = 0; temp < initialclctcandidates.size(); temp++)
			{
				const LUTEntry *e;
				int PID = initialclctcandidates.at(temp)->patternId();
				int CCID = initialclctcandidates.at(temp)->comparatorCodeId();
				LUTKey k(PID,CCID);
				if(lut.getEntry(k,e))
				{
					cout << PID << " " << CCID << endl << endl;
					continue;
				}
				initialclctcandidates.at(temp)->_lutEntry = e;
			}

			//split CLCTs by CFEB

			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_oldalgo;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_layers;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_PID;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_bayes;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_chi2;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_KHS;
			map<unsigned int,vector<CLCTCandidate*>> CFEBsplit_slope;

			vector<unsigned int> CFEBpossibilities; //keeps track of which CFEBs have CLCTs

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{
				float keystrip = initialclctcandidates.at(iclct)->keyStrip();
				unsigned int whichCFEB = (keystrip-1)/16;
				CFEBsplit[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_oldalgo[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_layers[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_PID[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_bayes[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_chi2[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_KHS[whichCFEB].push_back(initialclctcandidates.at(iclct));
				CFEBsplit_slope[whichCFEB].push_back(initialclctcandidates_temp.at(iclct));
				if(std::find(CFEBpossibilities.begin(),CFEBpossibilities.end(),whichCFEB) == CFEBpossibilities.end())
				CFEBpossibilities.push_back(whichCFEB);
			}	

			if(CFEBpossibilities.size() < 2)
			continue;

			//sort within each CFEB		

			for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
			{ 
				sort(CFEBsplit[*it].begin(),CFEBsplit[*it].end(),CLCTCandidate::cfebquality);
				sort(CFEBsplit_oldalgo[*it].begin(), CFEBsplit_oldalgo[*it].end(), CLCTCandidate::cfebquality);
				sort(CFEBsplit_layers[*it].begin(),CFEBsplit_layers[*it].end(),CLCTCandidate::cfebquality);
				sort(CFEBsplit_PID[*it].begin(),CFEBsplit_PID[*it].end(),CLCTCandidate::cfebquality);
				sort(CFEBsplit_bayes[*it].begin(),CFEBsplit_bayes[*it].end(),CLCTCandidate::cfebquality);
				sort(CFEBsplit_chi2[*it].begin(),CFEBsplit_chi2[*it].end(),CLCTCandidate::cfebquality);
				sort(CFEBsplit_KHS[*it].begin(),CFEBsplit_KHS[*it].end(),CLCTCandidate::cfebquality);	
				for(unsigned int iclct = 0; iclct < CFEBsplit_slope[*it].size(); iclct++)
				{
					const LUTEntry *e;
					int PID = CFEBsplit_slope[*it].at(iclct)->patternId();
					int CCID = CFEBsplit_slope[*it].at(iclct)->comparatorCodeId();
					LUTKey k(PID,CCID);
					if(linearfits.getEntry(k,e))
					{
						cout << PID << " " << CCID << endl << endl;
						continue;
					}
					CFEBsplit_slope[*it].at(iclct)->_lutEntry = e;
				}				
				sort(CFEBsplit_slope[*it].begin(),CFEBsplit_slope[*it].end(),CLCTCandidate::cfebquality);
			}

			//pick first from each CFEB, use LUT to find best

			vector<CLCTCandidate*> finalclctcandidates;

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit[*it].size() == 0)
					continue;
					finalclctcandidates.push_back(CFEBsplit[*it].at(0));
				}

				sort(finalclctcandidates.begin()+iclct,finalclctcandidates.end(),CLCTCandidate::LUTquality);

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit[*it].size() == 0)
					continue;
					CFEBsplit[*it].erase(CFEBsplit[*it].begin());
				}
			}

			//Old Algo

			vector<CLCTCandidate*> finalclctcandidates_oldalgo;

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_oldalgo[*it].size() == 0)
					continue;
					finalclctcandidates_oldalgo.push_back(CFEBsplit_oldalgo[*it].at(0));
				}

				sort(finalclctcandidates_oldalgo.begin()+iclct,finalclctcandidates_oldalgo.end(),CLCTCandidate::cfebquality);

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_oldalgo[*it].size() == 0)
					continue;
					CFEBsplit_oldalgo[*it].erase(CFEBsplit_oldalgo[*it].begin());
				}
			}


			vector<CLCTCandidate*> finalclctcandidates_layers;
			vector<CLCTCandidate*> finalclctcandidates_PID;
			vector<CLCTCandidate*> finalclctcandidates_bayes;
			vector<CLCTCandidate*> finalclctcandidates_chi2;
			vector<CLCTCandidate*> finalclctcandidates_KHS;
			vector<CLCTCandidate*> finalclctcandidates_slope;

			//just layers

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_layers[*it].size() == 0)
					continue;
					finalclctcandidates_layers.push_back(CFEBsplit_layers[*it].at(0));
				}

				sort(finalclctcandidates_layers.begin()+iclct,finalclctcandidates_layers.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(c1->layerCount() >= c2->layerCount())
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_layers[*it].size() == 0)
					continue;
					CFEBsplit_layers[*it].erase(CFEBsplit_layers[*it].begin());
				}
			}	

			//just PID

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_PID[*it].size() == 0)
					continue;
					finalclctcandidates_PID.push_back(CFEBsplit_PID[*it].at(0));
				}

				sort(finalclctcandidates_PID.begin()+iclct,finalclctcandidates_PID.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(c1->patternId() >= c2->patternId())
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_PID[*it].size() == 0)
					continue;
					CFEBsplit_PID[*it].erase(CFEBsplit_PID[*it].begin());
				}
			} 

			//just bayes

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_bayes[*it].size() == 0)
					continue;
					finalclctcandidates_bayes.push_back(CFEBsplit_bayes[*it].at(0));
				}

				sort(finalclctcandidates_bayes.begin()+iclct,finalclctcandidates_bayes.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(l1->bayesprobability() >= l2->bayesprobability())
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_bayes[*it].size() == 0)
					continue;
					CFEBsplit_bayes[*it].erase(CFEBsplit_bayes[*it].begin());
				}
			}

			//just chi2

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_chi2[*it].size() == 0)
					continue;
					finalclctcandidates_chi2.push_back(CFEBsplit_chi2[*it].at(0));
				}

				sort(finalclctcandidates_chi2.begin()+iclct,finalclctcandidates_chi2.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(l1->_chi2 <= l2->_chi2)
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_chi2[*it].size() == 0)
					continue;
					CFEBsplit_chi2[*it].erase(CFEBsplit_chi2[*it].begin());
				}
			}

			//just KHS

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_KHS[*it].size() == 0)
					continue;
					finalclctcandidates_KHS.push_back(CFEBsplit_KHS[*it].at(0));
				}

				sort(finalclctcandidates_KHS.begin()+iclct,finalclctcandidates_KHS.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(c1->keyHalfStrip() <= c2->keyHalfStrip())
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_KHS[*it].size() == 0)
					continue;
					CFEBsplit_KHS[*it].erase(CFEBsplit_KHS[*it].begin());
				}
			}

			//just slope

			for(unsigned int iclct = 0; iclct < initialclctcandidates.size(); iclct++)
			{

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{
					if(CFEBsplit_slope[*it].size() == 0)
					continue;
					finalclctcandidates_slope.push_back(CFEBsplit_slope[*it].at(0));
				}

				sort(finalclctcandidates_slope.begin()+iclct,finalclctcandidates_slope.end(),
				[](CLCTCandidate* c1, CLCTCandidate* c2)
				{	
					const LUTEntry *l1 = c1->_lutEntry;
					const LUTEntry *l2 = c2->_lutEntry;

					if(!l2) return true;
					if(!l1) return false;

					if(c1->slope() <= c2->slope())
					return true;
					else
					return false;
				});

				for(auto it = CFEBpossibilities.begin(); it < CFEBpossibilities.end(); it++)
				{	
					if(CFEBsplit_slope[*it].size() == 0)
					continue;
					CFEBsplit_slope[*it].erase(CFEBsplit_slope[*it].begin());
				}
			}


			vector<int> matchedclctindices;
			vector<int> matchedclctindices_oldalgo;
			vector<int> matchedclctindices_layers;
			vector<int> matchedclctindices_PID;
			vector<int> matchedclctindices_bayes;
			vector<int> matchedclctindices_chi2;
			vector<int> matchedclctindices_KHS;
			vector<int> matchedclctindices_slope;

			//iterate through all segments in chamber

			for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
			{
				if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
				continue;

				if(segments.mu_id->at(iseg) == -1)
				continue;				

				float segmentx = segments.pos_x->at(iseg);
				float pt = muons.pt->at(segments.mu_id->at(iseg));

				if(me11a)
				{
					if(segmentx > 47) continue;
				}
				else if (me11b || me13) 
				{
					if(segmentx > 63) continue;
				} 
				else 
				{
					if(segmentx > 79) continue;
				}

				totalmuonsegments++;				

				//iterate through all CLCTs in chamber, find best match

				//LUTquality

				int closestclcttosegmentindex = -1;
				float closestclcttosegmentdistance = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates.size(); iclct++)
				{
					if(std::find(matchedclctindices.begin(),matchedclctindices.end(),iclct) != matchedclctindices.end())
					continue;

					float clctx = finalclctcandidates.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance)
					{
						closestclcttosegmentdistance = abs(clctx - segmentx);
						closestclcttosegmentindex = iclct;
					}
				}

				if(closestclcttosegmentindex != -1) //found a match
				{
					matchedclctindices.push_back(closestclcttosegmentindex);
					totalmatches++;
					matchespt->Fill(pt);
					matchedclctindex->Fill(closestclcttosegmentindex);
					if(closestclcttosegmentindex == 0)
					{
						firstclctmatches++;
						firstclctmatchespt->Fill(pt);
					}
					if(closestclcttosegmentindex == 1)
					{
						secondclctmatches++;
						secondclctmatchespt->Fill(pt);
					}
					if(closestclcttosegmentindex > 1)
					{
						otherindexclctmatches++;
						otherindexclctmatchespt->Fill(pt);
					}

				}

				//Old Algo

				int closestclcttosegmentindex_oldalgo = -1;
				float closestclcttosegmentdistance_oldalgo = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_oldalgo.size(); iclct++)
				{
					if(std::find(matchedclctindices_oldalgo.begin(), matchedclctindices_oldalgo.end(), iclct) != matchedclctindices_oldalgo.end())
					continue;

					float clctx = finalclctcandidates_oldalgo.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_oldalgo)
					{
						closestclcttosegmentdistance_oldalgo = abs(clctx - segmentx);
						closestclcttosegmentindex_oldalgo = iclct;
					}					
				}

				if(closestclcttosegmentindex_oldalgo != -1)
				{
					matchedclctindices_layers.push_back(closestclcttosegmentindex_oldalgo);

					if(closestclcttosegmentindex_oldalgo == 0)
						firstclctmatchespt_oldalgo->Fill(pt);					
				}

				//layers

				int closestclcttosegmentindex_layers = -1;
				float closestclcttosegmentdistance_layers = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_layers.size(); iclct++)
				{
					if(std::find(matchedclctindices_layers.begin(), matchedclctindices_layers.end(), iclct) != matchedclctindices_layers.end())
					continue;

					float clctx = finalclctcandidates_layers.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_layers)
					{
						closestclcttosegmentdistance_layers = abs(clctx - segmentx);
						closestclcttosegmentindex_layers = iclct;
					}					
				}

				if(closestclcttosegmentindex_layers != -1)
				{
					matchedclctindices_layers.push_back(closestclcttosegmentindex_layers);

					if(closestclcttosegmentindex_layers == 0)
						firstclctmatchespt_layers->Fill(pt);					
				}

				//PID

				int closestclcttosegmentindex_PID = -1;
				float closestclcttosegmentdistance_PID = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_PID.size(); iclct++)
				{
					if(std::find(matchedclctindices_PID.begin(), matchedclctindices_PID.end(), iclct) != matchedclctindices_PID.end())
					continue;

					float clctx = finalclctcandidates_PID.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_PID)
					{
						closestclcttosegmentdistance_PID = abs(clctx - segmentx);
						closestclcttosegmentindex_PID = iclct;
					}					
				}

				if(closestclcttosegmentindex_PID != -1)
				{
					matchedclctindices_PID.push_back(closestclcttosegmentindex_PID);

					if(closestclcttosegmentindex_PID == 0)
						firstclctmatchespt_PID->Fill(pt);
				}

				//bayes

				int closestclcttosegmentindex_bayes = -1;
				float closestclcttosegmentdistance_bayes = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_bayes.size(); iclct++)
				{
					if(std::find(matchedclctindices_bayes.begin(), matchedclctindices_bayes.end(), iclct) != matchedclctindices_bayes.end())
					continue;

					float clctx = finalclctcandidates_bayes.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_bayes)
					{
						closestclcttosegmentdistance_bayes = abs(clctx - segmentx);
						closestclcttosegmentindex_bayes = iclct;
					}					
				}

				if(closestclcttosegmentindex_bayes != -1)
				{
					matchedclctindices_bayes.push_back(closestclcttosegmentindex_bayes);

					if(closestclcttosegmentindex_bayes == 0)
						firstclctmatchespt_bayes->Fill(pt);
				}

				//chi2

				int closestclcttosegmentindex_chi2 = -1;
				float closestclcttosegmentdistance_chi2 = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_chi2.size(); iclct++)
				{
					if(std::find(matchedclctindices_chi2.begin(), matchedclctindices_chi2.end(), iclct) != matchedclctindices_chi2.end())
					continue;

					float clctx = finalclctcandidates_chi2.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_chi2)
					{
						closestclcttosegmentdistance_chi2 = abs(clctx - segmentx);
						closestclcttosegmentindex_chi2 = iclct;
					}					
				}

				if(closestclcttosegmentindex_chi2 != -1)
				{
					matchedclctindices_chi2.push_back(closestclcttosegmentindex_chi2);

					if(closestclcttosegmentindex_chi2 == 0)
						firstclctmatchespt_chi2->Fill(pt);
				}

				//KHS

				int closestclcttosegmentindex_KHS = -1;
				float closestclcttosegmentdistance_KHS = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_KHS.size(); iclct++)
				{
					if(std::find(matchedclctindices_KHS.begin(), matchedclctindices_KHS.end(), iclct) != matchedclctindices_KHS.end())
					continue;

					float clctx = finalclctcandidates_KHS.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_KHS)
					{
						closestclcttosegmentdistance_KHS = abs(clctx - segmentx);
						closestclcttosegmentindex_KHS = iclct;
					}					
				}

				if(closestclcttosegmentindex_KHS != -1)
				{
					matchedclctindices_KHS.push_back(closestclcttosegmentindex_KHS);

					if(closestclcttosegmentindex_KHS == 0)
						firstclctmatchespt_KHS->Fill(pt);
				
					
						
				}	

				//slope

				int closestclcttosegmentindex_slope = -1;
				float closestclcttosegmentdistance_slope = 1e5;

				for(int iclct = 0; iclct < (int)finalclctcandidates_slope.size(); iclct++)
				{
					if(std::find(matchedclctindices_slope.begin(), matchedclctindices_slope.end(), iclct) != matchedclctindices_slope.end())
					continue;

					float clctx = finalclctcandidates_slope.at(iclct)->keyStrip();

					if(abs(clctx - segmentx) < closestclcttosegmentdistance_slope)
					{
						closestclcttosegmentdistance_slope = abs(clctx - segmentx);
						closestclcttosegmentindex_slope = iclct;
					}					
				}

				if(closestclcttosegmentindex_slope != -1)
				{
					matchedclctindices_slope.push_back(closestclcttosegmentindex_slope);

					if(closestclcttosegmentindex_slope == 0)
						firstclctmatchespt_slope->Fill(pt);
				}			

			}			       						

        }

	}

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF)
	{
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

	effeciency1->Divide(firstclctmatchespt,matchespt);
	effeciency1_oldalgo->Divide(firstclctmatchespt_oldalgo,matchespt);
	effeciency1_layers->Divide(firstclctmatchespt_layers,matchespt);
	effeciency1_PID->Divide(firstclctmatchespt_PID,matchespt);
	effeciency1_bayes->Divide(firstclctmatchespt_bayes,matchespt);
	effeciency1_chi2->Divide(firstclctmatchespt_chi2,matchespt);
	effeciency1_KHS->Divide(firstclctmatchespt_KHS,matchespt);
	effeciency1_slope->Divide(firstclctmatchespt_slope,matchespt);
	effeciency2->Divide(secondclctmatchespt,matchespt);
	effeciency3->Divide(otherindexclctmatchespt,matchespt);

	outF->cd();
	matchespt->Write();
	firstclctmatchespt->Write();
	secondclctmatchespt->Write();
	otherindexclctmatchespt->Write();
	effeciency1->Write();
	effeciency1_oldalgo->Write();
	effeciency1_layers->Write();
	effeciency1_PID->Write();
	effeciency1_bayes->Write();
	effeciency1_chi2->Write();
	effeciency1_KHS->Write();
	effeciency1_slope->Write();
	effeciency2->Write();
	effeciency3->Write();
	matchedclctindex->Write();

	cout << totalmatches << " " << firstclctmatches << " " << secondclctmatches << " " << otherindexclctmatches << endl << endl;


	cout << "Wrote to file: " << outputfile << endl;

	return 0;
}

