/*
 * BremClasses.h
 *
 *  Created on: Feb 4, 2019
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_BREMCLASSES_H_
#define CSCPATTERNS_INCLUDE_BREMCLASSES_H_

#include <TH2F.h>
#include <TGraphAsymmErrors.h>
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
const std::string DETECTOR_NAMES[NDETECTORS] = {
		"ECAL",
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
const float SHOWER_THRESHOLDS[NDETECTORS] = {
		1.5,
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

//describes (energy) boundaries, used for creating histograms if you want log binning, etc
class LogBounds {
public:
	LogBounds(float minPow10, float maxPow10);
    float* bins(unsigned int nbins) const;
	float max() const {return _maxPow10;}
	float min() const {return _minPow10;}

private:
	float _minPow10;
	float _maxPow10;
};

static const LogBounds MOMENTUM_BOUNDARIES(0, 4);

//empirical boundaries of energy deposition in each detector
static const LogBounds DETECTOR_ENERGY_BOUNDARIES[NDETECTORS] = {
		LogBounds(-1.5,3), //ECAL
		LogBounds(-3,1), //HCAL
		LogBounds(-5.5,-3), //ME Chambers
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
		LogBounds(-5.5,-3),
};





//makes a histogram with normalized slices in y
TH2F* makeNormalizedInYSlices(const TH2F* hist);

TH2F* makeNormalizedInXSlices(const TH2F* hist);

class PointEstimate {
public:
	PointEstimate(){
		_estimate = 0;
		_lower = 0;
		_upper = 0;
	}
	PointEstimate(float estimate, float lower, float upper){
		_estimate = estimate;
		_lower = lower;
		_upper = upper;
	}

	void set(float estimate, float lower, float upper){
		_estimate = estimate;
		_lower = lower;
		_upper = upper;
	}
	float estimate() const { return _estimate;}
	float lower() const {return _lower;}
	float upper() const {return _upper;}
private:
	float _estimate;
	float _lower;
	float _upper;
};


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

	PDF(const std::string& name,unsigned int xBins, float* xbins, unsigned int yBins, float* ybins) : PDF(){
			_scatterPlot = new TH2F(name.c_str(),name.c_str(),xBins, xbins, yBins, ybins);
		}

	PDF(const std::string& name, TFile* f) : PDF(){
		_scatterPlot = (TH2F*)f->Get(name.c_str());
	}

	~PDF(){
		//if(_scatterPlotNorm) delete _scatterPlotNorm;
		//if(_scatterPlot) delete _scatterPlot;
	}

	int fill(float x, float y);
	int normalize();

	TH1D* projection(float y);
	//TGraphAsymmErrors* projection(float y);

	int write(TFile* f, const string& xaxis, const string& yaxis);

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
	void setDetector(const std::string& detector, float deposit);

	float getDeposit(const std::string& detector);


	//returns the amount of non-zero energy deposits
	unsigned int nDeposits(){
		unsigned int ndeposits = 0;
		for(auto dep : _deposits){
			//cout << dep << endl;
			if(dep) ndeposits++;
		}
		//cout << "deposits: " << ndeposits << endl;
		return ndeposits;
	}

	//returns if the detector has any energy deposit at all
	const array<bool,NDETECTORS> hasEnergy() const;
	//returns if the detector had a shower, as defined by the thresholds
	const array<bool, NDETECTORS> hasShower() const;


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

	void fill(float p, const EnergyDeposits& e);


	vector<float> _energies[NDETECTORS];

	vector<TH1F*> plots();
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
