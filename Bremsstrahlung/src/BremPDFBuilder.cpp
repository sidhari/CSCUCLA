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

	TH1F* h_momentumDist = new TH1F("h_momentumDist", "; Gen Momentum [GeV]; Muons / 10 GeV", 400, 0, 4000);

	TH2F* h_totalEnergyLostVsGenP = new TH2F("h_totalEnergyLostVsGenP","h_totalEnergyLostVsGenP; Gen P [GeV]; dE [GeV]", NPBINS, P_LOW,P_HIGH, 120, 0,120);

	TH2F* h_chamberLossVsPLoss_31_41 = new TH2F("h_chamberLossVsPLoss_31_41","h_chamberLossVsPLoss_31_41; #Delta P_{31-41}; dE in Chamber [GeV]",100, 0, 2, 100, 0, 2e-4);

	LogBounds ncomphits(0, 2);
	TH2F* h_ncomphitsVsP_ME12 = new TH2F("h_ncomphitsVsP_ME12", "h_ncomphitsVsPME12; Gen P[GeV]; NCompHits", NPBINS, MOMENTUM_BOUNDARIES.bins(NPBINS), 20, ncomphits.bins(20));

	/*
	 *  1. GUT THIS MF'ER
	 *  2. TEST IT WORKS
	 *  3. PUT IT ON GIT
	 *
	 */



	//
	// MAKE THE PDFS
	//

/*

	//try log p binning
	const float minLogP = 0;
	const float maxLogP = 4;
	const unsigned int nLogPBins = 4*(maxLogP - minLogP);
	float logPBins[nLogPBins+1];
	//make p bins equally spaced in logP
	for(unsigned int i = 0; i <= nLogPBins; i++){
		logPBins[i] = TMath::Power(10., 1.*(maxLogP-minLogP)*i/nLogPBins + minLogP);
	}

	//temp, first iteration of log dE pdfs

	const float minLogdE = -2;
	const float maxLogdE = 3;
	const unsigned int nLogdEBins = 5*(maxLogdE - minLogdE);
	float logdEBins[nLogdEBins+1];

	for(unsigned int i =0; i <= nLogdEBins; i++){
		logdEBins[i] = TMath::Power(10., 1.*(maxLogdE-minLogdE)*i/nLogdEBins + minLogdE);
	}

	const int nTestPBins = 40;
	float pBinsTest[nTestPBins+1];
	for(unsigned int i =0; i <= nTestPBins; i++){
		pBinsTest[i] =1.* (P_HIGH-P_LOW)*i/nTestPBins + P_LOW;
	}

	PDF ECAL_log("ECAL_log", nTestPBins, pBinsTest, nLogdEBins, logdEBins);
	PDF ECAL_loglog("ECAL_loglog", nLogPBins, logPBins,nLogdEBins, logdEBins);
	PDF HCAL_log("HCAL_log", nTestPBins, pBinsTest, nLogdEBins, logdEBins);
	PDF HCAL_loglog("HCAL_loglog", nLogPBins, logPBins, nLogdEBins, logdEBins);

	PDF ECAL("ECAL",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, ECAL_ENERGY_MAX); //sim energy deposited in ecal
	PDF HCAL("HCAL",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, HCAL_ENERGY_MAX);

	PDF ECAL_low("ECAL_low",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, 1); //sim energy deposited in ecal
	PDF HCAL_low("HCAL_low",NPBINS, P_LOW,P_HIGH,ENERGY_BINS, 0, 0.1);

	float energyBins[ENERGY_BINS];
	for(unsigned int i =0; i <= ENERGY_BINS;i++){
		energyBins[i] = 0.1*(i)/ENERGY_BINS;
	}
	//TH2F* h_hcal_dEvsLogP = new TH2F("h_hcal_dEvsLogP", "h_hcal_dEvsLogP; Log(P_{G} [GeV]); dE [GeV] ",nLogPBins, 0.01,10000,ENERGY_BINS, 0, 0.1);
	TH2F* h_hcal_dEvsLogP = new TH2F("h_hcal_dEvsLogP", "h_hcal_dEvsLogP; Log(P_{G} [GeV]); dE [GeV] ",nLogPBins, logPBins, ENERGY_BINS,energyBins);

	//delete new_bins;

	map<const unsigned int*, PDF*> MuonChamberPdfs;
	map<const unsigned int*, PDF*> MuonChamberPdfs_low;
	map<const unsigned int*, PDF*> MuonChamberPdfs_low_rh;
	for(unsigned int im=0; im < NCHAMBERS; im++) {
		MuonChamberPdfs[CHAMBER_ST_RI[im]] = new PDF(CHAMBER_NAMES[im],NPBINS, P_LOW, P_HIGH,ENERGY_BINS,0,ME_ENERGY_MAX);
		MuonChamberPdfs_low[CHAMBER_ST_RI[im]] = new PDF(string(CHAMBER_NAMES[im]+"_low"),NPBINS, P_LOW, P_HIGH,ENERGY_BINS,0,5e-5);
		MuonChamberPdfs_low_rh[CHAMBER_ST_RI[im]] = new PDF(string(CHAMBER_NAMES[im]+"_low_rh"),NPBINS, P_LOW, P_HIGH,ENERGY_BINS,0,10000);
	}

*/


	//eventually remove all pdf stuff above this ^^^^

	map<string,PDF*> logPDFs;
	float pBinsTest[NPBINS+1];
		for(unsigned int i =0; i <= NPBINS; i++){
			pBinsTest[i] =1.* (P_HIGH-P_LOW)*i/NPBINS+ P_LOW;
		}

	for(unsigned int i=0; i < NDETECTORS; i++){
		logPDFs[DETECTOR_NAMES[i]] = new PDF(DETECTOR_NAMES[i],NPBINS, pBinsTest,
				ENERGY_BINS, DETECTOR_ENERGY_BOUNDARIES[i].bins(ENERGY_BINS));
	}


	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	for(int i = start; i < end; i++) {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		EnergyDeposits eSimDeps;
		EnergyDeposits eRealDeps;

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
			h_momentumDist->Fill(genP);

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
				float hphi = hcaloHits.phi->at(ic);
				float heta = hcaloHits.eta->at(ic);
				if(dr(eta,phi, heta,hphi) < DR_CUT){
					totalHcalEnergy += hcaloHits.energyEM->at(ic);
					totalHcalEnergy += hcaloHits.energyHad->at(ic);
				}
			}
		}

		eSimDeps.setDetector("ECAL",totalEcalEnergy);
		eSimDeps.setDetector("HCAL",totalHcalEnergy);



		/* Temporary, to build on if it is fruitful
		 *
		 * looking at change in muon momentum between
		 * chambers, to see how it relates to energy deposits within
		 * chambers themselves
		 */

		//float pAtME11B = -1;
		//float pAtME21 = -1;
		float pAtME31 = -1;
		float pAtME41 = -1;
		float pAtME4 = -1;
		unsigned int ncomphits_me12 = 0;

		//cout << "genP = " << genP << endl;

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

			/*
			 *  TEMP, KILL ME
			 */
			float pInChamber = -1;

			float chamberSimHitEnergyLoss = 0;
			for(unsigned int isim=0; isim < simHits.size(); isim++){
				if(simHits.ch_id->at(isim) != chamberHash) continue;
				chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);
				//REMOVE ME
				if(pInChamber < 10) pInChamber = simHits.pAtEntry->at(isim);
				if(ST == 4 && pAtME4 == -1) pAtME4 = simHits.pAtEntry->at(isim);
				if(ST == 4 && RI == 1 && pAtME41 == -1) pAtME41 = simHits.pAtEntry->at(isim);
				if(ST == 3 && RI == 1 && pAtME31 == -1) pAtME31 = simHits.pAtEntry->at(isim);


			}
			if(chamberSimHitEnergyLoss) {
				for(unsigned int ic=0; ic < NCHAMBERS;ic++){
					auto cham = CHAMBER_ST_RI[ic];
					if(cham[0]==ST && cham[1]==RI){
						eSimDeps.setDetector(CHAMBER_NAMES[ic], chamberSimHitEnergyLoss);
						/*
					MuonChamberPdfs[cham]->fill(genP, chamberSimHitEnergyLoss);
					MuonChamberPdfs_low[cham]->fill(genP, chamberSimHitEnergyLoss);
						 */

						//me21
						//cout << "\t for det: ST = " << ST << " RI = " << RI <<" energyLoss = "<< chamberSimHitEnergyLoss <<" p = "<< pInChamber << endl;
					}
				}
			}

			float chamberRecHitEnergyLoss = 0;
			for(unsigned int irh=0; irh < recHits.size(); irh++){
				if(recHits.ch_id->at(irh) != chamberHash) continue;
				chamberRecHitEnergyLoss += recHits.e->at(irh);
			}

			for(unsigned int ic=0; ic < comparators.size(); ic++){
				if(comparators.ch_id->at(ic) != chamberHash) continue;
				if(ST==1 && RI ==2) ncomphits_me12++;
			}

			if(chamberRecHitEnergyLoss){
				for(unsigned int ic=0; ic < NCHAMBERS;ic++){
					auto cham = CHAMBER_ST_RI[ic];
					if(cham[0]==ST && cham[1]==RI){
						eRealDeps.setDetector(CHAMBER_NAMES[ic], chamberRecHitEnergyLoss);
					}
				}
			}
		}
		//remove me
		if(pAtME4 != -1) h_totalEnergyLostVsGenP->Fill(genP, genP-pAtME4);
		if(pAtME41 != -1 && pAtME31 != -1) h_chamberLossVsPLoss_31_41->Fill(pAtME31 - pAtME41,eSimDeps.getDeposit("ME31")+eSimDeps.getDeposit("ME41"));
		if(ncomphits_me12) h_ncomphitsVsP_ME12->Fill(genP, ncomphits_me12);
		/*
		if(eSimDeps.getDeposit("HCAL"))h_hcal_dEvsLogP->Fill(genP, eSimDeps.getDeposit("HCAL"));
		ECAL.fill(genP, eSimDeps.getDeposit("ECAL"));
		ECAL_log.fill(genP, eSimDeps.getDeposit("ECAL"));
		ECAL_loglog.fill(genP, eSimDeps.getDeposit("ECAL"));
		ECAL_low.fill(genP, eSimDeps.getDeposit("ECAL"));
		HCAL.fill(genP, eSimDeps.getDeposit("HCAL"));
		HCAL_log.fill(genP, eSimDeps.getDeposit("HCAL"));
		HCAL_loglog.fill(genP, eSimDeps.getDeposit("HCAL"));
		HCAL_low.fill(genP, eSimDeps.getDeposit("HCAL"));
		for(unsigned int ic=0; ic < NCHAMBERS;ic++){
			auto cham = CHAMBER_ST_RI[ic];
			MuonChamberPdfs[cham]->fill(genP, eSimDeps.getDeposit(CHAMBER_NAMES[ic]));
			MuonChamberPdfs_low[cham]->fill(genP, eSimDeps.getDeposit(CHAMBER_NAMES[ic]));
			MuonChamberPdfs_low_rh[cham]->fill(genP, eRealDeps.getDeposit(CHAMBER_NAMES[ic]));
		}
*/

		//arbitrary minimum momentum threshold
		//if(genP > 10){
		for(unsigned int id=0; id < NDETECTORS; id++){
			logPDFs[DETECTOR_NAMES[id]]->fill(genP, eSimDeps.getDeposit(DETECTOR_NAMES[id]));
		}
		//}

	}



	//
	// Normalize the PDFs
	//
	/*
	ECAL.normalize();
	HCAL.normalize();
	for(auto& cham : MuonChamberPdfs) {
		cham.second->normalize();
	}
	*/


	outF->cd();



	h_momentumDist->Write();
	h_totalEnergyLostVsGenP->Write();
	h_chamberLossVsPLoss_31_41->Write();
	h_ncomphitsVsP_ME12->Write();
	makeNormalizedInYSlices(h_ncomphitsVsP_ME12)->Write();
	/*
	h_hcal_dEvsLogP->Write();
	makeNormalizedInYSlices(h_hcal_dEvsLogP)->Write();
	ECAL.write(outF);
	ECAL_log.write(outF);
	ECAL_loglog.write(outF);
	ECAL_low.write(outF);
	HCAL.write(outF);
	HCAL_log.write(outF);
	HCAL_loglog.write(outF);
	HCAL_low.write(outF);


	for(auto& cham : MuonChamberPdfs) {
		cham.second->write(outF);
	}
	for(auto& cham : MuonChamberPdfs_low) {
		cham.second->write(outF);
	}
	for(auto& cham : MuonChamberPdfs_low_rh) {
		cham.second->write(outF);
	}
*/
	for(auto& entry: logPDFs) entry.second->write(outF, "P [GeV]", "dE [GeV]");




	outF->Close();

	printf("Wrote to file: %s\n",outputfile.c_str());

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;

	//MuonChamberPdfs.clear();
	//MuonChamberPdfs_low.clear();
	logPDFs.clear();


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







