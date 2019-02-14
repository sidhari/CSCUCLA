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

int BremGenPEstimator(string inputfile, string outputfile, int start=0, int end=-1) {

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
	const float DR_CUT = 0.6;

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
	// MAKE THE PDFS
	//

	const unsigned int nPBins = 16;
	const float pLow = 0;
	const float pHigh = 4000;

	PDF ECAL(nPBins, pLow,pHigh,20, 0, 20); //sim energy deposited in ecal
	PDF HCAL(nPBins, pLow,pHigh,20, 0, 5);

	map<const unsigned int*, PDF*> MuonChamberPdfs;
	for(auto cham: CHAMBER_ST_RI){
		MuonChamberPdfs[cham] = new PDF(nPBins, pLow, pHigh,20,0,1e-3);
	}

	vector<ProbabilityMap> maps;
	vector<EnergyDistributions> distributions;
	//maps.push_back(ProbabilityMap(0,100));

	float pBinSize = (pHigh-pLow)/nPBins;
	for(unsigned int i =0; i < 4000; i+=pBinSize){
		ProbabilityMap map(i, i+pBinSize);
		maps.push_back(map);
		EnergyDistributions e(i,i+pBinSize);
		distributions.push_back(e);
	}

	//ProbabilityMap highP(3000,4000);


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

		float totalEcalEnergy = 0;
		float totalHcalEnergy = 0;

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			double pt = gen.pt->at(ig);
			double phi = gen.phi->at(ig);
			double eta = gen.eta->at(ig);
			double theta = 2.*TMath::ATan(TMath::Exp(-eta));
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

		energyDeposits.setDetector("ECAL", totalEcalEnergy);
		energyDeposits.setDetector("HCAL", totalHcalEnergy);

		/*
		for(auto& map: maps){
			map.fill(genP, "ECAL", totalEcalEnergy);
			map.fill(genP, "HCAL", totalHcalEnergy);
		}
		*/

		//highP.fill(genP,"ECAL",totalEcalEnergy);
		//highP.fill(genP,"HCAL",totalHcalEnergy);

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
			//for(auto cham: CHAMBER_ST_RI){
			for(unsigned int ic=0; ic < NCHAMBERS;ic++){
				auto cham = CHAMBER_ST_RI[ic];
				if(cham[0]==ST && cham[1]==RI){
					MuonChamberPdfs[cham]->fill(genP, chamberSimHitEnergyLoss);

					energyDeposits.setDetector(CHAMBER_NAMES[ic], chamberSimHitEnergyLoss);
					/*
					for(auto& map: maps){
						map.fill(genP, CHAMBER_NAMES[ic], chamberSimHitEnergyLoss);
					}
					*/
					//highP.fill(genP, CHAMBER_NAMES[ic], chamberSimHitEnergyLoss);
				}
			}
		}
		for(auto& map: maps){
			map.fill(genP, energyDeposits);
		}
		for(auto& d : distributions){
			d.fill(genP, energyDeposits);
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

	//
	// TEMP
	//
	start = 0;
	end = 50;

	outF->cd();


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
				double pt = gen.pt->at(ig);
				double phi = gen.phi->at(ig);
				double eta = gen.eta->at(ig);
				double theta = 2.*TMath::ATan(TMath::Exp(-eta));
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

			TH1F* h_genP = new TH1F(("h_genP"+to_string(i)).c_str(), "h_genP; Gen P [GeV]; Probability", nPBins, pLow, pHigh);
			h_genP->Fill(genP);
			h_genP->Write();

			TH1D* ecalProj = ECAL.projection(totalEcalEnergy);
			TH1D* bestGuess = (TH1D*)ecalProj->Clone(("h_bestGuess"+to_string(i)).c_str());
			ecalProj->SetName(("h_ECAL"+to_string(i)).c_str());
			ecalProj->Write();
			TH1D* hcalProj = HCAL.projection(totalHcalEnergy);
			bestGuess->Multiply(hcalProj);
			hcalProj->SetName(("h_HCAL"+to_string(i)).c_str());
			hcalProj->Write();


			//
			//Iterate through all possible chambers
			//
			for(auto cham: CHAMBER_ST_RI){

				/* Since there is sometimes overlap between chambers with the same ring / station
				 * we currently pick the the chamber in the ring station combination that has the
				 * largest energy deposit. For instance
				 *
				 * ME12-1 : energy = 0.2 10^-3 [units]
				 * ME12-2 : energy = 0.5 10^-3 [units]
				 *
				 * So we pick the larger one. Eventually, want to check overlap region of 5 strips
				 * and see if one can put together shower that spans multiple chambers
				 *
				 */
				float largestEnergyDeposit = 0;
				for(int chamberHash = 0; chamberHash < (int)CSCHelper::MAX_CHAMBER_HASH; chamberHash++){
					CSCHelper::ChamberId c = CSCHelper::unserialize(chamberHash);

					unsigned int EC = c.endcap;
					unsigned int ST = c.station;
					unsigned int RI = c.ring;
					unsigned int CH = c.chamber;

					if(!CSCHelper::isValidChamber(ST,RI,CH,EC)) continue;

					if(ST==1 && RI == 4) continue; //avoid ME11A, since there is weird stuff going on there

					//only look at chamber type we are considering
					if(!(cham[0]==ST && cham[1]==RI)) continue;
					float chamberSimHitEnergyLoss = 0;
					for(unsigned int isim=0; isim < simHits.size(); isim++){
						if(simHits.ch_id->at(isim) != chamberHash) continue;
						chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);

					}

					if(chamberSimHitEnergyLoss > largestEnergyDeposit) largestEnergyDeposit = chamberSimHitEnergyLoss;

				}
				if(!largestEnergyDeposit) continue; //if no energy deposit
				TH1D* hist = MuonChamberPdfs[cham]->projection(largestEnergyDeposit);
				hist->SetName(("h_ME"+to_string(cham[0])+to_string(cham[1])+"_"+to_string(i)).c_str());
				bestGuess->Multiply(hist);
				hist->Write();
			}
			//normalize the best guess
			//double scale = bestGuess->GetXaxis()->GetBinWidth(1)/bestGuess->GetIntegral();
			Double_t scale = 1./bestGuess->Integral();
			bestGuess->Scale(scale);
			bestGuess->Write();
			/* Look at integral of bestGuess distribution,
			 * calculate momentum at which it is 90% likely to be higher than
			 *
			 * print this momentum and gen momentum, and see how often you are correct
			 *
			 */
			auto bestGuessIntegral = bestGuess->GetIntegral();

			float probThreshold = 0.1; //guess momentum at which likelihood true momentum being higher than threshold is 90%

			for(unsigned int ibin=0; ibin < bestGuess->GetNbinsX(); ibin++){
				float thisP = bestGuess->GetBinCenter(ibin);

				//cout << "p: " << thisP << " int: " << bestGuessIntegral[ibin] << endl;
				if(bestGuessIntegral[ibin] > probThreshold){
					cout << "Limit is :" << thisP << " genP = " << genP ;
					if(thisP < genP) cout << "\033[1;32m right\033[0m";
					else cout << "\033[1;31m wrong \033[0m";
					cout << endl;
					break;
				}
			}
	}

	for(auto& map : maps){
		map.print();
	}
	//highP.print();

	outF->cd();

	//write shower probability histograms
	for(unsigned int i=0; i < NDETECTORS; i++){
		const unsigned int msize = maps.size();
		float prob[msize];
		float p[msize];
		float probError[msize];
		float pError[msize];


		TH1F* num = new TH1F((DETECTOR_NAMES[i]+"_num").c_str(),"",nPBins, pLow,pHigh);
		TH1F* den = new TH1F((DETECTOR_NAMES[i]+"_den").c_str(),"",nPBins, pLow,pHigh);

		for(unsigned int im=0; im < maps.size(); im++){
			prob[im] = maps.at(im).probabilities()[i];
			p[im] = maps.at(im).midP();
			probError[im] = 0;
			pError[im] = pBinSize/2.;

			//cout << "Filling bin " << num->GetBin(maps.at(im).midP()) << " with val: " << maps.at(im).numerators()[i] << endl;

			num->SetBinContent(im+1, maps.at(im).numerators()[i]);
			den->SetBinContent(im+1, maps.at(im).denominators()[i]);
		}

		num->Write();
		den->Write();
		TGraphErrors* g = new TGraphErrors(msize, p, prob, pError, probError);
		g->SetName(DETECTOR_NAMES[i].c_str());
		g->SetTitle(DETECTOR_NAMES[i].c_str());
		g->Write();

	}

	ECAL.write(outF);
	HCAL.write(outF);


	for(auto& cham : MuonChamberPdfs) {
		cham.second->write(outF);
	}

	for(auto& d : distributions){
		auto hists = d.plots();
		for(auto& hist : hists){
			hist->Write();
		}
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
			return BremGenPEstimator(string(argv[1]), string(argv[2]));
		case 4:
			return BremGenPEstimator(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return BremGenPEstimator(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







