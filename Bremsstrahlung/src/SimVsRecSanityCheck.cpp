/*
 * GenStudy.cpp
 *
 *  Created on: jan. 16 2019
 *      Author: root
 */


#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <time.h>


#include "../../CSCPatterns/include/PatternConstants.h"
#include "../../CSCPatterns/include/PatternFinderClasses.h"
#include "../../CSCPatterns/include/PatternFinderHelperFunctions.h"
#include "../../CSCPatterns/include/LUTClasses.h"
#include "../include/BremClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;


float dr(float eta1, float phi1, float eta2, float phi2){
	float PI = 3.14159;
	float deltaEta = eta1-eta2;
	float deltaPhi = std::abs(phi1-phi2);
	if(deltaPhi > PI) deltaPhi -= 2*PI;
	return sqrt(pow(deltaEta,2)+pow(deltaPhi,2));
}



/* Calculates probability of a muon given a comparator code.
 * See slides: https://indico.cern.ch/event/744948/
 */

int SimVsRecSanityCheck(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}


	//
	// SET INPUT BRANCHES
	//

	CSCInfo::Event evt(t);
	//CSCInfo::Muons muons(t);
	//CSCInfo::Segments segments(t);
	CSCInfo::RecHits recHits(t);
	//CSCInfo::LCTs lcts(t);
	//CSCInfo::CLCTs clcts(t);
	CSCInfo::Comparators comparators(t);
	CSCInfo::GenParticles gen(t);
	CSCInfo::SimHits simHits(t);
	CSCInfo::CaloHit ebcaloHits(CSCInfo::CaloHit::ecalBarrel(),t);
	CSCInfo::CaloHit eecaloHits(CSCInfo::CaloHit::ecalEndcap(),t);
	CSCInfo::CaloHit escaloHits(CSCInfo::CaloHit::ecalPreshower(),t);
	CSCInfo::CaloHit hcaloHits(CSCInfo::CaloHit::hcal(),t);
	CSCInfo::PFCandidate pfCand(t);

	//
	// MAKE THE PLOTS
	//

	TH2F* h_ecalRecVsSim = new TH2F("h_ecalRecVsSim", "h_ecalPFVsSim; Sim Energy Deposit [GeV]; PF Energy Deposit",
			100, 0,20, 100,0,20);
	TH2F* h_hcalRecVsSim = new TH2F("h_hcalRecVsSim", "h_hcalPFVsSim; Sim Energy Deposit [GeV]; PF Energy Deposit",
				100, 0, 5, 100,0,5);
	TH2F* h_cscRecVsSim = new TH2F("h_cscRecVsSim", "h_cscRecHitVsSim; Sim Energy Deposit; Rec Energy Deposit",
					100, 0, 1e-3, 100,0,80000);



	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		if(gen.size() > 1){
			cout << "Error more than one gen particle. quitting..." << endl;
			continue;
		}



		float totalSimEcalEnergy = 0;
		float totalSimHcalEnergy = 0;


		//bool everythingInDrCone = true;
		const float DR_CUT = 0.2;
		//const float DR_CUT = 200000; //dummy

		for(unsigned int ig  =0; ig < 1; ig++){

			double phi = gen.phi->at(ig);
			double eta = gen.eta->at(ig);

			//cout << "== gen phi: " << phi << "gen eta: " << eta << endl;

			for(unsigned int ic = 0; ic < ebcaloHits.size(); ic++){
				float ephi =ebcaloHits.phi->at(ic);
				float eeta =ebcaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalSimEcalEnergy += ebcaloHits.energyEM->at(ic);
					totalSimEcalEnergy += ebcaloHits.energyHad->at(ic);
				}
			}

			for(unsigned int ic = 0; ic < eecaloHits.size(); ic++){
				float ephi =eecaloHits.phi->at(ic);
				float eeta =eecaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalSimEcalEnergy += eecaloHits.energyEM->at(ic);
					totalSimEcalEnergy += eecaloHits.energyHad->at(ic);
				}
			}

			for(unsigned int ic = 0; ic < escaloHits.size(); ic++){
				float ephi =escaloHits.phi->at(ic);
				float eeta =escaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalSimEcalEnergy += escaloHits.energyEM->at(ic);
					totalSimEcalEnergy += escaloHits.energyHad->at(ic);
				}
			}



			for(unsigned int ic = 0; ic < hcaloHits.size(); ic++){
				float hphi =hcaloHits.phi->at(ic);
				float heta =hcaloHits.eta->at(ic);
				//cout << "hcal phi: " <<hphi << " heta: " << heta << "dr ="<< dr(eta,phi,heta,hphi) << endl;
				if(dr(eta,phi,heta,hphi) < DR_CUT) {
					totalSimHcalEnergy += hcaloHits.energyEM->at(ic);
					totalSimHcalEnergy += hcaloHits.energyHad->at(ic);
				}
			}

			float totalRecEcalEnergy = 0;
			float totalRecHcalEnergy = 0;

			for(unsigned int ipf=0; ipf < pfCand.size(); ipf++){
				float pphi = pfCand.phi->at(ipf);
				float peta = pfCand.eta->at(ipf);
				//cout << "pf phi: " <<pphi << " peta: " << peta << "dr ="<< dr(eta,phi,peta,pphi) << endl;
				if(dr(eta,phi,peta,pphi) < DR_CUT){
					totalRecEcalEnergy += pfCand.ecalEnergy->at(ipf);
					totalRecHcalEnergy += pfCand.hcalEnergy->at(ipf);
				}
			}

			//cout << "pfCand.size(): " << pfCand.size() << endl;
			//cout << "totalSimEcal: " << totalSimEcalEnergy << " totalSimHcal: " << totalSimHcalEnergy << endl;
			//cout << "totalRecEcal: " << totalRecEcalEnergy << " totalRecHcal: " << totalRecHcalEnergy << endl;

			h_ecalRecVsSim->Fill(totalSimEcalEnergy, totalRecEcalEnergy);
			h_hcalRecVsSim->Fill(totalSimHcalEnergy, totalRecHcalEnergy);


			//
			//Iterate through all possible chambers
			//
			for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++){
				CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

				unsigned int EC = c.endcap;
				unsigned int ST = c.station;
				unsigned int RI = c.ring;
				unsigned int CH = c.chamber;

				if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

				if(ST==1 && RI == 4) continue; //avoid ME11A, since there is weird stuff going on there

				float chamberSimHitEnergyLoss = 0;
				for(unsigned int isim=0; isim < simHits.size(); isim++){
					if(simHits.ch_id->at(isim) != chamberHash) continue;

					//float sphi = simHits.phiAtEntry->at(isim);
					//float seta = simHits.thetaAtEntry
					chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);

				}

				float chamberRecHitEnergyLoss = 0;
				for(unsigned int irh=0; irh < recHits.size(); irh++){
					if(recHits.ch_id->at(irh) != chamberHash) continue;
					chamberRecHitEnergyLoss += recHits.e->at(irh);
				}

				if(!chamberSimHitEnergyLoss && !chamberRecHitEnergyLoss) continue;
				h_cscRecVsSim->Fill(chamberSimHitEnergyLoss, chamberRecHitEnergyLoss);

			}
		}
	}


	outF->cd();

	h_ecalRecVsSim->Write();
	h_hcalRecVsSim->Write();
	h_cscRecVsSim->Write();

	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;


	return 0;

}



int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return SimVsRecSanityCheck(string(argv[1]), string(argv[2]));
		case 4:
			return SimVsRecSanityCheck(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return SimVsRecSanityCheck(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







