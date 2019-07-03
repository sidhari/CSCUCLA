/*
 * LUTBuilder..cpp
 *
 *  Created on: June 19 2019
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
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>
#include<map>


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;

struct SegmentMatch 
{
	int clctIndex;
	float posOffset;
	float slopeOffset;
	float pt;
};

int LUTBuilder(string inputfile, string outputfile, int start = 0, int end = -1)
{
    cout << endl << "Running over file: " << inputfile << endl;

    TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

   	TFile* f_emu = TFile::Open("dat/Trees/EmulationResults.root");

	TTree* t_emu = (TTree*)f_emu->Get("EmulationResults");

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
   

	LUT bayesLUT("bayes", "dat/luts/linearFits.lut");

    //
	// TREE ITERATION
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

    cout << endl << "Starting Event = " << start << ", Ending Event = " << end << endl;

    for(int m = start; m < end; m++)
    {
        if(!(m%1000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(m-start)/(end-start), m-start);

        t->GetEntry(m);       

        t_emu->GetEntry(m);

        //
		//Iterate through all possible chambers
		//

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

			vector<CLCTCandidate*> newSetMatch;
            
            unsigned int PID;

            int CLCTcounter = 0;            

            for(unsigned int i = 0; i < emulatedclcts.size(); i++)
            {

                cout << "2" << endl << endl;

                if(chamberHash != (unsigned int)emulatedclcts.ch_id->at(i))
                continue;

                cout << "2.5" <<  endl << endl;

                if(CLCTcounter > 1)
                {
                    continue;
                }                
                else
                {
                    CLCTcounter++;
                }

               PID = emulatedclcts.patternId->at(i);

               if(PID == 100)
               {
                   CSCPattern p = newEnvelopes->at(0);
                   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
                   CLCTCandidate *c = &clct;
                   newSetMatch.push_back(c);
               }
               if(PID == 90)
               {
                   CSCPattern p = newEnvelopes->at(1);
                   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
                   CLCTCandidate *c = &clct;
                   newSetMatch.push_back(c);
               }
               if(PID == 80)
               {
                   CSCPattern p = newEnvelopes->at(2);
                   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
                   CLCTCandidate *c = &clct;
                   newSetMatch.push_back(c);
               }
               if(PID == 70)
               {
                   CSCPattern p = newEnvelopes->at(3);
                   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
                   CLCTCandidate *c = &clct;
                   newSetMatch.push_back(c);
               }
               if(PID == 60)
               {
                   CSCPattern p = newEnvelopes->at(4);
                   CLCTCandidate clct(p, emulatedclcts.comparatorCodeId->at(i), emulatedclcts._horizontalIndex->at(i), emulatedclcts._startTime->at(i));
                   CLCTCandidate *c = &clct;
                   newSetMatch.push_back(c);
               }
                
            }

            //while(newSetMatch.size() > 2)
            //newSetMatch.pop_back();

            vector<int> matchedNewId;
            vector<SegmentMatch> matchedNew;            

            for(unsigned int thisSeg = 0; thisSeg < segments.size(); thisSeg++)
            {   
                if(chamberHash != segments.ch_id->at(thisSeg))
                continue;

                if(segments.mu_id->at(thisSeg) == -1) // not a muon
                continue;

                float segmentX = segments.pos_x->at(thisSeg);
                float segmentdXdZ = segments.dxdz->at(thisSeg);
                float Pt = muons.pt->at(segments.mu_id->at(thisSeg));

                if(me11a)
				{
					if(segmentX > 47) continue;
				}
				else if (me11b || me13) 
				{
					if(segmentX > 63) continue;
				} 
				else 
				{
					if(segmentX > 79) continue;
				}

                int closestNewMatchIndex = findClosestToSegment(newSetMatch,segmentX);

                if(find(matchedNewId.begin(), matchedNewId.end(), closestNewMatchIndex) == matchedNewId.end())
                {
                    auto& clct = newSetMatch.at(closestNewMatchIndex);

                    float clctX = clct->keyStrip();

                    SegmentMatch thisMatch;
                    thisMatch.clctIndex = closestNewMatchIndex;
                    thisMatch.posOffset = segmentX - clctX;
                    thisMatch.slopeOffset = segmentdXdZ;
                    thisMatch.pt = Pt;

                    matchedNewId.push_back(closestNewMatchIndex);
                    matchedNew.push_back(thisMatch);

                }                

            }

            for(int iclct=0; iclct < (int)newSetMatch.size(); iclct++)
            {
			    auto& clct = newSetMatch.at(iclct);
			    LUTEntry* entry = 0;

			    if(bayesLUT.editEntry(clct->key(),entry))
                {
				    return -1;
			    }

		    	bool foundSegment = false;
		    	for(auto segMatch : matchedNew)
                {
				    if(segMatch.clctIndex == iclct)
                    {
				    	foundSegment = true;
				    	float pt = segMatch.pt;
				    	float pos = segMatch.posOffset;
			    		float slope = segMatch.slopeOffset;
				    	entry->addCLCT(newSetMatch.size(), pt, pos,slope);
			    	}
		    	}
		    	if(!foundSegment)
                {
			    	entry->addCLCT(newSetMatch.size());
		    	}

		    }

            newSetMatch.clear();

        }
                  
    }

    bayesLUT.print(10);

    bayesLUT.sort("lkxpscme");
    
    bayesLUT.setqual();

    bayesLUT.print(10);

    bayesLUT.writeToROOT(outputfile);

    cout << "Wrote to file: " << outputfile << endl;
    
    return 0;
}

int main(int argc, char* argv[])
{
	try 
    {
		switch(argc)
        {
		case 3:
			return LUTBuilder(string(argv[1]), string(argv[2]));
		case 4:
			return LUTBuilder(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return LUTBuilder(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./LUTBuilder inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) 
    {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}

