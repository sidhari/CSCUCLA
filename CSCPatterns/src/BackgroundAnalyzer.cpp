/*
 * BackgroundAnalyzer.cpp
 *
 *  Created on: June 6, 2019
 *      Author: Siddharth Hariprakash
 */

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include<map>

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>

#include "../include/CSCConstants.h"
#include "../include/CSCClasses.h"
#include "../include/CSCHelperFunctions.h"
#include "../include/LUTClasses.h"

#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

#include "../include/BackgroundAnalyzer.h"

using namespace std;

int main(int argc, char* argv[]){
	BackgroundAnalyzer p;
	return p.main(argc,argv);
}

int BackgroundAnalyzer::run(string inputfile, string outputfile, int start, int end)
{

	cout << endl << "Running over file: " << inputfile << endl << endl;


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

	TFile* f_emu = TFile::Open("dat/Trees/EmulationResults_test.root");

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

    EmulatedCLCTs OPemulatedclcts(t_emu,1);
	EmulatedCLCTs NPemulatedclcts(t_emu,2);	

    //
    //COUNTERS
    //

    unsigned long long int totalsegments = 0; //total segments
    unsigned long long int totalmuonsegments = 0; //total segments associated to muons
    unsigned long long int totalnotmuonsegments = 0; //total segments not associated to muons
    unsigned long long int totalsegmentsonedgesofchambers = 0; //total segments found on the edges of chambers

    //unsigned long long int check = 0;

    unsigned long long int norechitsinchamber = 0; //if CLCT goes unmatched, check if chamber did not have rechits
    unsigned long long int nocomphitsinchamber = 0; //if segment goes unmatched, check if chamber did not have comparator hits
    unsigned long long int samesetofrechitsmultiplesegments = 0; //if segment goes unmatched. check to see if this was because of multiple segments formed from sae set of rechits

    unsigned long long int Rtotalclcts = 0; //total R CLCTs (3 Layer min)
    unsigned long long int Rtotalmatchestosegments = 0; //total R CLCT - Segments matches
    unsigned long long int Rtotalmatchestomuonsegments = 0; //total R CLCT - Muon Segment matches (signal)
    unsigned long long int Rmuonbackgroundcounter = 0; //total R CLCT - Segments not associated to muons matches
    unsigned long long int Rtotalunmatchedclcts = 0; //total unmatched R CLCTs (Other Background)
    unsigned long long int Rclctsonedgeofchamber = 0; //total R clcts found on edges of chambers, since we are skipping segments on the edges these go unmatched 
    unsigned long long int Rtotalunmatchedsegments = 0; //total unmatched segments with R CLCTs (CLCT Screw Ups)
    unsigned long long int Rtotalunmatchedmuonsegments = 0; //total unmatched Muon Segments with R CLCTs

    unsigned long long int OPtotalclcts = 0; //total OP CLCTs (3 Layer min)
    unsigned long long int OPtotalmatchestosegments = 0; //total OP CLCT - Segments matches
    unsigned long long int OPtotalmatchestomuonsegments = 0; //total OP CLCT - Muon Segment matches (signal)
    unsigned long long int OPmuonbackgroundcounter = 0; //total OP CLCT - Segments not associated to muons matches
    unsigned long long int OPtotalunmatchedclcts = 0; //total unmatched OP CLCTs (Other Background)
    unsigned long long int OPclctsonedgeofchamber = 0; //total OP clcts found on edges of chambers, since we are skipping segments on the edges these go unmatched 
    unsigned long long int OPtotalunmatchedsegments = 0; //total unmatched segments with OP CLCTs (CLCT Screw Ups)
    unsigned long long int OPtotalunmatchedmuonsegments = 0; //total unmatched Muon Segments with OP
        
    unsigned long long int NPtotalclcts = 0; //total NP CLCTs (3 Layer min)
    unsigned long long int NPtotalmatchestosegments = 0; //total NP CLCT-Segments matches
    unsigned long long int NPtotalmatchestomuonsegments = 0; //total NP CLCT - Muon Segment matches (signal)
    unsigned long long int NPmuonbackgroundcounter = 0; //total NP CLCT - Segments not associated to muons matches
    unsigned long long int NPtotalunmatchedclcts = 0; //total unmatched NP CLCTs (Other Background)
    unsigned long long int NPclctsonedgeofchamber = 0; //total NP clcts found on edges of chambers, since we are skipping segments on the edges these go unmatched 
    unsigned long long int NPtotalunmatchedsegments = 0; //total unmatched segments with NP CLCTs (CLCT Screw Ups)
    unsigned long long int NPtotalunmatchedmuonsegments = 0; //total unmatched Muon Segments with NP

    //
    //HISTOGRAMS
    //

    TH1F* OPmatchedmuonsegmentspt = new TH1F ("Matched Muon Segments (OP): Pt", "Matched Muon Segments (OP): Pt;Pt [GeV];Count", 20, 0, 100);
    TH1F* OPmatchedmuonsegmentseta = new TH1F ("Matched Muon Segments (OP): Eta", "Matched Muon Segments (OP): Eta;Eta;Count", 50, -3, 3);
    TH1F* OPmatchedmuonsegmentsphi = new TH1F ("Matched Muon Segments (OP): Phi", "Matched Muon Segments (OP): Phi;Phi;Count", 50, -5, 5);
    TH1F* OPsegmentmatchpositiondifference = new TH1F ("Matched Segment, CLCT postion difference (OP)", "Matched Segment, CLCT postion difference (OP);Position Difference;Count", 10, -5, 5);
    TH1F* OPmuonsegmentmatchpositiondifference = new TH1F ("Matched Muon Segment, CLCT postion difference (OP)", "Matched Muon Segment, CLCT postion difference (OP);Position Difference;Count", 10, -5, 5);
    TH2D* OPmuonbackgroundchambertype = new TH2D ("Muon Background: Chamber Type Distribution (OP)", "Muon Background: Chamber Type Distribution (OP);Station;Ring", 4, 1, 5, 4, 1, 5 );    
    TH2D* OPunmatchedclctschambertype = new TH2D ("Unmatched CLCTs: Chamber Type Distribution (OP)", "Unmatched CLCTs: Chamber Type (OP);Station;Ring", 4, 1, 5, 4, 1, 5);
    TH1F* OPmatchedclctslayercount = new TH1F ("Matched CLCTs: Layer Count Distribution (OP)", "Matched CLCTs: Layer Count Distribution (OP);Layer Count;Count", 7, 0, 7);
    TH1F* OPmatchedclctstomuonsegmentslayercount = new TH1F ("Matched CLCTs (to muon segments): Layer Count Distribution (OP)", "Matched CLCTs (to muon segments): Layer Count Distribution (OP);Layer Count;Count", 7, 0, 7);
    TH1F* OPunmatchedclctslayercount = new TH1F ("Unmatched CLCTs: Layer Count Distribution (OP)", "Unmatched CLCTs: Layer Count Distribution (OP);Layer Count;Count", 7, 0, 7);
    TH1F* OPmatchedclctspatternid = new TH1F ("Matched CLCTs: Pattern ID Distribution (OP)", "Matched CLCTs: Pattern ID Distribution (OP);PID;Count", 9, 2, 11);
    TH1F* OPmatchedclctstomuonsegmentspatternid = new TH1F ("Matched CLCTs (to muon segments): Pattern ID Distribution (OP)", "Matched CLCTs (to muon segments): Pattern ID Distribution (OP);PID;Count", 9, 2, 11);
    TH1F* OPunmatchedclctspatternid = new TH1F ("Unmatched CLCTs: Pattern ID Distribution (OP)", "Unmatched CLCTs: Pattern ID Distribution (OP);PID;Count", 9, 2, 11);
    TH2D* OPunmatchedsegmentschambertype = new TH2D ("Unmatched Segments: Chamber Type Distribution (OP)", "Unmatched Segments: Chamber Type Distribution (OP);Station;Ring", 4, 1, 5, 4, 1, 5);
    TH1F* OPmatchedsegmentsdxdz = new TH1F ("Matched Segments: dxdz (OP)", "Matched Segments: dx/dz (OP)", 200, -4, 4);
    TH1F* OPmatchedmuonsegmentsdxdz = new TH1F ("Matched Muon Segments: dxdz (OP)", "Matched Muon Segments: dx/dz (OP)", 200, -4, 4);
    TH1F* OPunmatchedsegmentsdxdz = new TH1F ("Unmatched Segments: dxdz (OP)", "Unmatched Segments: dx/dz (OP)", 200, -4, 4);
    TH1F* OPmatchedsegmentsdydz = new TH1F ("Matched Segments: dydz (OP)", "Matched Segments: dy/dz (OP)", 200, -4, 4);
    TH1F* OPmatchedmuonsegmentsdydz = new TH1F ("Matched Muon Segments: dydz (OP)", "Matched Muon Segments: dy/dz (OP)", 200, -4, 4);
    TH1F* OPunmatchedsegmentsdydz = new TH1F ("Unmatched Segments: dydz (OP)", "Unmatched Segments: dy/dz (OP)", 200, -4, 4);
    TH1F* OPmatchedsegmentsnhits = new TH1F ("Matched Segments: nhits (OP)", "Matched Segments: nhits (OP);nhits;Count", 7, 0, 7);
    TH1F* OPmatchedmuonsegmentsnhits = new TH1F ("Matched Muon Segments: nhits (OP)", "Matched Muon Segments: nhits (OP);nhits;Count", 7, 0, 7);
    TH1F* OPunmatchedsegmentsnhits = new TH1F ("Unmatched Segments: nhits (OP)", "Unmatched Segments: nhits (OP);nhits;Count", 7, 0, 7);

    TH1F* NPmatchedmuonsegmentspt = new TH1F ("Matched Muon Segments (NP): Pt", "Matched Muon Segments (NP): Pt;Pt [GeV];Count", 20, 0, 100);
    TH1F* NPmatchedmuonsegmentseta = new TH1F ("Matched Muon Segments (NP): Eta", "Matched Muon Segments (NP): Eta;Eta;Count", 50, -3, 3);
    TH1F* NPmatchedmuonsegmentsphi = new TH1F ("Matched Muon Segments (NP): Phi", "Matched Muon Segments (NP): Phi;Phi;Count", 50, -5, 5);
    TH1F* NPsegmentmatchpositiondifference = new TH1F ("Matched Segment, CLCT postion difference (NP)", "Matched Segment, CLCT postion difference (NP);Position Difference;Count", 10, -5, 5);
    TH1F* NPmuonsegmentmatchpositiondifference = new TH1F ("Matched Muon Segment, CLCT postion difference (NP)", "Matched Muon Segment, CLCT postion difference (NP);Position Difference;Count", 10, -5, 5);
    TH2D* NPmuonbackgroundchambertype = new TH2D ("Muon Background: Chamber Type Distribution (NP)", "Muon Background: Chamber Type Distribution (NP);Station;Ring", 4, 1, 5, 4, 1, 5 );
    TH2D* NPunmatchedclctschambertype = new TH2D ("Unmatched CLCTs: Chamber Type Distribution (NP)", "Unmatched CLCTs: Chamber Type (NP);Station;Ring", 4, 1, 5, 4, 1, 5);
    TH1F* NPmatchedclctslayercount = new TH1F ("Matched CLCTs: Layer Count Distribution (NP)", "Matched CLCTs: Layer Count Distribution (NP);Layer Count;Count", 7, 0, 7);
    TH1F* NPmatchedclctstomuonsegmentslayercount = new TH1F ("Matched CLCTs (to muon segments): Layer Count Distribution (NP)", "Matched CLCTs (to muon segments): Layer Count Distribution (NP);Layer Count;Count", 7, 0, 7);
    TH1F* NPunmatchedclctslayercount = new TH1F ("Unmatched CLCTs: Layer Count Distribution (NP)", "Unmatched CLCTs: Layer Count Distribution (NP);Layer Count;Count", 7, 0, 7);
    TH1F* NPmatchedclctspatternid = new TH1F ("Matched CLCTs: Pattern ID Distribution (NP)", "Matched CLCTs: Pattern ID Distribution (NP);PID/10;Count", 5, 6, 11);
    TH1F* NPmatchedclctstomuonsegmentspatternid = new TH1F ("Matched CLCTs (to muon segments): Pattern ID Distribution (NP)", "Matched CLCTs (to muon segments): Pattern ID Distribution (NP);PID/10;Count", 5, 6, 11);
    TH1F* NPunmatchedclctspatternid = new TH1F ("Unmatched CLCTs: Pattern ID Distribution (NP)", "Unmatched CLCTs: Pattern ID Distribution (NP);PID/10;Count", 5, 6, 11);
    TH2D* NPunmatchedsegmentschambertype = new TH2D ("Unmatched Segments: Chamber Type Distribution (NP)", "Unmatched Segments: Chamber Type Distribution (NP);Station;Ring", 4, 1, 5, 4, 1, 5);
    TH1F* NPmatchedsegmentsdxdz = new TH1F ("Matched Segments: dxdz (NP)", "Matched Segments: dx/dz (NP)", 200, -4, 4);
    TH1F* NPmatchedmuonsegmentsdxdz = new TH1F ("Matched Muon Segments: dxdz (NP)", "Matched Muon Segments: dx/dz (NP)", 200, -4, 4);
    TH1F* NPunmatchedsegmentsdxdz = new TH1F ("Unmatched Segments: dxdz (NP)", "Unmatched Segments: dx/dz (NP)", 200, -4, 4);
    TH1F* NPmatchedsegmentsdydz = new TH1F ("Matched Segments: dydz (NP)", "Matched Segments: dy/dz (NP)", 200, -4, 4);
    TH1F* NPmatchedmuonsegmentsdydz = new TH1F ("Matched Muon Segments: dydz (NP)", "Matched Muon Segments: dy/dz (NP)", 200, -4, 4);
    TH1F* NPunmatchedsegmentsdydz = new TH1F ("Unmatched Segments: dydz (NP)", "Unmatched Segments: dy/dz (NP)", 200, -4, 4);
    TH1F* NPmatchedsegmentsnhits = new TH1F ("Matched Segments: nhits (NP)", "Matched Segments: nhits (NP);nhits;Count", 7, 0, 7);
    TH1F* NPmatchedmuonsegmentsnhits = new TH1F ("Matched Muon Segments: nhits (NP)", "Matched Muon Segments: nhits (NP);nhits;Count", 7, 0, 7);
    TH1F* NPunmatchedsegmentsnhits = new TH1F ("Unmatched Segments: nhits (NP)", "Unmatched Segments: nhits (NP);nhits;Count", 7, 0, 7);    

    //
    //EVENT LOOP
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
		if(evt.RunNumber < 321710 || evt.RunNumber > 323362) continue; //correct
        //if(evt.RunNumber != 324970) continue;
		/* Era after min-separation change (10 -> 5), also includes 3 layer firmware change
		 */
		//if(evt.RunNumber <= 323362) continue;

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

			bool me11a = (ST == 1 && RI == 4);
			bool me11b = (ST == 1 && RI == 1);
			bool me13 = (ST == 1 && RI == 3);
           
            unsigned int segmentsinchamber = 0; //number of segments in current chamber
            unsigned int muonsegmentsinchamber = 0; //number of muon segments in current chamber

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                segmentsinchamber++;

                if(segments.mu_id->at(iseg) != -1)
                muonsegmentsinchamber++;
                
            }

            totalsegments += segmentsinchamber;
            totalmuonsegments += muonsegmentsinchamber;
            totalnotmuonsegments += (segmentsinchamber - muonsegmentsinchamber);

            unsigned int Rclctcountinchamber = 0; //number of R clcts in current chamber
            unsigned int OPclctcountinchamber = 0; //number of OP clcts in current chamber 
            unsigned int NPclctcountinchamber = 0; //number of NP clcts in current chamber  

            for(unsigned int iclct = 0; iclct < clcts.size(); iclct++)
            {
                if((unsigned int)clcts.ch_id->at(iclct) != chamberHash)
                continue;

                if((unsigned int)clcts.quality->at(iclct) < 3)
                continue;

                Rclctcountinchamber++;

            }          

            for(unsigned int iclct = 0; iclct < (unsigned int)OPemulatedclcts.size(); iclct++)
            {

                if(OPemulatedclcts.ch_id->at(iclct) != chamberHash)
                continue;

                if(OPemulatedclcts.layerCount->at(iclct) < 3)
                continue;

                OPclctcountinchamber++;

            }

            for(unsigned int iclct = 0; iclct < (unsigned int)NPemulatedclcts.size(); iclct++)
            {

                if(NPemulatedclcts.ch_id->at(iclct) != chamberHash)
                continue;

                if(NPemulatedclcts.layerCount->at(iclct) < 3)
                continue;

                NPclctcountinchamber++;

            }

            Rtotalclcts += Rclctcountinchamber;

            if(OPclctcountinchamber > 2)
            OPclctcountinchamber = 2;

            if(NPclctcountinchamber > 2)
            NPclctcountinchamber = 2;   

            OPtotalclcts += OPclctcountinchamber;
            NPtotalclcts += NPclctcountinchamber;
            
            vector<unsigned int> matchedRclctindex; //stores indices of R clcts matched to a segment
            vector<unsigned int> matchedOPclctindex; //stores indices of OP clcts matched to a segment
            vector<unsigned int> matchedNPclctindex; //stores indices of NP clcts matched to a segment

            vector<unsigned int> matchedRsegmentindex; //stores indices of segments matched to R clcts
            vector<unsigned int> matchedOPsegmentindex; //stores indices of segments matched to OP clcts
            vector<unsigned int> matchedNPsegmentindex; //stores indices of segments matched to NP clcts            

            //iterate through all segments in chamber

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                float segmentx = segments.pos_x->at(iseg);

               	//ignore segments at edges of chamber

                if(segmentx < 1)
                {
                    totalsegmentsonedgesofchambers++;
                    continue;
                }                

				if(me11a)
				{
					if(segmentx > 47)
                    {   
                        totalsegmentsonedgesofchambers++;
                        continue;
                    }
				}
				else if (me11b || me13) 
				{
					if(segmentx > 63)
                    {
                        totalsegmentsonedgesofchambers++;
                        continue;
                    }
				} 
				else 
				{
					if(segmentx > 79)
                    {
                        totalsegmentsonedgesofchambers++;
                        continue;
                    }
				}

                int closestRclcttosegmentindex = -1; //index of real CLCT matched to current segment
                float closestRclcttosegmentdistance = 1e5; //distance between segment and real CLCT in the event of a match

                //iterate through all real clcts in chamber

                for(unsigned int iclct = 0; iclct < clcts.size(); iclct++)
                {
                    if((unsigned int)clcts.ch_id->at(iclct) != chamberHash)
                    continue;

                    if((unsigned int)clcts.quality->at(iclct) < 3)
                    continue;

                    if(std::find(matchedRclctindex.begin(), matchedRclctindex.end(), iclct) != matchedRclctindex.end())
                    continue;

                    float clctposx = clcts.halfStrip->at(iclct) / 2. + 16*clcts.CFEB->at(iclct);
                    if(me11a) clctposx -= 64;
                    
                    if(abs(clctposx - segmentx) < closestRclcttosegmentdistance)
                    {
                        closestRclcttosegmentdistance = abs(clctposx - segmentx);
                        closestRclcttosegmentindex = iclct;
                    }

                }

                if(closestRclcttosegmentindex != -1) // found an R clct match to the segment
                {
                    matchedRclctindex.push_back(closestRclcttosegmentindex);
                    matchedRsegmentindex.push_back(iseg);

                    Rtotalmatchestosegments++;       

                    if(segments.mu_id->at(iseg) == -1) // matched to a segment not associated to a muon
                    {
                        Rmuonbackgroundcounter++;
                    }
                    else // matched to a muon's segment
                    {
                        Rtotalmatchestomuonsegments++;

                    }
                    
                }
                

                int closestOPclcttosegmentindex = -1; //index of OP CLCT matched to current segment
                float closestOPclcttosegmentdistance = 1e5; //distance between segment and OP CLCT in the event of a match

                int flag = 0;

                //iterate through all OP CLCTs in chamber

                for(unsigned int iclct = 0; iclct < OPemulatedclcts.size(); iclct++)
                {
                    if(OPemulatedclcts.ch_id->at(iclct) != chamberHash)
                    continue;

                    if(OPemulatedclcts.layerCount->at(iclct) < 3)
                    continue;

                    if(flag > 1) //first 2 clcts only
                    {
                        continue;
                    }                    
                    else
                    {
                        flag++;
                    }

                    if(std::find(matchedOPclctindex.begin(), matchedOPclctindex.end(), iclct) != matchedOPclctindex.end())
                    continue;

                    float clctposx = OPemulatedclcts.keyStrip->at(iclct);
                    
                    if(abs(clctposx - segmentx) < closestOPclcttosegmentdistance)
                    {
                        closestOPclcttosegmentdistance = abs(clctposx - segmentx);
                        closestOPclcttosegmentindex = iclct;
                    }

                }

                if(closestOPclcttosegmentindex != -1) // found an OP clct match to the segment
                {
                    matchedOPclctindex.push_back(closestOPclcttosegmentindex);
                    matchedOPsegmentindex.push_back(iseg);

                    OPtotalmatchestosegments++;

                    OPsegmentmatchpositiondifference->Fill(closestOPclcttosegmentdistance);     
                    OPmatchedclctslayercount->Fill((int)OPemulatedclcts.layerCount->at(closestOPclcttosegmentindex));
                    OPmatchedclctspatternid->Fill((int)OPemulatedclcts.patternId->at(closestOPclcttosegmentindex));       

                    OPmatchedsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                    OPmatchedsegmentsdydz->Fill(segments.dydz->at(iseg));
                    OPmatchedsegmentsnhits->Fill(segments.nHits->at(iseg));       

                    if(segments.mu_id->at(iseg) == -1) // matched to a segment not associated to a muon
                    {
                        OPmuonbackgroundcounter++;
                        OPmuonbackgroundchambertype->Fill(ST,RI);
                    }
                    else // matched to a muon's segment
                    {
                        OPtotalmatchestomuonsegments++;
                        float pt = muons.pt->at(segments.mu_id->at(iseg));
                        OPmatchedmuonsegmentspt->Fill(pt);
                        float eta = muons.eta->at(segments.mu_id->at(iseg));
                        OPmatchedmuonsegmentseta->Fill(eta);
                        float phi = muons.phi->at(segments.mu_id->at(iseg));
                        OPmatchedmuonsegmentsphi->Fill(phi);
                        OPmuonsegmentmatchpositiondifference->Fill(closestOPclcttosegmentdistance);
                        OPmatchedclctstomuonsegmentslayercount->Fill((int)OPemulatedclcts.layerCount->at(closestOPclcttosegmentindex));
                        OPmatchedclctstomuonsegmentspatternid->Fill((int)OPemulatedclcts.patternId->at(closestOPclcttosegmentindex));
                        OPmatchedmuonsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                        OPmatchedmuonsegmentsdydz->Fill(segments.dydz->at(iseg));
                        OPmatchedmuonsegmentsnhits->Fill(segments.nHits->at(iseg));  

                    }
                    
                }

                int closestNPclcttosegmentindex = -1; //index of NP CLCT matched to current segment
                float closestNPclcttosegmentdistance = 1e5; //distance between segment and NP CLCT in the event of a match

                flag = 0;

                // iterate through all NP CLCTs in chamber
                
                for(unsigned int iclct = 0; iclct < NPemulatedclcts.size(); iclct++)
                {
                    if(NPemulatedclcts.ch_id->at(iclct) != chamberHash)
                    continue;

                    if(NPemulatedclcts.layerCount->at(iclct) < 3)
                    continue;

                    if(flag > 1) //first 2 clcts only
                    {
                        continue;
                    }                    
                    else
                    {
                        flag++;
                    }

                    if(std::find(matchedNPclctindex.begin(), matchedNPclctindex.end(), iclct) != matchedNPclctindex.end())
                    continue;

                    float clctposx = NPemulatedclcts.keyStrip->at(iclct);
                    
                    if(abs(clctposx - segmentx) < closestNPclcttosegmentdistance)
                    {
                        closestNPclcttosegmentdistance = abs(clctposx - segmentx);
                        closestNPclcttosegmentindex = iclct;
                    }

                }

                if(closestNPclcttosegmentindex != -1) // found an NP clct match to the segment
                {
                    matchedNPclctindex.push_back(closestNPclcttosegmentindex);
                    matchedNPsegmentindex.push_back(iseg);

                    NPtotalmatchestosegments++;

                    NPsegmentmatchpositiondifference->Fill(closestNPclcttosegmentdistance);
                    NPmatchedclctslayercount->Fill((int)NPemulatedclcts.layerCount->at(closestNPclcttosegmentindex));
                    NPmatchedclctspatternid->Fill((int)(NPemulatedclcts.patternId->at(closestNPclcttosegmentindex)/10));

                    NPmatchedsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                    NPmatchedsegmentsdydz->Fill(segments.dydz->at(iseg));
                    NPmatchedsegmentsnhits->Fill(segments.nHits->at(iseg)); 

                    if(segments.mu_id->at(iseg) == -1) // matched to a segment not associated to a muon
                    {
                        NPmuonbackgroundcounter++;
                        NPmuonbackgroundchambertype->Fill(ST,RI);
                        
                    }
                    else // matched to a muon's segment
                    {
                        NPtotalmatchestomuonsegments++;
                        float pt = muons.pt->at(segments.mu_id->at(iseg));
                        NPmatchedmuonsegmentspt->Fill(pt);
                        float eta = muons.eta->at(segments.mu_id->at(iseg));
                        NPmatchedmuonsegmentseta->Fill(eta);
                        float phi = muons.phi->at(segments.mu_id->at(iseg));
                        NPmatchedmuonsegmentsphi->Fill(phi);
                        NPmuonsegmentmatchpositiondifference->Fill(closestOPclcttosegmentdistance);
                        NPmatchedclctstomuonsegmentslayercount->Fill((int)NPemulatedclcts.layerCount->at(closestNPclcttosegmentindex));
                        NPmatchedclctstomuonsegmentspatternid->Fill((int)(NPemulatedclcts.patternId->at(closestNPclcttosegmentindex)/10));
                        NPmatchedmuonsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                        NPmatchedmuonsegmentsdydz->Fill(segments.dydz->at(iseg));
                        NPmatchedmuonsegmentsnhits->Fill(segments.nHits->at(iseg));

                    }
                                
                }

            }

            //iterate through all R CLCTs in chamber to see how many went unmatched

            for(unsigned int iclct = 0; iclct < clcts.size(); iclct++)
            {
                if((unsigned int)clcts.ch_id->at(iclct) != chamberHash)
                continue;

                if((unsigned int)clcts.quality->at(iclct) < 3)
                continue;

                if(std::find(matchedRclctindex.begin(), matchedRclctindex.end(), iclct) != matchedRclctindex.end())
                continue;

                // if code gets here, then the CLCT went unmatched
                // check if the CLCT was on the edge

                float clctposx = clcts.halfStrip->at(iclct) / 2. + 16*clcts.CFEB->at(iclct);
                if(me11a) clctposx -= 64;

                if(clctposx < 1)
                Rclctsonedgeofchamber++;
                
                if(me11a)
				{
					if(clctposx > 47) Rclctsonedgeofchamber++;
				}
				else if (me11b || me13) 
				{
					if(clctposx > 63) Rclctsonedgeofchamber++;
				} 
				else 
				{
					if(clctposx > 79) Rclctsonedgeofchamber++;
				}

                Rtotalunmatchedclcts++;              
                
            }

            int flag = 0;

            // iterate through all OP CLCTs in chamber to see how many went unmatched

            for(unsigned int iclct = 0; iclct < OPemulatedclcts.size(); iclct++)
            {
                if(OPemulatedclcts.ch_id->at(iclct) != chamberHash)
                continue;

                if(OPemulatedclcts.layerCount->at(iclct) < 3)
                continue;

                if(flag > 1) //max 2 CLCTs
                {
                    continue;
                }
                else
                {
                    flag++;
                }

                if(std::find(matchedOPclctindex.begin(), matchedOPclctindex.end(), iclct) != matchedOPclctindex.end())
                continue;

                // if code gets here, then the CLCT went unmatched
                // check if the CLCT was on the edge

                if(OPemulatedclcts.keyStrip->at(iclct) < 1)
                OPclctsonedgeofchamber++;
                
                if(me11a)
				{
					if(OPemulatedclcts.keyStrip->at(iclct) > 47) OPclctsonedgeofchamber++;
				}
				else if (me11b || me13) 
				{
					if(OPemulatedclcts.keyStrip->at(iclct) > 63) OPclctsonedgeofchamber++;
				} 
				else 
				{
					if(OPemulatedclcts.keyStrip->at(iclct) > 79) OPclctsonedgeofchamber++;
				}

                OPtotalunmatchedclcts++;
                OPunmatchedclctslayercount->Fill(OPemulatedclcts.layerCount->at(iclct));
                OPunmatchedclctspatternid->Fill((int)OPemulatedclcts.patternId->at(iclct));           
                OPunmatchedclctschambertype->Fill(ST,RI);    
                
            }

            //iterate through all NP clcts in chamber to see how many went unmatched

            flag = 0;

            for(unsigned int iclct = 0; iclct < NPemulatedclcts.size(); iclct++)
            {
                if(NPemulatedclcts.ch_id->at(iclct) != chamberHash)
                continue;

                if(NPemulatedclcts.layerCount->at(iclct) < 3)
                continue;

                if(flag > 1) //max 2 CLCTs
                {
                    continue;
                }
                else
                {
                    flag++;
                }

                if(std::find(matchedNPclctindex.begin(), matchedNPclctindex.end(), iclct) != matchedNPclctindex.end())
                continue;

                // if code gets here, then the CLCT went unmatched
                // check if the CLCT was on the edge

                if(NPemulatedclcts.keyStrip->at(iclct) < 1)
                NPclctsonedgeofchamber++;
                
                if(me11a)
				{
					if(NPemulatedclcts.keyStrip->at(iclct) > 47) NPclctsonedgeofchamber++;
				}
				else if (me11b || me13) 
				{
					if(NPemulatedclcts.keyStrip->at(iclct) > 63) NPclctsonedgeofchamber++;
				} 
				else 
				{
					if(NPemulatedclcts.keyStrip->at(iclct) > 79) NPclctsonedgeofchamber++;
				}

                NPtotalunmatchedclcts++;                  
                NPunmatchedclctslayercount->Fill(NPemulatedclcts.layerCount->at(iclct));                
                NPunmatchedclctspatternid->Fill((int)(NPemulatedclcts.patternId->at(iclct)/10));
                NPunmatchedclctschambertype->Fill(ST,RI);

                //see if the chamber had rechits         

                
                ChamberHits rec(ST,RI,EC,CH);
                if(rec.fill(recHits))
                {
                    return -1;
                }
                if(rec.nhits() == 0 && evt.RunNumber != 321710 && evt.RunNumber != 321813 && evt.RunNumber != 321755)
                {   
                    cout << evt.RunNumber << endl << endl << evt.EventNumber << endl << endl;
                    rec.print();
                    cout << endl << endl;
                    norechitsinchamber++;
                    
                }
                
            }

            //iterate through all segments in chamber to see how many went unmatched to R CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(std::find(matchedRsegmentindex.begin(), matchedRsegmentindex.end(), iseg) != matchedRsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the segment went unmatched

                Rtotalunmatchedsegments++;          

            }

            //iterate through all segments in chamber to see how many went unmatched to OP CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(std::find(matchedOPsegmentindex.begin(), matchedOPsegmentindex.end(), iseg) != matchedOPsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the segment went unmatched

                OPtotalunmatchedsegments++;
                OPunmatchedsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                OPunmatchedsegmentsdydz->Fill(segments.dydz->at(iseg));
                OPunmatchedsegmentsnhits->Fill(segments.nHits->at(iseg));  
                OPunmatchedsegmentschambertype->Fill(ST,RI);              

            }

            //iterate through all segments in chamber to see how many went unmatched to NP CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(std::find(matchedNPsegmentindex.begin(), matchedNPsegmentindex.end(), iseg) != matchedNPsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the segment went unmatched

                NPtotalunmatchedsegments++;
                NPunmatchedsegmentsdxdz->Fill(segments.dxdz->at(iseg));
                NPunmatchedsegmentsdydz->Fill(segments.dydz->at(iseg));
                NPunmatchedsegmentsnhits->Fill(segments.nHits->at(iseg));  
                NPunmatchedsegmentschambertype->Fill(ST,RI);             

                //check to see if chamber had comp hits

                int temp = 0;
                for(unsigned int iclct1 = 0; iclct1 < N_MAX_HALF_STRIPS; iclct1++)
                {
                    for(unsigned int iclct2 = 0; iclct2 < NLAYERS; iclct2++)
                    {
                        if(compHits._hits[iclct1][iclct2])
                        temp++;
                    }
                }

                if(temp == 0) //no comphits in chamber
                {
                    nocomphitsinchamber++; 
                } 
                
                else
                {
                    samesetofrechitsmultiplesegments++;
                }
                          

                                       
                
            }

            //iterate through all muon segments in chamber to see how many went unmatched to R CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(segments.mu_id->at(iseg) == -1)
                continue;

                if(std::find(matchedRsegmentindex.begin(), matchedRsegmentindex.end(), iseg) != matchedRsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the muon segment went unmatched

                Rtotalunmatchedmuonsegments++;

            }

            //iterate through all muon segments in chamber to see how many went unmatched to OP CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(segments.mu_id->at(iseg) == -1)
                continue;

                if(std::find(matchedOPsegmentindex.begin(), matchedOPsegmentindex.end(), iseg) != matchedOPsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the muon segment went unmatched

                OPtotalunmatchedmuonsegments++;

            }

            //iterate through all muon segments in chamber to see how many went unmatched to NP CLCTs

            for(unsigned int iseg = 0; iseg < segments.size(); iseg++)
            {
                if((unsigned int)segments.ch_id->at(iseg) != chamberHash)
                continue;

                if(segments.mu_id->at(iseg) == -1)
                continue;

                if(std::find(matchedNPsegmentindex.begin(), matchedNPsegmentindex.end(), iseg) != matchedNPsegmentindex.end())
                continue;

                float segmentx = segments.pos_x->at(iseg);

                //ignore segments at edges of chamber

                if(segmentx < 1)
                continue;

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

                //if code gets here, then the muon segment went unmatched

                NPtotalunmatchedmuonsegments++;
                
            }

        }

    }

    TFile * outF = new TFile(outputfile.c_str(),"RECREATE");

	if(!outF)
    {
		cout << "Failed to open output file: " << outputfile << endl;
		return -1;
	}

    outF->cd();
    OPmatchedmuonsegmentspt->Write();
    NPmatchedmuonsegmentspt->Write();
    OPmatchedmuonsegmentseta->Write();
    NPmatchedmuonsegmentseta->Write();
    OPmatchedmuonsegmentsphi->Write();
    NPmatchedmuonsegmentsphi->Write();
    OPsegmentmatchpositiondifference->Write();
    NPsegmentmatchpositiondifference->Write();
    OPmuonsegmentmatchpositiondifference->Write();
    NPmuonsegmentmatchpositiondifference->Write();
    OPmuonbackgroundchambertype->Write();
    NPmuonbackgroundchambertype->Write();
    OPunmatchedclctschambertype->Write();
    NPunmatchedclctschambertype->Write();
    OPmatchedclctslayercount->Write();
    OPmatchedclctstomuonsegmentslayercount->Write();
    OPunmatchedclctslayercount->Write();
    NPmatchedclctslayercount->Write();
    NPmatchedclctstomuonsegmentslayercount->Write();
    NPunmatchedclctslayercount->Write();
    OPmatchedclctspatternid->Write();
    OPmatchedclctstomuonsegmentspatternid->Write();
    OPunmatchedclctspatternid->Write();
    NPmatchedclctspatternid->Write();
    NPmatchedclctstomuonsegmentspatternid->Write();
    NPunmatchedclctspatternid->Write();
    OPunmatchedsegmentschambertype->Write();
    NPunmatchedsegmentschambertype->Write();
    OPmatchedsegmentsdxdz->Write();
    OPmatchedmuonsegmentsdxdz->Write();
    OPunmatchedsegmentsdxdz->Write();
    NPmatchedsegmentsdxdz->Write();
    NPmatchedmuonsegmentsdxdz->Write();
    NPunmatchedsegmentsdxdz->Write();
    OPmatchedsegmentsdydz->Write();
    OPmatchedmuonsegmentsdydz->Write();
    OPunmatchedsegmentsdydz->Write();
    NPmatchedsegmentsdydz->Write();
    NPmatchedmuonsegmentsdydz->Write();
    NPunmatchedsegmentsdydz->Write();
    OPmatchedsegmentsnhits->Write();
    OPmatchedmuonsegmentsnhits->Write();
    OPunmatchedsegmentsnhits->Write();
    NPmatchedmuonsegmentsnhits->Write();
    NPunmatchedsegmentsnhits->Write();

    cout << "Total segments: " << totalsegments << endl;
    cout << "Total muon segments: " << totalmuonsegments << endl;
    cout << "Total segments not associated to muons: " << totalnotmuonsegments << endl;
    cout << "Total segments found on the edges of chambers: " << totalsegmentsonedgesofchambers << endl << endl;

    cout << norechitsinchamber << endl << endl;
    cout << nocomphitsinchamber << endl << endl;
    cout << samesetofrechitsmultiplesegments << endl << endl;

    cout << "Real CLCTs: " << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "Total CLCTs (3 layer min): " << Rtotalclcts << endl;
    cout << "Total segments matches: " << Rtotalmatchestosegments << endl;
    cout << "Total muon segment matches: " << Rtotalmatchestomuonsegments << endl;
    cout << "Muon background: " << Rmuonbackgroundcounter << endl;
    cout << "Total unmatched CLCTs: " << Rtotalunmatchedclcts << " (of which " << Rclctsonedgeofchamber << " were on the edges of chambers)" << endl;
    cout << "Total unmatched segments: " << Rtotalunmatchedsegments << endl;
    cout << "Total unmatched muon segments: " << Rtotalunmatchedmuonsegments << endl << endl;

    cout << "Old Patterns: " << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "Total CLCTs (3 layer min): " << OPtotalclcts << endl;
    cout << "Total segments matches: " << OPtotalmatchestosegments << endl;
    cout << "Total muon segment matches: " << OPtotalmatchestomuonsegments << endl;
    cout << "Muon background: " << OPmuonbackgroundcounter << endl;
    cout << "Total unmatched CLCTs: " << OPtotalunmatchedclcts << " (of which " << OPclctsonedgeofchamber << " were on the edges of chambers)" << endl;
    cout << "Total unmatched segments: " << OPtotalunmatchedsegments << endl;
    cout << "Total unmatched muon segments: " << OPtotalunmatchedmuonsegments << endl << endl;

    cout << "New Patterns: " << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "Total CLCTs (3 layer min): " << NPtotalclcts << endl;
    cout << "Total segments matches: " << NPtotalmatchestosegments << endl;
    cout << "Total muon segment matches: " << NPtotalmatchestomuonsegments << endl;
    cout << "Muon background: " << NPmuonbackgroundcounter << endl;
    cout << "Total unmatched CLCTs: " << NPtotalunmatchedclcts << " (of which " << NPclctsonedgeofchamber << " were on the edges of chambers)" << endl;
    cout << "Total unmatched segments: " << NPtotalunmatchedsegments << endl;
    cout << "Total unmatched muon segments: " << NPtotalunmatchedmuonsegments << endl << endl; 

    //cout << check << endl << endl;

    printf("Wrote to file: %s\n",outputfile.c_str());

	return 0;

}