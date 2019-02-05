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


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"
#include "../include/LUTClasses.h"

//using soft-links, if it doesn't work, is in ../../CSCDigiTuples/include/<name>
#include "../include/CSCInfo.h"
#include "../include/CSCHelper.h"

using namespace std;

//makes a histogram with normalized slices in y
TH2F* makeNormalizedInYSlices(const TH2F* hist){
	TH2F* normalized = (TH2F*)hist->Clone((string(hist->GetName())+"_normY").c_str());
	for(int i=0; i < normalized->GetNbinsX()+1; i++){
		float norm = 0;
		//calculate integral along p bin
		for(int j=0;j < normalized->GetNbinsY()+1; j++){
			norm += normalized->GetBinContent(i,j);
		}
		//normalize everything for a given p
		if(norm){
			for(int j=0;j < normalized->GetNbinsY()+1; j++){
				float content = normalized->GetBinContent(i,j);
				normalized->SetBinContent(i,j,content/norm);
				//norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
			}
		}
	}
	return normalized;

}

TH2F* makeNormalizedInXSlices(const TH2F* hist){
	TH2F* normalized = (TH2F*)hist->Clone((string(hist->GetName())+"_normX").c_str());
	for(int i =0; i < normalized->GetNbinsY()+1; i++){
		float norm =0;
		for(int j=0;j < normalized->GetNbinsX()+1; j++){
			norm += normalized->GetBinContent(j,i);
		}
		if(norm) {
			for(int j=0; j < normalized->GetNbinsX()+1;j++){
				float content = normalized->GetBinContent(j,i);
				normalized->SetBinContent(j,i,content/norm);
			}
		}
	}
	/*
	for(int i=0; i < normalized->GetNbinsY()+1; i++){
		float norm = 0;
		//calculate integral along p bin
		for(int j=0;j < normalized->GetNbinsX()+1; j++){
			norm += normalized->GetBinContent(j,i);
		}
		//normalize everything for a given p
		if(norm){
			for(int j=0;j < normalized->GetNbinsX()+1; j++){
				float content = normalized->GetBinContent(i,j);
				normalized->SetBinContent(j,i,content/norm);
				//norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
			}
		}
	}
	*/
	return normalized;
}


/* Calculates probability of a muon given a comparator code.
 * See slides: https://indico.cern.ch/event/744948/
 */

int GenStudy(string inputfile, string outputfile, int start=0, int end=-1) {

	//TODO: change everythign printf -> cout
	auto t1 = std::chrono::high_resolution_clock::now();

	printf("Running over file: %s\n", inputfile.c_str());


	TFile* f = TFile::Open(inputfile.c_str());
	if(!f) throw "Can't open file";

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t) throw "Can't find tree";

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
	CSCInfo::CaloHit ecaloHits(CSCInfo::CaloHit::ecal(),t);
	CSCInfo::CaloHit hcaloHits(CSCInfo::CaloHit::hcal(),t);
	CSCInfo::PFCandidate pfCand(t);

	//
	// MAKE ALL THE PATTERNS
	//


	//vector<CSCPattern>* newPatterns = createNewPatterns();

	const unsigned int nPBins = 10;
	const float pLow = 0;
	const float pHigh = 4000;

	map<int, TH2F*> h_compHitsPerChamberVsGenP_byChamber;
	map<int, TH2F*> h_rhEnergyPerChamberVsGenP_byChamber;
	for(int chamberCount =  1; chamberCount < 7; chamberCount++){
		h_compHitsPerChamberVsGenP_byChamber[chamberCount] = new TH2F(("h_compHitsPerChamberVsGenP_"+to_string(chamberCount)).c_str(),
				("h_compHitsPerChamberVsGenP"+to_string(chamberCount) +"; Gen P [GeV]; Comparator Hits / Chamber").c_str(),nPBins,pLow,pHigh, 100, 0, 100);
		h_rhEnergyPerChamberVsGenP_byChamber[chamberCount] = new TH2F(("h_rhEnergyPerChamberVsGenP_"+to_string(chamberCount)).c_str(),
						("h_rhEnergyPerChamberVsGenP"+to_string(chamberCount) +"; Gen P [GeV]; RH Energy / Chamber").c_str(),nPBins,pLow,pHigh, 50, 0, 100);

	}

	const unsigned int resBins = 100;
	const float resLow = -4000;
	const float resHigh = 4000;
	TH1F* h_pResolution = new TH1F("h_pResolution", "h_pResolution; #Delta P [GeV]; Gen Muons", resBins, resLow, resHigh);

	TH1F* h_demoLikelihood = new TH1F("h_demoLikelihood_gen3301", "h_demoLikelihood_gen3301; P [GeV]; ~ Probability", nPBins, pLow, pHigh);
	TH1F* h_demoLikelihood2 = new TH1F("h_demoLikelihood_gen2810", "h_demoLikelihood_gen2810; P [GeV]; ~ Probability", nPBins, pLow, pHigh);


	TH1F* h_p_est_0_1000GeV = new TH1F("h_p_est_0_1000GeV", "h_p_est_0-1000GeV; P Est [GeV]; Gen Muons", 4, pLow,pHigh);
	TH1F* h_p_est_1000_2000GeV = new TH1F("h_p_est_1000_2000GeV", "h_p_est_1000-2000GeV; P Est [GeV]; Gen Muons", 4, pLow,pHigh);
	TH1F* h_p_est_2000_3000GeV = new TH1F("h_p_est_2000_3000GeV", "h_p_est_2000-3000GeV; P Est [GeV]; Gen Muons", 4, pLow,pHigh);
	TH1F* h_p_est_3000_4000GeV = new TH1F("h_p_est_3000_4000GeV", "h_p_est_3000-4000GeV; P Est [GeV]; Gen Muons", 4, pLow,pHigh);

	TH1F* h_compHitsMinusRecHits = new TH1F("h_compHitsMinusRecHits", "h_compHitsMinusRecHits; CompHits - RecHits; Chambers", 100, -50,50);
	TH2F* h_compHitsVsRecHits = new TH2F("h_compHitsVsRecHits", "h_compHitsVsRecHits; RecHits; CompHits", 50, 0, 50, 50, 0, 50);

	TH2F* h_simHitsVsRecHits = new TH2F("h_simHitEnergyLossVsRecHits","h_simHitEnergyLossVsRecHits; RecHits In Chamber; Total Sim Hit Energy Loss In Chamber"
			,50, 0,50,100, 0, 1e-3);
	TH2F* h_simHitsVsRecHitsADC = new TH2F("h_simHitEnergyLossVsRecHitADC","h_simHitEnergyLossVsRecHitADC; RecHitADC In Chamber; Total Sim Hit Energy Loss In Chamber"
				,50, 0,60000,50, 0, 1e-3);
	TH2F* h_energyPerChamberVsP = new TH2F("h_energyPerChamberVsP","h_energyPerChamberVsP; P [GeV]; Energy Per Chamber [?]", 20,0.,4000., 100, 0, 50000);
	TH2F* h_simHitEnergyPerChamberVsP = new TH2F("h_simHitEnergyPerChamberVsP","h_simHitEnergyPerChamberVsP; P [GeV]; SimHitEnergyPerChamber",
			nPBins, pLow, pHigh, 50,0,1e-3);

	TH2F* h_secondaryCalEnergyVsP = new TH2F("h_secondaryCalEnergyVsP", "h_secondayCalEnergyVsP; P[GeV]; Total Cal Energy (dR < 0.3) [?]",
			nPBins, pLow, pHigh, 40, 0,40);
	TH2F* h_totalCalEnergyVsP = new TH2F("h_totalCalEnergyVsP", "h_totalCalEnergyVsP; P[GeV]; Total Cal Energy (dR< 0.3) [?]",
			nPBins, pLow, pHigh, 40, 0,40);

	TH2F* h_totalECALEnergyVsP = new TH2F("h_totalECALEnergyVsP", "h_totalECALEnergyVsP; P[GeV]; Total ECAL Energy (dR< 0.3) [?]",
			nPBins, pLow, pHigh, 40, 0,40);
	TH2F* h_totalHCALEnergyVsP = new TH2F("h_totalHCALEnergyVsP", "h_totalHCALEnergyVsP; P[GeV]; Total HCAL Energy (dR< 0.3) [?]",
			nPBins, pLow, pHigh, 10, 0,5);

	TH2F* h_totalSimECALEnergyVsP = new TH2F("h_totalSimECALEnergyVsP", "h_totalSimECALEnergyVsP; P[GeV]; Total ECAL Energy (dR< 0.3) [?]",
			nPBins, pLow, pHigh, 20, 0,10);
	TH2F* h_totalSimHCALEnergyVsP = new TH2F("h_totalSimHCALEnergyVsP", "h_totalSimHCALEnergyVsP; P[GeV]; Total HCAL Energy (dR< 0.3) [?]",
			nPBins, pLow, pHigh, 20, 0,5);


	//start with 4 for now
	map<float,TH1F*> h_pResolutions;
	for(int i =0; i < h_compHitsPerChamberVsGenP_byChamber[4]->GetNbinsX(); i++){
		float p = h_compHitsPerChamberVsGenP_byChamber[4]->GetXaxis()->GetBinLowEdge(i);
		h_pResolutions[p] = new TH1F(("h_pResolution_"+to_string((int)p)).c_str(),
				("h_pResolution_"+to_string((int)p)+"; Gen - Guess P [GeV]; Gen Muons").c_str(), resBins, resLow, resHigh);
	}

	TFile * outF = new TFile(outputfile.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", outputfile.c_str());
		return -1;
	}

	ofstream highEnergyBrem("3TeV_20-25compHits_ME21.txt", ios::trunc);
	ofstream lowEnergyBrem("0TeV_20-25compHits_ME21.txt",ios::trunc);


	vector<bool> p_showers_lowPt [NCHAMBERS];
	vector<bool> p_showers_highPt [NCHAMBERS];
	vector<bool> p_showers_correlation_lowPt [NCHAMBERS][NCHAMBERS];
	vector<bool> p_showers_correlation_highPt [NCHAMBERS][NCHAMBERS];


	//
	// SHOWERING THRESHOLD, ALL CHAMBERS WITH >= THIS AMOUNT ARE CALLED "SHOWERING"
	//
	const unsigned int SHOWER_THRESHOLD = 10;
	const float PI = 3.14159;
	const float DR_CUT = 0.3;

	if(highEnergyBrem.is_open() && lowEnergyBrem.is_open()){
	//if(highEnergyBrem.is_open()){
		//highEnergyBrem << "test\n";
		//highEnergyBrem.close();

		//
		// TREE ITERATION
		//


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

			float totalSecondaryEcalEnergy = 0;
			float totalSecondaryHcalEnergy = 0;
			float totalEcalEnergy = 0;
			float totalHcalEnergy = 0;

			float totalECal_emEnergy = 0;
			float totalECal_hEnergy = 0;
			float totalHCal_emEnergy = 0;
			float totalHCal_hEnergy = 0;

			float totalOtherParticleEnergy = 0;
			for(unsigned int ig  =0; ig < gen.size(); ig++){
				double pt = gen.pt->at(ig);
				double phi = gen.phi->at(ig);
				double eta = gen.eta->at(ig);
				double theta = 2.*TMath::ATan(TMath::Exp(-eta));
				if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);
				//cout << "== Gen ID: " << gen.pdg_id->at(ig) << " p = " << genP<<  " pfCands: " << pfCand.size() << endl;
				//iterate through pf candidates and add up energy in ecal / hcal
				for(unsigned int ipf =0; ipf < pfCand.size(); ipf++){

						//cout << "Found photon or neutral hadron. Pdg_id = " << pfCand.pdg_id->at(ipf) << endl;
						float deltaEta = pfCand.eta->at(ipf) - eta;
						float pfPhi = pfCand.phi->at(ipf);
						float deltaPhi = std::abs(pfPhi - phi);
						if(deltaPhi > PI) deltaPhi -=2*PI;
						float dr = sqrt(pow(deltaEta,2) + pow(deltaPhi,2));
						//cout << "dR = " << dr << endl;
						if(dr < DR_CUT){
							totalEcalEnergy += pfCand.ecalEnergy->at(ipf);
							totalHcalEnergy += pfCand.hcalEnergy->at(ipf);
							//4 == photon, 5 == neutral hadron
							if(pfCand.particleId->at(ipf) == 4 || pfCand.particleId->at(ipf) == 5){
								totalSecondaryEcalEnergy += pfCand.ecalEnergy->at(ipf);
								totalSecondaryHcalEnergy += pfCand.hcalEnergy->at(ipf) + pfCand.h0Energy->at(ipf);
							}
							else {
								float calEnergy = pfCand.ecalEnergy->at(ipf) + pfCand.hcalEnergy->at(ipf) + pfCand.h0Energy->at(ipf);
								if(calEnergy) {
									//cout << "Found calEnegy in dR < CUT: " << calEnergy << " associated with pdg_id : " << pfCand.pdg_id->at(ipf) << endl;
									totalOtherParticleEnergy += calEnergy;
								}
						}
					}
				}
				//cout << "== genEta = " << eta << " genPhi = " << phi << endl;
				for(unsigned int ic = 0; ic < ecaloHits.size(); ic++){
					//float eEta = ecaloHits.eta->at(ic);
					//float ePhi = ecaloHits.phi->at(ic);
					//cout << "ecalEta = " << eEta << " ecalPhi = " << ePhi << endl;
					totalECal_emEnergy += ecaloHits.energyEM->at(ic);
					totalECal_hEnergy += ecaloHits.energyHad->at(ic);
				}
				for(unsigned int ic = 0; ic < hcaloHits.size(); ic++){
					//float eEta = hcaloHits.eta->at(ic);
					//float ePhi = hcaloHits.phi->at(ic);
					totalHCal_emEnergy += hcaloHits.energyEM->at(ic);
					totalHCal_hEnergy += hcaloHits.energyHad->at(ic);
					//cout << "hcalEta = " << eEta << " hcalPhi = " << ePhi << endl;
				}
			}

			//cout << "genP = " << genP << endl;
			//cout << "totalECal - ecalEnergy: " << totalECal_emEnergy << " hcalEnergy: " << totalECal_hEnergy << endl;
			//cout << "totalHCal - ecalEnergy: " << totalHCal_emEnergy << " hcalEnergy: " << totalHCal_hEnergy << endl;
			//cout << "totalEcal = " << totalEcalEnergy << endl;
			//cout << "totalHcal = " << totalHcalEnergy << endl;
			//cout << "totalOtherEnergy = " << totalOtherParticleEnergy << endl;
			h_secondaryCalEnergyVsP->Fill(genP, totalSecondaryEcalEnergy+totalSecondaryHcalEnergy);
			h_totalCalEnergyVsP->Fill(genP, totalSecondaryEcalEnergy+totalSecondaryHcalEnergy+totalOtherParticleEnergy);

			h_totalECALEnergyVsP->Fill(genP, totalEcalEnergy);
			h_totalHCALEnergyVsP->Fill(genP, totalHcalEnergy);

			h_totalSimECALEnergyVsP->Fill(genP, totalECal_emEnergy+totalECal_hEnergy);
			h_totalSimHCALEnergyVsP->Fill(genP, totalHCal_emEnergy+totalHCal_hEnergy);

			//
			// LOOK AT AL THE CHAMBERS
			//

			int nChambersWithHits = 0;
			int nCompHits = 0;
			float totalRecHitRecordedEnergy = 0;
			float totalSimHitRecordedEnergy = 0;
			vector<int> compHitsPerChamber;
			bool chambersShowered [NCHAMBERS];
			for(unsigned int ich = 0; ich < NCHAMBERS; ich++){
				chambersShowered[ich] = false;
			}
			bool eventHasShower = false;

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
				//
				// Emulate the TMB to find all the CLCTs
				//

				ChamberHits chamberCompHits(ST, RI, EC, CH);

				if(chamberCompHits.fill(comparators)) return -1;

				ChamberHits chamberRecHits(ST,RI,EC,CH);

				if(chamberRecHits.fill(recHits)) return -1;

				if(!chamberCompHits.nhits() && !chamberRecHits.nhits()) continue;

				//float totalChamberEnergyLoss = 0;
				float chamberSimHitEnergyLoss = 0;
				for(unsigned int isim =0; isim < simHits.size(); isim++){
					if(simHits.ch_id->at(isim) != chamberHash) continue;
					chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);
					//totalChamberEnergyLoss += simHits.energyLoss->at(isim);
					//cout <<"simEnergyLoss = " << simHits.energyLoss->at(isim) << endl;
				}
				h_simHitsVsRecHits->Fill(chamberRecHits.nhits(),chamberSimHitEnergyLoss);
				float chamberRecHitEnergyLoss = 0;
				for(unsigned int irh=0; irh < recHits.size(); irh++){
					if(recHits.ch_id->at(irh) != chamberHash) continue;
					chamberRecHitEnergyLoss += recHits.e->at(irh);
				}
				h_simHitsVsRecHitsADC->Fill(chamberRecHitEnergyLoss, chamberSimHitEnergyLoss);
				totalSimHitRecordedEnergy += chamberSimHitEnergyLoss;
				totalRecHitRecordedEnergy += chamberRecHitEnergyLoss;


				h_compHitsMinusRecHits->Fill((int)chamberCompHits.nhits() - (int)chamberRecHits.nhits());
				h_compHitsVsRecHits->Fill(chamberRecHits.nhits(),chamberCompHits.nhits());

				/*
				if(ST == 1 && RI == 4){
					cout << "rechits = " << chamberRecHits.nhits() << " comphits = " << chamberCompHits.nhits() << endl;
					chamberRecHits.print();

					chamberCompHits.print();
				}
				*/
				if(!chamberCompHits.nhits()) continue;

				nChambersWithHits++;
				nCompHits += chamberCompHits.nhits();
				compHitsPerChamber.push_back(chamberCompHits.nhits());

				if(chamberCompHits.nhits() >= SHOWER_THRESHOLD){
					eventHasShower = true;
					unsigned int ch_array_index = -1;
					for(unsigned int ich =0; ich < NCHAMBERS; ich++){
						if(ST == CHAMBER_ST_RI[ich][0] && RI == CHAMBER_ST_RI[ich][1]){
							ch_array_index = ich;
							break;
						}
					}
					chambersShowered[ch_array_index] = true;
				}


				if(ST == 2 && RI == 1 && CH == 10){
					if(chamberCompHits.nhits() > 10 && chamberCompHits.nhits() < 15) {
						if(genP < 500){
							lowEnergyBrem << "-- Gen P = " << genP << " --"<< endl;
							lowEnergyBrem << chamberCompHits;
						} else if( genP > 3500){
							highEnergyBrem << "-- Gen P = " << genP << " --\n";
							highEnergyBrem << chamberCompHits;
						}
						/*
					if(genP < 500 || genP > 3500){

						cout << "-- Gen P = " << genP << " --"<< endl;
						chamberCompHits.print();
					}
						 */
					}
				}
			} //end chamber loop
			if(nChambersWithHits){
				float averageRHEnergyLoss = totalRecHitRecordedEnergy / nChambersWithHits;
				h_energyPerChamberVsP->Fill(genP,averageRHEnergyLoss);
				float averageSHEnergyLoss = totalSimHitRecordedEnergy / nChambersWithHits;
				h_simHitEnergyPerChamberVsP->Fill(genP, averageSHEnergyLoss);
			}



			if(eventHasShower){
				for(unsigned int ich =0; ich < NCHAMBERS; ich++){
					if(genP < 500){
						p_showers_lowPt[ich].push_back(chambersShowered[ich]);
					}else if(genP > 3500){
						p_showers_highPt[ich].push_back(chambersShowered[ich]);
					}
					if(chambersShowered[ich]){
						for(unsigned int iich=0; iich < NCHAMBERS; iich++){
							if(genP < 500){
								p_showers_correlation_lowPt[ich][iich].push_back(chambersShowered[iich]);
							} else if (genP > 3500){
								p_showers_correlation_highPt[ich][iich].push_back(chambersShowered[iich]);
							}
						}
					}
				}
			}

			if(nChambersWithHits && nChambersWithHits < 7){
				for(auto& hits: compHitsPerChamber){
					h_compHitsPerChamberVsGenP_byChamber[nChambersWithHits]->Fill(genP, hits);
				}
			}


		}
		//TEMP
		highEnergyBrem.close();
		lowEnergyBrem.close();
	}

	TH2F* h_simHitEnergyPerChamberVsP_normalized = (TH2F*)h_simHitEnergyPerChamberVsP->Clone("h_simHitEnergyPerChamberVsP_norm");
	for(int i=0; i < h_simHitEnergyPerChamberVsP_normalized->GetNbinsX()+1; i++){
		float norm = 0;
		for(int j =0; j < h_simHitEnergyPerChamberVsP_normalized->GetNbinsY()+1; j++){
			norm += h_simHitEnergyPerChamberVsP_normalized->GetBinContent(i,j);
		}
		if(norm){
			for(int j=0; j < h_simHitEnergyPerChamberVsP_normalized->GetNbinsY()+1; j++){
				float content = h_simHitEnergyPerChamberVsP_normalized->GetBinContent(i,j);
				h_simHitEnergyPerChamberVsP_normalized->SetBinContent(i,j,content/norm);
			}
		}
	}



	map<int, TH2F*> h_compHitsPerChamberVsGenP_byChamber_normalized;
	for(const auto& entry: h_compHitsPerChamberVsGenP_byChamber){

		TH2F* h_compHitsPerChamberVsGenP_byChamber_norm = (TH2F*)entry.second->Clone(("h_compHitsPerChamberVsGenP_byChamber_norm_"+to_string(entry.first)).c_str());
		for(int i =0; i < h_compHitsPerChamberVsGenP_byChamber_norm->GetNbinsX()+1; i++){
			float norm = 0;
			//calculate integral along pt bin
			for(int j=0;j < h_compHitsPerChamberVsGenP_byChamber_norm->GetNbinsY()+1; j++){
				norm += h_compHitsPerChamberVsGenP_byChamber_norm->GetBinContent(i,j);
			}
			//normalize everything for a given pt
			if(norm){
				for(int j=0;j < h_compHitsPerChamberVsGenP_byChamber_norm->GetNbinsY()+1; j++){
					float content = h_compHitsPerChamberVsGenP_byChamber_norm->GetBinContent(i,j);
					h_compHitsPerChamberVsGenP_byChamber_norm->SetBinContent(i,j,content/norm);
				}
			}
		}

		h_compHitsPerChamberVsGenP_byChamber_normalized[entry.first] = h_compHitsPerChamberVsGenP_byChamber_norm;
	}

	TH2F* h_totalECALEnergyVsP_norm = makeNormalizedInYSlices(h_totalECALEnergyVsP);
	TH2F* h_totalHCALEnergyVsP_norm = makeNormalizedInYSlices(h_totalHCALEnergyVsP);

	//
	// Reiterate through tree
	//

	printf("-- Reiterating through tree! --\n");
	printf("Starting Event = %i, Ending Event = %i\n", start, end);

	bool foundDemo = false;
	bool foundDemo2 = false;
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

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			//cout << "Gen ID: " << gen.pdg_id->at(ig) << " pt = " << gen.pt->at(ig) << endl;
			double pt = gen.pt->at(ig);
			double eta = gen.eta->at(ig);
			double theta = 2.*TMath::ATan(TMath::Exp(-eta));
			if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);
		}

		//
		// LOOK AT AL THE CHAMBERS
		//

		int nChambersWithHits = 0;
		int nCompHits = 0;
		vector<int> compHitsPerChamber;
		vector<float> simHitLossPerChamber;

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
			//
			// Emulate the TMB to find all the CLCTs
			//

			ChamberHits chamberCompHits(ST, RI, EC, CH);

			if(chamberCompHits.fill(comparators)) return -1;

			if(!chamberCompHits.nhits()) continue;

			nChambersWithHits++;
			nCompHits += chamberCompHits.nhits();
			compHitsPerChamber.push_back(chamberCompHits.nhits());

			float chamberSimHitEnergyLoss = 0;
			for(unsigned int isim =0; isim < simHits.size(); isim++){
				if(simHits.ch_id->at(isim) != chamberHash) continue;
				chamberSimHitEnergyLoss += simHits.energyLoss->at(isim);
			}
			simHitLossPerChamber.push_back(chamberSimHitEnergyLoss);
		}


		//
		// Calculate ECAL and HCAL energy
		//
		float totalEcalEnergy = 0;
		float totalHcalEnergy = 0;

		for(unsigned int ig  =0; ig < gen.size(); ig++){
			double pt = gen.pt->at(ig);
			double phi = gen.phi->at(ig);
			double eta = gen.eta->at(ig);
			double theta = 2.*TMath::ATan(TMath::Exp(-eta));
			if(TMath::Sin(theta)) genP = pt/TMath::Sin(theta);
			//cout << "== Gen ID: " << gen.pdg_id->at(ig) << " p = " << genP<<  " pfCands: " << pfCand.size() << endl;
			//iterate through pf candidates and add up energy in ecal / hcal
			for(unsigned int ipf =0; ipf < pfCand.size(); ipf++){

				//cout << "Found photon or neutral hadron. Pdg_id = " << pfCand.pdg_id->at(ipf) << endl;
				float deltaEta = pfCand.eta->at(ipf) - eta;
				float pfPhi = pfCand.phi->at(ipf);
				float deltaPhi = std::abs(pfPhi - phi);
				if(deltaPhi > PI) deltaPhi -=2*PI;
				float dr = sqrt(pow(deltaEta,2) + pow(deltaPhi,2));
				//cout << "dR = " << dr << endl;
				if(dr < DR_CUT){
					totalEcalEnergy += pfCand.ecalEnergy->at(ipf);
					totalHcalEnergy += pfCand.hcalEnergy->at(ipf);
				}
			}
		}

		//do this to start with
		if(genP < 4000){
			/* Using number of comparator hits to guess energy
			if(nChambersWithHits == 4){
				TH2F* h_compHitsPerChamberVsGenP_byChamber_norm = h_compHitsPerChamberVsGenP_byChamber_normalized[4];
				float highest_prob = 0;
				float best_p_guess = -1;
				for(int k =0; k < h_compHitsPerChamberVsGenP_byChamber_norm->GetNbinsX()+1; k++){
					float test_p = h_compHitsPerChamberVsGenP_byChamber_norm->GetXaxis()->GetBinCenter(k);

					if(DEBUG > 0)cout << "Testing For P = " << test_p << endl;
					float prob = 1.;
					for(const auto& hits: compHitsPerChamber){
						int biny = h_compHitsPerChamberVsGenP_byChamber_norm->GetYaxis()->FindBin(hits);
						float hit_prob = h_compHitsPerChamberVsGenP_byChamber_norm->GetBinContent(k, biny);
						if(DEBUG > 0)cout << "-- P(" << hits << ") = " << hit_prob << " biny = " << biny<< endl;
						prob *= hit_prob;
					}
					if(prob > highest_prob){
						highest_prob = prob;
						best_p_guess = test_p;
					}
					if(!foundDemo){
						h_demoLikelihood->SetBinContent(k, prob);

						//cout << "Gen P = " << genP << "comp hit count = ( ";
						//for(auto& hits : compHitsPerChamber){
						//	cout << hits << ", ";
						//}

						//cout << ") P_estimate = "  <<best_p_guess << endl;

					}
					if(foundDemo && !foundDemo2){
						h_demoLikelihood2->SetBinContent(k,prob);
					}

				}

				*/
			if(nChambersWithHits == 4){
				//TH2F* h_compHitsPerChamberVsGenP_byChamber_norm = h_compHitsPerChamberVsGenP_byChamber_normalized[4];
				float highest_prob = 0;
				float best_p_guess = -1;
				for(unsigned int k =0; k < nPBins+1; k++){
					float test_p = h_simHitEnergyPerChamberVsP_normalized->GetXaxis()->GetBinCenter(k);

					if(DEBUG > 0)cout << "Testing For P = " << test_p << endl;
					float prob = 1.;
					for(const auto& eloss : simHitLossPerChamber){
						int biny = h_simHitEnergyPerChamberVsP_normalized->GetYaxis()->FindBin(eloss);
						float hit_prob = h_simHitEnergyPerChamberVsP_normalized->GetBinContent(k,biny);
						if(DEBUG > 0)cout << "-- P(" << eloss << ") = " << hit_prob << " biny = " << biny<< endl;
						prob *= hit_prob;
					}

					//hcal likelihood
					int hcal_biny = h_totalHCALEnergyVsP_norm->GetYaxis()->FindBin(totalHcalEnergy);
					float hcal_prob = h_totalHCALEnergyVsP_norm->GetBinContent(k,hcal_biny);
					if(DEBUG > 0) cout << "HCAL: -- P(" << totalHcalEnergy << ") = " << hcal_prob << endl;
					prob *= hcal_prob;

					//ecal likelihood
					int ecal_biny = h_totalECALEnergyVsP_norm->GetYaxis()->FindBin(totalEcalEnergy);
					float ecal_prob = h_totalECALEnergyVsP_norm->GetBinContent(k, ecal_biny);
					if(DEBUG > 0) cout << "ECAL: -- P(" << totalEcalEnergy << ") = " << ecal_prob << endl;
					prob *= ecal_prob;





					if(prob > highest_prob){
						highest_prob = prob;
						best_p_guess = test_p;
					}
					if(!foundDemo){
						h_demoLikelihood->SetBinContent(k, prob);

						//cout << "Gen P = " << genP << "comp hit count = ( ";
						//for(auto& hits : compHitsPerChamber){
						//	cout << hits << ", ";
						//}

						//cout << ") P_estimate = "  <<best_p_guess << endl;

					}
					if(foundDemo && !foundDemo2){
						h_demoLikelihood2->SetBinContent(k,prob);
					}

				}
				if(foundDemo) foundDemo2 = true;
				foundDemo = true;


				/*
				cout << "Gen P = " << genP << " comp hit count = ( ";
				for(auto& hits : compHitsPerChamber){
					cout << hits << ", ";
				}
				*/
				if(i < 100) {
					cout << "Gen P = " << genP << " sim hit e = ( ";
					for(auto& eloss : simHitLossPerChamber){
						cout << eloss << ", ";
					}
					cout << "HCAL = " << totalHcalEnergy << ", ECAL = " << totalEcalEnergy;
					cout << ") P_estimate = "  << best_p_guess << endl;
				}

			//	if(nChambersWithHits == 4){
					if(genP >=0 && genP < 1000){
						h_p_est_0_1000GeV->Fill(best_p_guess);
					}else if (genP >=1000 && genP < 2000){
						h_p_est_1000_2000GeV->Fill(best_p_guess);
					}else if(genP >=2000 && genP < 3000){
						h_p_est_2000_3000GeV->Fill(best_p_guess);
					}else if(genP >= 3000 && genP < 4000){
						h_p_est_3000_4000GeV->Fill(best_p_guess);
					}
			//	}

				//cout << "\n\n GenP = " <<genP << " P Estimate = " << best_p_guess <<"\n\n" << endl;
				h_pResolution->Fill(genP - best_p_guess);
				for(int ibin = 0; ibin < h_compHitsPerChamberVsGenP_byChamber[4]->GetNbinsX(); ibin++){
					float low = h_compHitsPerChamberVsGenP_byChamber[4]->GetXaxis()->GetBinLowEdge(ibin);
					float high = h_compHitsPerChamberVsGenP_byChamber[4]->GetXaxis()->GetBinUpEdge(ibin);
					if(genP >= low  &&
							genP <  high) {
						h_pResolutions[low]->Fill(genP-best_p_guess);
					}
				}
			}
		}
	}

	outF->cd();

	for(auto& entry : h_compHitsPerChamberVsGenP_byChamber_normalized) entry.second->Write();
	for(auto& entry : h_pResolutions) entry.second->Write();
	h_pResolution->Write();
	h_demoLikelihood->Write();
	h_demoLikelihood2->Write();
	h_p_est_0_1000GeV->Write();
	h_p_est_1000_2000GeV->Write();
	h_p_est_2000_3000GeV->Write();
	h_p_est_3000_4000GeV->Write();
	h_compHitsMinusRecHits->Write();
	h_compHitsVsRecHits->Write();
	h_simHitsVsRecHits->Write();
	h_simHitsVsRecHitsADC->Write();
	h_energyPerChamberVsP->Write();
	TH2F* h_energyPerChamberVsP_normalized = (TH2F*)h_energyPerChamberVsP->Clone("h_energyPerChamberVsP_norm");
	for(int i=0; i < h_energyPerChamberVsP_normalized->GetNbinsX()+1; i++){
		float norm = 0;
		//calculate integral along pt bin
		for(int j=0;j < h_energyPerChamberVsP_normalized->GetNbinsY()+1; j++){
			norm += h_energyPerChamberVsP_normalized->GetBinContent(i,j);
		}
		//normalize everything for a given pt
		if(norm){
			for(int j=0;j < h_energyPerChamberVsP_normalized->GetNbinsY()+1; j++){
				float content = h_energyPerChamberVsP_normalized->GetBinContent(i,j);
				h_energyPerChamberVsP_normalized->SetBinContent(i,j,content/norm);
				//norm += h_multiplicityVsPt_normalized->GetBinContent(i,j);
			}
		}
	}
	h_energyPerChamberVsP_normalized->Write();
	h_simHitEnergyPerChamberVsP->Write();
	h_simHitEnergyPerChamberVsP_normalized->Write();
	h_secondaryCalEnergyVsP->Write();
	makeNormalizedInYSlices(h_secondaryCalEnergyVsP)->Write();
	h_totalCalEnergyVsP->Write();
	makeNormalizedInYSlices(h_totalCalEnergyVsP)->Write();
	h_totalECALEnergyVsP->Write();
	h_totalECALEnergyVsP_norm->Write();
	h_totalHCALEnergyVsP->Write();
	h_totalHCALEnergyVsP_norm->Write();
	h_totalSimECALEnergyVsP->Write();
	makeNormalizedInXSlices(makeNormalizedInYSlices(h_totalSimECALEnergyVsP))->Write();
	h_totalSimHCALEnergyVsP->Write();


	outF->Close();

	cout << "Printing Shower Probabilities: P(column | row)" << endl;
	cout << "genP < 500 GeV" << endl;

	cout << "Probability of Shower" << endl;
	for(unsigned int ich=0; ich < NCHAMBERS;ich++){
			printf("%5s ", CHAMBER_NAMES[ich].c_str());
		}
	cout << endl;
	for(unsigned int ich=0; ich < NCHAMBERS; ich++){
		float prob = 0;
		for(auto ishow: p_showers_lowPt[ich]) prob += ishow;
		if(prob) prob /=p_showers_lowPt[ich].size();
		printf("%5.3f ", prob);
	}
	cout << endl;


	cout << "Correlations : P(column | row)" << endl;
	for(unsigned int ich=0; ich < NCHAMBERS;ich++){
		if(ich ==0) cout << "      ";
		printf("%5s ", CHAMBER_NAMES[ich].c_str());
	}
	cout << endl;
	for(unsigned int ich=0; ich < NCHAMBERS; ich++){
		printf("%5s ", CHAMBER_NAMES[ich].c_str());
		for(unsigned int iich=0; iich < NCHAMBERS; iich++){
			float prob_iich_given_ich = 0;
			for(auto ishow: p_showers_correlation_lowPt[ich][iich]) prob_iich_given_ich +=ishow;
			if(prob_iich_given_ich) prob_iich_given_ich /= p_showers_correlation_lowPt[ich][iich].size();
			printf("%5.3f ", prob_iich_given_ich);
		}
		cout << endl;
	}
	cout << "genP > 3500 GeV" << endl;

	cout << "Probability of Shower" << endl;
	for(unsigned int ich=0; ich < NCHAMBERS;ich++){
		printf("%5s ", CHAMBER_NAMES[ich].c_str());
	}
	cout << endl;
	for(unsigned int ich=0; ich < NCHAMBERS; ich++){
		float prob = 0;
		for(auto ishow: p_showers_highPt[ich]) prob += ishow;
		if(prob) prob /=p_showers_highPt[ich].size();
		printf("%5.3f ", prob);
	}
	cout << endl;
	cout << "Correlations : P(column | row)" << endl;
	for(unsigned int ich=0; ich < NCHAMBERS;ich++){
		if(ich ==0) cout << "      ";
		printf("%5s ", CHAMBER_NAMES[ich].c_str());
	}
	cout << endl;
	for(unsigned int ich=0; ich < NCHAMBERS; ich++){
		printf("%5s ", CHAMBER_NAMES[ich].c_str());
		for(unsigned int iich=0; iich < NCHAMBERS; iich++){
			float prob_iich_given_ich = 0;
			for(auto ishow: p_showers_correlation_highPt[ich][iich]) prob_iich_given_ich +=ishow;
			if(prob_iich_given_ich) prob_iich_given_ich /= p_showers_correlation_highPt[ich][iich].size();
			printf("%5.3f ", prob_iich_given_ich);
		}
		cout << endl;
	}



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
			return GenStudy(string(argv[1]), string(argv[2]));
		case 4:
			return GenStudy(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
		case 5:
			return GenStudy(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
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







