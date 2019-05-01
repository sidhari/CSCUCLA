/*
 * BremClasses.cpp
 *
 *  Created on: Feb 8, 2019
 *      Author: wnash
 */

#include "../include/BremClasses.h"
#include "../include/BremHelperFunctions.h"

#include <TMath.h>

LogBounds::LogBounds(float minPow10, float maxPow10) {
	_minPow10 = minPow10;
	_maxPow10 = maxPow10;
}

float* LogBounds::bins(unsigned int nbins) const{
	float* logBins =  new float[nbins+1];
	for(unsigned int i=0; i<= nbins; i++){
		logBins[i] = TMath::Power(10., 1.*(_maxPow10-_minPow10)*i/nbins + _minPow10);
	}
	return logBins;
}

//makes a histogram with normalized slices in y
TH2F* makeNormalizedInYSlices(const TH2F* hist){
	TH2F* normalized = (TH2F*)hist->Clone((string(hist->GetName())+"_normY").c_str());
	for(int i=0; i < normalized->GetNbinsX()+2; i++){
		float norm = 0;
		//calculate integral along p bin
		for(int j=0;j < normalized->GetNbinsY()+2; j++){
			norm += normalized->GetBinContent(i,j);
		}
		//normalize everything for a given p
		if(norm){
			for(int j=0;j < normalized->GetNbinsY()+2; j++){
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
	for(int i =0; i < normalized->GetNbinsY()+2; i++){
		float norm =0;
		for(int j=0;j < normalized->GetNbinsX()+2; j++){
			norm += normalized->GetBinContent(j,i);
		}
		if(norm) {
			for(int j=0; j < normalized->GetNbinsX()+2;j++){
				float content = normalized->GetBinContent(j,i);
				normalized->SetBinContent(j,i,content/norm);
			}
		}
	}
	return normalized;
}

//
// PDF
//


int PDF::fill(float x, float y){
	//don't fill if there is nothing there, maybe a more elegant way to handle this?
	if(!y) return 0;
	if(!_scatterPlot) return -1;
	return _scatterPlot->Fill(x,y);
}


int PDF::normalize() {
	if(_isNormalized) return 0;
	//normalized in Y first, then in X
	_scatterPlotNorm = makeNormalizedInYSlices(_scatterPlot);
	_isNormalized = true;
	return 0;
}


TH1D* PDF::projection(float y) {
	normalize();
	int ybin = _scatterPlot->GetYaxis()->FindBin(y);
	return _scatterPlotNorm->ProjectionX("_px",ybin,ybin);
}


/*
TGraphAsymmErrors* PDF::projection(float y) {
	//cout << "projecting..." << endl;
	int ybin = _scatterPlot->GetYaxis()->FindBin(y);

	const unsigned int npoints = _scatterPlot->GetNbinsX();
	float Y[npoints];
	float lowerY[npoints];
	float upperY[npoints];
	float X[npoints];
	float lowerX[npoints];
	float upperX[npoints];

	//for each x bin at the given y coordinate, find the ratio of entries in that bin
	// compared to the total in the column (same x value)
	for(unsigned int i=1; i <npoints+1; i++){
		int nentries = _scatterPlot->ProjectionY("", i,i)->GetEntries();
		int nentriesInBin = _scatterPlot->GetBinContent(i,ybin);
		cout << "nEntries: " << nentries << " nentriesInBin: " << nentriesInBin << endl;
		const PointEstimate p = ClopperPearson(nentriesInBin,nentries);
		Y[i-1] = p.estimate();
		lowerY[i-1] = p.lower();
		upperY[i-1] = p.upper();
		X[i-1] = _scatterPlot->GetXaxis()->GetBinCenter(i);
		lowerX[i-1] = _scatterPlot->GetXaxis()->GetBinWidth(i)/2.;
		upperX[i-1] = _scatterPlot->GetXaxis()->GetBinWidth(i)/2.;
	}
	TGraphAsymmErrors* g = new TGraphAsymmErrors(npoints,X,Y,  lowerX,upperX, lowerY,upperY);
	g->SetName((string(_scatterPlot->GetName())+"_graph").c_str());
	g->SetTitle(string(_scatterPlot->GetTitle()).c_str());

	return g;
}
*/

int PDF::write(TFile* f,const string& xaxis, const string& yaxis){
		_scatterPlot->GetXaxis()->SetTitle(xaxis.c_str());
		_scatterPlot->GetYaxis()->SetTitle(yaxis.c_str());

		f->cd();
		normalize();
		if(_scatterPlot)_scatterPlot->Write();
		if(_scatterPlotNorm)_scatterPlotNorm->Write();
		return 0;
	}

//
// Energy Deposits
//


void EnergyDeposits::setDetector(const std::string& detector, float deposit){
		for(unsigned int i=0; i < NDETECTORS; i++){
			auto det = DETECTOR_NAMES[i];
			if(detector != det) continue;
			/* Temporary.
			 *
			 * TODO: Need to figure how to combine energy
			 * deposits from neighboring same station/ring chambers
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

float EnergyDeposits::getDeposit(const std::string& detector){
	for(unsigned int i=0; i < NDETECTORS; i++){
		auto det = DETECTOR_NAMES[i];
		if(detector != det) continue;
		return _deposits[i];
	}
	return -1;
}

const array<bool,NDETECTORS> EnergyDeposits::hasEnergy() const{
		array<bool,NDETECTORS> energy;
		for(unsigned int i=0; i < NDETECTORS; i++){
			energy[i] = _deposits[i] > 0;
		}
		return energy;
	}

const array<bool, NDETECTORS> EnergyDeposits::hasShower() const {
	array<bool,NDETECTORS> shower;
	for(unsigned int i=0; i < NDETECTORS; i++){
		shower[i] = _deposits[i] > SHOWER_THRESHOLDS[i];
	}
	return shower;
}

//
// Energy Distributions
//

void EnergyDistributions::fill(float p, const EnergyDeposits& e){
	if(p >= _pHigh || p < _pLow) return; //only look in correct range
	for(unsigned int i =0; i < NDETECTORS;i++){
		if(e._deposits[i]) _energies[i].push_back(e._deposits[i]);
	}
}

vector<TH1F*> EnergyDistributions::plots(){
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

ProbabilityMap::ProbabilityMap(float pLow, float pHigh) {
	_pLow = pLow;
	_pHigh = pHigh;
	_isCalculated = false;

	for(unsigned int i =0; i < NDETECTORS; i++){
		_probabilities[i] = 0;
		_numerators[i] = 0;
		_denominators[i] = 0;
		for(unsigned int j=0; j < NDETECTORS; j++){
			_correlations[i][j] = 0;
		}
	}
	/*
	_showerThresholds = {1.5,
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
}

int ProbabilityMap::fill(float p, const EnergyDeposits& deposits){
	if(_isCalculated) return -1;
	if(p >= _pHigh || p < _pLow) return 0; //only look in correct range
	_events.push_back(deposits);

	//cout << "filling P: " << p << " det: " << detector << " energyDep: " << energyDeposit << endl;
	/*
	bool foundDetector = false;
	for(unsigned int i=0; i < NDETECTORS; i++)  {
		auto det = DETECTOR_NAMES[i];
		if(detector != det) continue;
		foundDetector = true;
		//cout << "foundDet: " << det << endl;
		_hasShower[i].push_back(energyDeposit > _showerThresholds[i]);

	}
	if(!foundDetector) return -1;
	*/
	return 0;
}

void ProbabilityMap::calculate(){
	if(_isCalculated) return;


	vector<bool> hasShower[NDETECTORS];
	vector<bool> hasShowerCorrelation[NDETECTORS][NDETECTORS];
	for(auto& event : _events){ //iterate through all the events
		auto eventHasEnergy = event.hasEnergy();
		auto eventHasShower = event.hasShower();
		/*
		cout << "hasEnergy: ";
		for(auto e : eventHasEnergy) cout << e << " ";
		cout << endl;
		cout << "hasShower: ";
		for(auto e : eventHasShower) cout << e << " ";
		cout << endl;
		 */

		for(unsigned int i=0; i < NDETECTORS; i++){
			if(eventHasEnergy[i]) {
				hasShower[i].push_back(eventHasShower[i]);
				_denominators[i]++;
				if(eventHasShower[i]) _numerators[i]++;
				for(unsigned int j=0; j < NDETECTORS; j++){
					if(eventHasShower[i] && eventHasEnergy[j]) hasShowerCorrelation[i][j].push_back(eventHasShower[j]);
				}
			}
		}
	}

	for(unsigned int i=0; i < NDETECTORS; i++){
		unsigned int showers = 0;
		for(auto shower : hasShower[i]){
			showers += shower;
		}
		_probabilities[i] = hasShower[i].size() ? (float)showers/hasShower[i].size() : 0;
		for(unsigned int j=0; j < NDETECTORS; j++){
			unsigned int doubleShowers = 0;
			for(auto doubleShower : hasShowerCorrelation[i][j]){
				doubleShowers += doubleShower;
			}
			_correlations[i][j] = hasShowerCorrelation[i][j].size() ? (float)doubleShowers/hasShowerCorrelation[i][j].size() : 0;
		}
	}

	_isCalculated = true;
}

void ProbabilityMap::print() {
	if(!_isCalculated) calculate();
	cout << "== " << _pLow << " <= p < " <<_pHigh << endl;
	cout << "Probability of Shower" << endl;
	for(unsigned int ich=0; ich < NDETECTORS;ich++){
		printf("%5s ", DETECTOR_NAMES[ich].c_str());
	}
	cout << endl;
	for(auto prob: _probabilities){
		printf("%5.3f ", prob);
	}
	cout << endl;


	cout << "Shower Correlations -  P(column | row), given both have a non-zero amount of energy deposit" << endl;
	for(unsigned int ich=0; ich < NDETECTORS;ich++){
		if(ich ==0) cout << "      ";
		printf("%5s ", DETECTOR_NAMES[ich].c_str());
	}
	cout << endl;
	for(unsigned int i=0; i < NDETECTORS; i++){
		printf("%5s ", DETECTOR_NAMES[i].c_str());
		for(unsigned int j=0; j < NDETECTORS; j++){
			printf("%5.3f ", _correlations[i][j]);
		}
		cout << endl;
	}

	//TODO: correlations

}







