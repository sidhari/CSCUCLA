/*
 * BremClasses.cpp
 *
 *  Created on: Feb 8, 2019
 *      Author: wnash
 */

#include "../include/BremClasses.h"


ProbabilityMap::ProbabilityMap(float pLow, float pHigh) {
	_pLow = pLow;
	_pHigh = pHigh;
	_isCalculated = false;
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
			if(eventHasEnergy[i]) hasShower[i].push_back(eventHasShower[i]);
			for(unsigned int j=0; j < NDETECTORS; j++){
				if(eventHasShower[i] && eventHasEnergy[j]) hasShowerCorrelation[i][j].push_back(eventHasShower[j]);
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







