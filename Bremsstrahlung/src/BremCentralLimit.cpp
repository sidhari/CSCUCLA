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
#include "../include/BremHelperFunctions.h"

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



/* Calculates probability of a muon given a comparator code.
 * See slides: https://indico.cern.ch/event/744948/
 */

int BremCentralLimit(string inputfile, string outputfile, int start=0, int end=-1) {

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

	/* Looking at small range of P to look at central limit convergence of sampling
	 * for various detectors
	 */
	const float MIN_GEN_P = 2000;
	const float MAX_GEN_P = 2100;

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

	//TFile* pdfFile = TFile::Open("/uscms/home/wnash/eos/MuonGun/PDF-FullSans6.root");
	//TFile* pdfFile = TFile::Open("/uscms/home/wnash/eos/MuonGun/PDF-FullSans6.root");
	TFile* pdfFile = TFile::Open("/uscms/home/wnash/eos/MuonGun/PDF-FullLog12.root");
	if(!pdfFile){
		cout << "Error, can't load PDFs" << endl;
		return -1;
	}

	PDF ECAL("ECAL",pdfFile); //sim energy deposited in ecal
	PDF HCAL("HCAL",pdfFile);

	map<const unsigned int*, PDF*> MuonChamberPdfs;
	for(unsigned int im=0; im < NCHAMBERS; im++) {
		MuonChamberPdfs[CHAMBER_ST_RI[im]] = new PDF(CHAMBER_NAMES[im],pdfFile);
	}

	vector<ProbabilityMap> maps;
	vector<EnergyDistributions> distributions;


	float pBinSize = (P_HIGH-P_LOW)/NPBINS;
	for(unsigned int i =0; i < 4000; i+=pBinSize){
		ProbabilityMap map(i, i+pBinSize);
		maps.push_back(map);
		EnergyDistributions e(i,i+pBinSize);
		distributions.push_back(e);
	}

	//
	// Normalize the PDFs
	//

	ECAL.normalize();
	HCAL.normalize();
	for(auto& cham : MuonChamberPdfs) {
		cham.second->normalize();
	}


	//
	// Set up vectors of likelihood functions to guess momentum
	//

	vector<TH1D*> hcal_likelihoods;
	vector<TH1D*> ecal_likelihoods;

	if(end > t->GetEntries() || end < 0) end = t->GetEntries();

	printf("Starting Event = %i, Ending Event = %i\n", start, end);


	for(int i = start; i < end; i++) {
		if(!(i%1000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*(i-start)/(end-start), i-start);

		t->GetEntry(i);

		EnergyDeposits energyDeposits;

		//
		// GET GEN P
		//
		float genP = 0;
		if(gen.size() > 1){
			cout << "Error more than one gen particle. quitting..." << endl;
			continue;
		}

		//
		// RECO P
		//

		float recoP = -1;

		float totalEcalEnergy = 0;
		float totalHcalEnergy = 0;

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			double pt = gen.pt->at(ig);
			double phi = gen.phi->at(ig);
			double eta = gen.eta->at(ig);
			double theta = 2.*TMath::ATan(TMath::Exp(-eta));
			if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);


			/* ONLY LOOK AT MOMENTUMS WITH PRESCRIPTED RANGE
			 */
			if(genP < MIN_GEN_P || genP > MAX_GEN_P) continue;


			//find associated reco muon momentum
			float smallestRecoDr = 1e4;
			for(unsigned int im=0; im < muons.size(); im++){
				double reco_pt = muons.pt->at(im);
				double reco_phi = muons.phi->at(im);
				double reco_eta = muons.eta->at(im);
				double reco_theta = 2.*TMath::ATan(TMath::Exp(reco_eta));
				if(TMath::Sin(reco_theta)) {
					float thisReco_P = reco_pt/TMath::Sin(reco_theta);
					if(dr(eta,phi, reco_eta, reco_phi) < smallestRecoDr){
						smallestRecoDr = dr(eta,phi, reco_eta, reco_phi);
						recoP = thisReco_P;
					}
				}
			}

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

		if(genP < MIN_GEN_P || genP > MAX_GEN_P) continue;
		energyDeposits.setDetector("ECAL", totalEcalEnergy);
		energyDeposits.setDetector("HCAL", totalHcalEnergy);

		TH1D* hcalProj = HCAL.projection(totalHcalEnergy);
		hcal_likelihoods.push_back((TH1D*)hcalProj->Clone(("hcal_"+to_string(hcal_likelihoods.size())).c_str()));
		TH1D* ecalProj = ECAL.projection(totalEcalEnergy);
		ecal_likelihoods.push_back((TH1D*)ecalProj->Clone(("ecal_"+to_string(ecal_likelihoods.size())).c_str()));

	}

	outF->cd();

	//for(auto& hist: hcal_likelihoods) hist->Write();

	cout << "Gen Momentum: [ " << MIN_GEN_P << ", " << MAX_GEN_P << "]" << endl;
	const unsigned int max_prints = 1000;


	cout << "~~ HCAL ~~" << endl;
	const unsigned int hcalSamples = hcal_likelihoods.size() > max_prints ? max_prints : hcal_likelihoods.size();
	Double_t x[hcalSamples];
	Double_t y[hcalSamples];
	Double_t exl[hcalSamples];
	Double_t eyl[hcalSamples];
	Double_t exh[hcalSamples];
	Double_t eyh[hcalSamples];

	TH1D* hcalRunningLikelihood = 0;
	for(unsigned int i =0; i < hcalSamples; i++){
		if(!hcalRunningLikelihood) hcalRunningLikelihood = hcal_likelihoods.at(i);
		else hcalRunningLikelihood->Multiply(hcal_likelihoods.at(i));
		hcalRunningLikelihood->Scale(1./hcalRunningLikelihood->Integral());
		hcalRunningLikelihood->SetTitle(("HCAL "+to_string(i+1)+" Samples").c_str());
		hcalRunningLikelihood->GetYaxis()->SetTitle("L(p)");
		hcalRunningLikelihood->Write();

		PointEstimate est;
		if(!calculateMomentumEstimate(est, hcalRunningLikelihood)){
			return -1;
		}

		x[i] = i;
		y[i] = est.estimate();
		exl[i] = 0;
		exh[i] = 0;
		eyl[i] = est.estimate() - est.lower();
		eyh[i] = est.upper() - est.estimate();

		cout << "\tHCALs Sampled: " << i+1 << " BestGuess: " << est.estimate()
				<< " within [" << est.lower() << ", " << est.upper() << "]" << endl;
	}


	cout << "~~ ECAL ~~" << endl;
	const unsigned int ecalSamples = ecal_likelihoods.size() > max_prints ? max_prints : ecal_likelihoods.size();
	Double_t e_x[ecalSamples];
	Double_t e_y[ecalSamples];
	Double_t e_exl[ecalSamples];
	Double_t e_eyl[ecalSamples];
	Double_t e_exh[ecalSamples];
	Double_t e_eyh[ecalSamples];

	TH1D* ecalRunningLikelihood =0;
	for(unsigned int i=0; i < ecalSamples; i++) {
		if(!ecalRunningLikelihood) ecalRunningLikelihood = ecal_likelihoods.at(i);
		else ecalRunningLikelihood->Multiply(ecal_likelihoods.at(i));
		ecalRunningLikelihood->Scale(1./ecalRunningLikelihood->Integral());
		ecalRunningLikelihood->SetTitle(("ECAL "+to_string(i+1)+" Samples").c_str());
		ecalRunningLikelihood->GetYaxis()->SetTitle("L(p)");
		ecalRunningLikelihood->Write();

		PointEstimate est;
		if(!calculateMomentumEstimate(est, ecalRunningLikelihood)){
			return -1;
		}

		e_x[i] = i;
		e_y[i] = est.estimate();
		e_exl[i] = 0;
		e_exh[i] = 0;
		e_eyl[i] = est.estimate() - est.lower();
		e_eyh[i] = est.upper() - est.estimate();

		cout << "\tECALs Sampled: " << i+1 << " BestGuess: " << est.estimate()
										<< " within [" << est.lower() << ", " << est.upper() << "]" << endl;

	}

	TGraphAsymmErrors* e_estimationsByIteration = new TGraphAsymmErrors(ecalSamples,e_x,e_y,e_exl,e_exh,e_eyl,e_eyh);
	e_estimationsByIteration->SetTitle("ECAL Momentum Estimation using muons within range 2000-2100 GeV");
	e_estimationsByIteration->GetXaxis()->SetTitle("nMuons Sampled");
	e_estimationsByIteration->GetYaxis()->SetTitle("Momentum Estimation [GeV]");
	e_estimationsByIteration->Write();

	TGraphAsymmErrors* estimationsByIteration = new TGraphAsymmErrors(hcalSamples,x,y,exl,exh,eyl,eyh);
	estimationsByIteration->SetTitle("HCAL Momentum Estimation using muons within range 2000-2100 GeV");
	estimationsByIteration->GetXaxis()->SetTitle("nMuons Sampled");
	estimationsByIteration->GetYaxis()->SetTitle("Momentum Estimation [GeV]");
	estimationsByIteration->Write();


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
			return BremCentralLimit(string(argv[1]), string(argv[2]));
		case 4:
			return BremCentralLimit(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return BremCentralLimit(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
		default:
			cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
			cout << "./BremCentralLimit inputFile outputFile (events)" << endl;
			return -1;
		}
	}catch( const char* msg) {
		cerr << "ERROR: " << msg << endl;
		return -1;
	}
	return 0;
}







