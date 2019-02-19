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
#include <TGraphErrors.h>

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
#include "../include/BremConstants.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;
using namespace brem;


float dr(float eta1, float phi1, float eta2, float phi2){
	float PI = 3.14159;
	float deltaEta = eta1-eta2;
	float deltaPhi = std::abs(phi1-phi2);
	if(deltaPhi > PI) deltaPhi -= 2*PI;
	return sqrt(pow(deltaEta,2)+pow(deltaPhi,2));
}



/* Builds PDF's based on energy deposited in ECAL,HCAL, Muon Chambers
 * used to localize moun energy
 */

int BremPDFBuilder(string inputfile, string outputfile, int start=0, int end=-1) {

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
	// Constants
	//

	//const float PI = 3.14159;
	//const float DR_CUT = 0.6;

	//
	// SET INPUT BRANCHES
	//

	CSCInfo::Event evt(t);
	CSCInfo::Muons muons(t);
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
	// MAKE THE PDFS
	//


	PDF ECAL("ECAL",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, ECAL_ENERGY_MAX); //sim energy deposited in ecal
	PDF HCAL("HCAL",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, HCAL_ENERGY_MAX);

	map<const unsigned int*, PDF*> MuonChamberPdfs;
	for(unsigned int im=0; im < NCHAMBERS; im++) {
		MuonChamberPdfs[CHAMBER_ST_RI[im]] = new PDF(CHAMBER_NAMES[im],NPBINS, P_LOW, P_HIGH,ENERGY_BINS,0,ME_ENERGY_MAX);
	}


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		//
		// GET GEN P
		//
		float genP = 0;
		if(gen.size() > 1){
			cout << "Error more than one gen particle. quitting..." << endl;
			continue;
		}

		float totalEcalEnergy = 0;
		float totalHcalEnergy = 0;

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			const double pt = gen.pt->at(ig);
			const double phi = gen.phi->at(ig);
			const double eta = gen.eta->at(ig);
			const double theta = 2.*TMath::ATan(TMath::Exp(-eta));
			if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);

			//find energy associated with muon in ECAL
			for(unsigned int ic = 0; ic < ebcaloHits.size(); ic++){
				float ephi =ebcaloHits.phi->at(ic);
				float eeta =ebcaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalEcalEnergy += ebcaloHits.energyEM->at(ic);
					totalEcalEnergy += ebcaloHits.energyHad->at(ic);
				}
			}

			for(unsigned int ic = 0; ic < eecaloHits.size(); ic++){
				float ephi =eecaloHits.phi->at(ic);
				float eeta =eecaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalEcalEnergy += eecaloHits.energyEM->at(ic);
					totalEcalEnergy += eecaloHits.energyHad->at(ic);
				}
			}

			for(unsigned int ic = 0; ic < escaloHits.size(); ic++){
				float ephi =escaloHits.phi->at(ic);
				float eeta =escaloHits.eta->at(ic);
				if(dr(eta,phi, eeta,ephi) < DR_CUT){
					totalEcalEnergy += escaloHits.energyEM->at(ic);
					totalEcalEnergy += escaloHits.energyHad->at(ic);
				}
			}

			//find energy associted with muon in HCAL
			for(unsigned int ic = 0; ic < hcaloHits.size(); ic++){
				totalHcalEnergy += hcaloHits.energyEM->at(ic);
				totalHcalEnergy += hcaloHits.energyHad->at(ic);
			}
		}

		ECAL.fill(genP, totalEcalEnergy);
		HCAL.fill(genP, totalHcalEnergy);


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
				chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);

			}
			if(!chamberSimHitEnergyLoss) continue;

			for(unsigned int ic=0; ic < NCHAMBERS;ic++){
				auto cham = CHAMBER_ST_RI[ic];
				if(cham[0]==ST && cham[1]==RI){
					MuonChamberPdfs[cham]->fill(genP, chamberSimHitEnergyLoss);

				}
			}
		}
	}

	//
	// Normalize the PDFs
	//
	ECAL.normalize();
	HCAL.normalize();
	for(auto& cham : MuonChamberPdfs) {
		cham.second->normalize();
	}



	outF->cd();



	ECAL.write(outF);
	HCAL.write(outF);


	for(auto& cham : MuonChamberPdfs) {
		cham.second->write(outF);
	}


	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	MuonChamberPdfs.clear();


	return 0;

}



int main(int argc, char* argv[])
{
	try {
		switch(argc){
		case 3:
			return BremPDFBuilder(string(argv[1]), string(argv[2]));
		case 4:
			return BremPDFBuilder(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return BremPDFBuilder(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







