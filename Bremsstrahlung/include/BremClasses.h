/*
 * BremClasses.h
 *
 *  Created on: Feb 4, 2019
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_BREMCLASSES_H_
#define CSCPATTERNS_INCLUDE_BREMCLASSES_H_

#include <TH2F.h>
#include <TFile.h>
#include <iostream>

#include "../../CSCPatterns/include/PatternConstants.h"

using namespace std;

/*
const float ECAL_SHOWER_THRESHOLD = 1.5; //GeV PULLED OUT OF THIN AIR, NEED TO TUNE
const float HCAL_SHOWER_THRESHOLD = 0.25; //GeV PULLED OUT OF THIN AIR, NEED TO TUNE
const float CSC_SHOWER_THRESHOLD = 0.0001; //PULLED OUT OF THIN AIR, NEED TO TUNE
*/

const unsigned int NDETECTORS = NCHAMBERS+2; //all muon chambers + ECAL and HCAL
//not done in the smartest way...
const std::string DETECTOR_NAMES[NDETECTORS] = {"ECAL",
		"HCAL",
		"ME11B",
		"ME11A",
		"ME12",
		"ME13",
		"ME21",
		"ME22",
		"ME31",
		"ME32",
		"ME41",
		"ME42"
};

//TODO: all made up, come up with definition!
const float SHOWER_THRESHOLDS[NDETECTORS] = {1.5,
		0.15,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001,
		0.0001
};

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
	return normalized;
}

/* Idea: Want to fill these with Hits in detector vs gen P,
 * with that information, once everything is filled, normalized in
 * P bins.
 *
 * Make one of these for each type of chamber: ECAL, HCAL, ME11, ME12, ME13...
 *
 * Want to be able to ask the probability of a given nhits / energy for a
 * given genP
 *
 * Should have capability to return projection along X for probability given
 * hits/energy deposit
 *
 * Eventually, want to weight each of these, since it is likely that the ECAL
 * will give a better indication of the energy than a smaller chamber.
 *
 * Factor_to_Maximize = a_ecal * p_ecal(energy_ecal) + a_hcal * p_hcal(energy_hcal) + ...
 *
 */
class PDF {
public:
	PDF() {
		_scatterPlot = 0;
		_scatterPlotNorm = 0;
		_isNormalized = false;
	}

	PDF(const std::string& name,unsigned int xBins, float xLow, float xHigh, unsigned int yBins, float yLow, float yHigh) : PDF(){
		_scatterPlot = new TH2F(name.c_str(),name.c_str(),xBins, xLow, xHigh, yBins, yLow,yHigh);
	}

	PDF(const std::string& name, TFile* f) : PDF(){
		_scatterPlot = (TH2F*)f->Get(name.c_str());
	}

	~PDF(){
		//if(_scatterPlotNorm) delete _scatterPlotNorm;
		//if(_scatterPlot) delete _scatterPlot;
	}

	int fill(float x, float y){
		if(!_scatterPlot) return -1;
		if(_isNormalized){
			cout << "Already Normalized!" << endl;
			return -1;
		}
		return _scatterPlot->Fill(x,y);
	}
	int normalize() {
		if(_isNormalized) return 0;
		//normalized in Y first, then in X
		_scatterPlotNorm = makeNormalizedInYSlices(_scatterPlot);
		_isNormalized = true;
		return 0;
	}

	TH1D* projection(float y) {
		normalize();
		int ybin = _scatterPlot->GetYaxis()->FindBin(y);
		return _scatterPlotNorm->ProjectionX("_px",ybin,ybin);
	}

	int write(TFile* f){
		f->cd();
		normalize();
		if(_scatterPlot)_scatterPlot->Write();
		if(_scatterPlotNorm)_scatterPlotNorm->Write();
		return 0;
	}

private:
	bool _isNormalized;
	TH2F* _scatterPlot;
	TH2F* _scatterPlotNorm;
};


class EnergyDeposits {
	friend class EnergyDistributions;
public:
	/*
	static const unsigned int NDETECTORS = NCHAMBERS+2; //all muon chambers + ECAL and HCAL
	//not done in the smartest way...
	static constexpr std::string DETECTOR_NAMES[NDETECTORS] = {"ECAL",
													"HCAL",
													"ME11B",
													"ME11A",
													"ME12",
													"ME13",
													"ME21",
													"ME22",
													"ME31",
													"ME32",
													"ME41",
													"ME42"
	};

	//TODO: all made up, come up with definition!
	static constexpr float SHOWER_THRESHOLDS[NDETECTORS] = {1.5,
			0.25,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001
};
	 */
	EnergyDeposits(){
		for(unsigned int i=0; i < NDETECTORS;i++){
			_deposits[i] = 0;
		}
	}
	EnergyDeposits(float deposits[NDETECTORS]) {
		for(unsigned int i=0; i< NDETECTORS; i++){
			_deposits[i] = deposits[i];
		}
	}
	void setDetector(const std::string& detector, float deposit){
		for(unsigned int i=0; i < NDETECTORS; i++){
			auto det = DETECTOR_NAMES[i];
			if(detector != det) continue;
			/* Temporary.
			 *
			 * TODO: Need to figure how to combine energy
			 * deposits from same station/ring chambers
			 *
			 * temporarily taking the highest energy deposit
			 * and forgetting about the other deposits.
			 *
			 */
			if(_deposits[i] > deposit) break;
			_deposits[i] = deposit;
			break;
		}
	}

	//returns if the detector has any energy deposit at all
	const array<bool,NDETECTORS> hasEnergy() const{
		array<bool,NDETECTORS> energy;
		for(unsigned int i=0; i < NDETECTORS; i++){
			energy[i] = _deposits[i] > 0;
		}
		return energy;
	}
	//returns if the detector had a shower, as defined by the thresholds
	const array<bool, NDETECTORS> hasShower() const {
		array<bool,NDETECTORS> shower;
		for(unsigned int i=0; i < NDETECTORS; i++){
			shower[i] = _deposits[i] > SHOWER_THRESHOLDS[i];
		}
		return shower;
	}


private:
	float _deposits[NDETECTORS];
};

/* Distributions of energy deposits within chambers
 *
 */
class EnergyDistributions{
public:
	EnergyDistributions(float pLow, float pHigh){
		_pLow = pLow;
		_pHigh= pHigh;
	}

	void fill(float p, const EnergyDeposits& e){
		if(p >= _pHigh || p < _pLow) return; //only look in correct range
		for(unsigned int i =0; i < NDETECTORS;i++){
			if(e._deposits[i]) _energies[i].push_back(e._deposits[i]);
		}
	}


	vector<float> _energies[NDETECTORS];

	vector<TH1F*> plots(){
		vector<TH1F*> plots;
		for(unsigned int i =0; i < NDETECTORS; i++){
			TH1F* p = new TH1F((DETECTOR_NAMES[i]+"_energy"+to_string((int)_pLow)+"_"+to_string((int)_pHigh)).c_str(),
					DETECTOR_NAMES[i].c_str(), 100, 0, 2*SHOWER_THRESHOLDS[i]);
			for(auto energy : _energies[i]){
				p->Fill(energy);
			}
			plots.push_back(p);
		}


		return plots;
	}
private:
	float _pLow;
	float _pHigh;

};


/* Calculates probability of showering for all
 * types of detectors, and correlation between
 * detectors
 */
class ProbabilityMap {
public:
	ProbabilityMap(float pLow, float pHigh);
	~ProbabilityMap(){};

	int fill(float p, const EnergyDeposits& deposits);

	void print();
	float midP(){
		return (_pHigh+_pLow)/2.;
	}


private:
	void calculate();


	float _pLow;
	float _pHigh;
	bool _isCalculated;

	float _probabilities[NDETECTORS];
	unsigned int _numerators[NDETECTORS];
	unsigned int _denominators[NDETECTORS];
	float _correlations [NDETECTORS][NDETECTORS];
	/*
	float _showerThresholds[NDETECTORS] = {1.5,
			0.25,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001,
			0.0001
			};*/

	vector<EnergyDeposits> _events;
	//std::vector<int> _hasShower [NDETECTORS];
public:
	const float* probabilities() const{
		return _probabilities;
	}

	const unsigned int* numerators() const{
		return _numerators;
	}

	const unsigned int* denominators() const {
		return _denominators;
	}

	/*
	//returns a pair (numerator, denominator) which, when divided, give
	pair<TH1F*,TH1F*> probabiltyHists(){

	}
	*/
};



#endif /* CSCPATTERNS_INCLUDE_BREMCLASSES_H_ */
