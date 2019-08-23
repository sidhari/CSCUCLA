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


#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/LUTQualityAnalyzer.h"

using namespace std;

struct SegmentMatch 
{
	int clctIndex;
	float posOffset;
	float slopeOffset;
	float pt;
};

int main(int argc, char* argv[]){
	LUTQualityAnalyzer p;
	return p.main(argc,argv);
}

int LUTQualityAnalyzer::run(string inputfile, string outputfile, int start, int end)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    cout << endl << "Running over file: " << inputfile << endl << endl;

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

	LUT bayesLUT(string("bayes"));
    bayesLUT.loadText(string("dat/luts/linearFits.lut"));

    //
    //COUNTERS
    //

    unsigned long long int muonsegmentmatchcounter = 0;
    unsigned long long int notmuonsegmentmatchcounter = 0;

    map<LUTKey,unsigned long long int> muonsegmentmatchesperkey;
    map<LUTKey,unsigned long long int> notmuonsegmentmatchesperkey;

    for(unsigned int PID = 100; PID >59; PID-=10)
    {
        for(unsigned int CCID = 0; CCID < 4096; CCID++)
        {
            LUTKey k(PID,CCID);
            muonsegmentmatchesperkey[k] = 0;
            notmuonsegmentmatchesperkey[k] = 0; 
        }
    }

    //
	// TREE ITERATION
	//

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

    cout << "Starting Event = " << start << ", Ending Event = " << end << endl << endl;

    for(int i = start; i < end; i++)
    {
        if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n\n", 100.*(i-start)/(end-start), i-start);

        t->GetEntry(i);               

        t_emu->GetEntry(i);        

        /* First 3-layer firmware installation era on ME+1/1/11. Does not include min-CLCT-separation change (10 -> 5)
		* installed on September 12
		*/
		if(evt. RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		*/

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

            ChamberHits compHits(ST, RI, EC, CH);

			if(compHits.fill(comparators)) return -1;			             

            vector<unsigned int> matchedclctsindex; //stores indices of matched CLCTs
            vector<SegmentMatch> matchedsegmentinfo; //stores info of matched segments

            //iterate through all segments in chamber

            for(unsigned int iseg = 0; iseg < (unsigned int)segments.size(); iseg++)
            {
                if(chamberHash != (unsigned int)segments.ch_id->at(iseg))
                continue;

                float segmentdxdz;
                float pt;

                if(segments.mu_id->at(iseg) != -1)
                {
                    segmentdxdz = segments.dxdz->at(iseg);
                    pt = muons.pt->at(segments.mu_id->at(iseg));
                }            

                float segmentx = segments.pos_x->at(iseg); //segment position in strips                

                //ignore segments at edges of chambers

                //if(CSCHelper::segmentIsOnEdgeOfChamber(segmentx,ST,RI)) 
                //continue;

                //iterate through all CLCTs in chamber, see if there is a match

                int closestclcttosegmentindex = -1;
                float closestclcttosegmentdistance = 1e5;

                for(unsigned int iclct = 0; iclct < (unsigned int)emulatedclcts.size(); iclct++)
                {   
                    if(emulatedclcts.ch_id->at(iclct) != chamberHash)
                    continue;

                    if(emulatedclcts.layerCount->at(iclct) < 3)
                    continue;

                    if(std::find(matchedclctsindex.begin(), matchedclctsindex.end(), iclct) != matchedclctsindex.end())
                    continue;

                    float clctposx = emulatedclcts.keyStrip->at(iclct);

                    if(abs(clctposx - segmentx) < closestclcttosegmentdistance)
                    {
                        closestclcttosegmentdistance = abs(clctposx - segmentx);
                        closestclcttosegmentindex = iclct;
                    }

                }

                if(closestclcttosegmentindex != -1) //found a match
                {
                    if(segments.mu_id->at(iseg) == -1)
                    {
                        notmuonsegmentmatchcounter++;
                        int PID = (int)emulatedclcts.patternId->at(closestclcttosegmentindex);
                        int CCID = (int)emulatedclcts.comparatorCodeId->at(closestclcttosegmentindex);
                        LUTKey k(PID,CCID);
                        notmuonsegmentmatchesperkey[k]++;
                        //cout << "not muon " << notmuonsegmentmatchesperkey[k] << endl << endl;
                        matchedclctsindex.push_back(closestclcttosegmentindex);
                        continue;
                    }

                    muonsegmentmatchcounter++;
                    int PID = (int)emulatedclcts.patternId->at(closestclcttosegmentindex);
                    int CCID = (int)emulatedclcts.comparatorCodeId->at(closestclcttosegmentindex);
                    LUTKey k(PID,CCID);
                    muonsegmentmatchesperkey[k]++;
                    //cout << "muon " << muonsegmentmatchesperkey[k] << endl << endl;

                    float clctx = emulatedclcts.keyStrip->at(closestclcttosegmentindex);

                    SegmentMatch thismatch;
                    thismatch.clctIndex = closestclcttosegmentindex;
                    thismatch.posOffset = segmentx - clctx;
                    thismatch.slopeOffset = segmentdxdz;
                    thismatch.pt = pt;

                    matchedclctsindex.push_back(closestclcttosegmentindex);
                    matchedsegmentinfo.push_back(thismatch);

                }
            }

            for(unsigned int iclct = 0; iclct < (unsigned int)emulatedclcts.size(); iclct++)
            { 
                if(emulatedclcts.ch_id->at(iclct) != chamberHash)
                continue;

                if(emulatedclcts.layerCount->at(iclct) < 3)
                continue;

                LUTEntry* entry = 0;

                int PID = emulatedclcts.patternId->at(iclct);
                int code = emulatedclcts.comparatorCodeId->at(iclct);                

                LUTKey k(PID,code);

                if(bayesLUT.editEntry(k,entry))
                {
                    return -1;              
                }

                bool foundsegment = false;

                for(auto segmatch : matchedsegmentinfo)
                {
                    if((unsigned int)segmatch.clctIndex == iclct)
                    {
                        foundsegment = true;
                        float pt = segmatch.pt;
                        float pos = segmatch.posOffset;
                        float slope = segmatch.slopeOffset;
                        entry->addCLCT(emulatedclcts.size(chamberHash), pt, pos, slope);
                    }
                }

                if(!foundsegment)
                {                    
                    entry->addCLCT(emulatedclcts.size(chamberHash));
                }

            }

        }
                  
    }

    map<LUTKey,LUTEntry> _lut = bayesLUT.lut();    

    for(int PID = 100; PID > 59; PID-=10)
    {
        for(int CCID = 0; CCID < 4096; CCID++)
        {
            LUTKey k(PID,CCID);
            LUTEntry* e = 0;
            if(bayesLUT.editEntry(k,e))
            {
                //cout << k._pattern << " " << k._code << endl;
                //cout << muonsegmentmatchesperkey[k] << " " << notmuonsegmentmatchesperkey[k] << endl;
                //cout << "key not found" << endl << endl;
                continue;
            }

            e->calculatebayesprobability(muonsegmentmatchesperkey[k], notmuonsegmentmatchesperkey[k], muonsegmentmatchcounter, notmuonsegmentmatchcounter);
        
        }
    }

    /*for(auto&x : _lut)
    {
        LUTEntry* e = &x.second;
        auto k = x.first;
        e->calculatebayesprobability(muonsegmentmatchesperkey[k], notmuonsegmentmatchesperkey[k], muonsegmentmatchcounter, notmuonsegmentmatchcounter);        
    }*/

    //bayesLUT.sort("b");

    bayesLUT.makeFinal();

    bayesLUT.sort("lbxk");

    bayesLUT.setqual();

    //bayesLUT.writeToROOT(outputfile);

    bayesLUT.writeToText(outputfile);

    cout << "Wrote to file: " << outputfile << endl << endl;

    auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl << endl;
    
    return 0;
}

