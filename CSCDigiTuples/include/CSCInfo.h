#ifndef CSCUCLA_CSCDIGITUPLES_CSCINFO_H
#define CSCUCLA_CSCDIGITUPLES_CSCINFO_H


/*
 *  Author: William Nash
 *  Date Inherited: September 11 2018
 * 
 *	Forked By: Chau Dao
 *	Date Forked: 26 July 2019
 */

#include <vector>

#include <TTree.h>

#define GET_VARIABLE_NAME(Variable) (#Variable)

typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

//
// TODO: Can clean up a lot of boiler plate using boost libraries and
//	"reflections".
//

using namespace std;

class TreeContainer;

namespace CSCInfo {


class Object {
public:
	const string name;
	virtual ~Object(){};
	Object(const char *n): name(n){}

	/* @brief takes a variable name and returns a std::string in current branch convention
	 * branchify
	 * 	- reqires: pointer to the variable name [varname]
	 * 	- returns: required string format to read from current convention TTree
	 */

	inline std::string branchify(char const * varname)
	{
		return (name+'_'+string(varname));
	}
};

class Event : public Object{
public:

	/*
	 * Constructor used in FillCSCInfo.h to write to a tree using current
	 * naming conventions. Analogous for following classes.
	 */
	Event() : Object("Event"){
		EventNumber = 0;
		RunNumber = 0;
		LumiSection = -1;
		BXCrossing = -1;
		NSegmentsInEvent = 0;
	}

	/*
	 * Constructor used when wanting to read from branches of a TTree. Can be
	 * used independently of FillCSCInfo.h. Analogous for following classes
	 */
	Event(TTree* t): Event(){
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(EventNumber)).c_str(), &EventNumber);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(RunNumber)).c_str(), &RunNumber);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(LumiSection)).c_str(),&LumiSection);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BXCrossing)).c_str(), &BXCrossing);
		if(t->GetBranch(branchify(GET_VARIABLE_NAME(NSegmentsInEvent)).c_str())) { //in the event you are using legacy trees, defaults the value to 0
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(NSegmentsInEvent)).c_str(),
						&NSegmentsInEvent);
		}
	}

	unsigned long long EventNumber;
	unsigned long long RunNumber;
	int LumiSection;
	int BXCrossing;
	int NSegmentsInEvent;
};

class GenParticles : public Object{
public:
	GenParticles() : Object("gen") {
		pdg_id = 0;
		pt = 0;
		eta = 0;
		phi = 0;
		q = 0;
	}

	GenParticles(TTree* t) : GenParticles() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pdg_id)).c_str(),
				&pdg_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pt)).c_str(),
				&pt);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(eta)).c_str(),
				&eta);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(phi)).c_str(),
				&phi);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(q)).c_str(),
				&q);
	}

	unsigned int size() const {
		return pt ? pt->size() : 0;
	}
	std::vector<int>* pdg_id;
	std::vector<float>* pt;
	std::vector<float>* eta;
	std::vector<float>* phi;
	std::vector<int>* q; //charge
};

class SimHits : public Object {
public:
	SimHits() : Object("sim_hits") {
		ch_id = 0;
		pdg_id = 0;
		layer = 0;
		energyLoss = 0;
		thetaAtEntry = 0;
		phiAtEntry = 0;
		pAtEntry = 0;
	}

	SimHits(TTree* t) : SimHits() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(),
				&ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pdg_id)).c_str(),
				&pdg_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(layer)).c_str(),
				&layer);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(energyLoss)).c_str(),
				&energyLoss);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(thetaAtEntry)).c_str(),
				&thetaAtEntry);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(phiAtEntry)).c_str(),
				&phiAtEntry);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pAtEntry)).c_str(),
				&pAtEntry);

	}
	unsigned int size() const {
		return ch_id ? ch_id->size() : 0;
	}

	std::vector<int>* ch_id;
	std::vector<int>* pdg_id;
	std::vector<int>* layer;
	std::vector<float>* energyLoss;
	std::vector<float>* thetaAtEntry;
	std::vector<float>* phiAtEntry;
	std::vector<float>* pAtEntry;
};

class CaloHit : public Object {
public:
	CaloHit(const string& pref) : Object(string(pref+"_calo_hits").c_str()){
		energyEM = 0;
		energyHad = 0;
		eta = 0;
		phi = 0;
	}

	CaloHit(const string& pref, TTree* t) : CaloHit(pref) {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(energyEM)).c_str(),
								&energyEM);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(energyHad)).c_str(),
								&energyHad);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(eta)).c_str(),
								&eta);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(phi)).c_str(),
								&phi);
	}
	unsigned int size() const {
		return energyEM? energyEM->size() : 0;
	}

	std::vector<float>* energyEM; //GeV
	std::vector<float>* energyHad; //GeV
	std::vector<float>* eta;
	std::vector<float>* phi;

	static std::string ecalBarrel(){return "ecalBarrel";}
	static std::string ecalEndcap(){return "ecalEndcap";}
	static std::string ecalPreshower(){return "ecalPreshower";}
	static std::string hcal(){return "hcal";}
};

class PFCandidate : public Object {
public:
	PFCandidate() : Object("pfcand") {
		pdg_id = 0;
		particleId = 0;
		eta = 0;
		phi = 0;
		ecalEnergy = 0;
		hcalEnergy = 0;
		h0Energy = 0;
	}

	unsigned int size() const {
		return pdg_id ? pdg_id->size() : 0;
	}


	PFCandidate(TTree* t) : PFCandidate() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pdg_id)).c_str(),
				&pdg_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(particleId)).c_str(),
						&particleId);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(eta)).c_str(),
						&eta);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(phi)).c_str(),
						&phi);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ecalEnergy)).c_str(),
						&ecalEnergy);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(hcalEnergy)).c_str(),
						&hcalEnergy);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(h0Energy)).c_str(),
								&h0Energy);
	}

	std::vector<int>* pdg_id;
	std::vector<int>* particleId; //internal id
	std::vector<float>* eta;
	std::vector<float>* phi;
	std::vector<float>* ecalEnergy;
	std::vector<float>* hcalEnergy;
	std::vector<float>* h0Energy;
};


class Muons : public Object{
public:
	Muons() :Object("muon") {
		pt = 0;
		eta = 0;
		phi = 0;
		q = 0;
		isGlobal = 0;
		isTracker = 0;
	}
	Muons(TTree* t) : Muons() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pt)).c_str(), &pt);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(eta)).c_str(), &eta);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(phi)).c_str(), &phi);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(q)).c_str(), &q);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(isGlobal)).c_str(), &isGlobal);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(isTracker)).c_str(), &isTracker);
	}

	unsigned int size() const {
		return pt ? pt->size() : 0;
	}

	std::vector<float>* pt;
	std::vector<float>* eta;
	std::vector<float>* phi;
	std::vector<int>* q; //charge
	std::vector<bool>* isGlobal;
	std::vector<bool>* isTracker;
};

class Segments : public Object {
public:
	Segments() : Object("segment") {
		mu_id = 0;
		ch_id = 0;
		pos_x = 0;
		pos_y = 0;
		dxdz = 0;
		dydz = 0;
		chisq = 0;
		nHits = 0;
	}
	Segments(TTree* t) : Segments() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(mu_id)).c_str(), &mu_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pos_x)).c_str(), &pos_x);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pos_y)).c_str(), &pos_y);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(dxdz)).c_str(), &dxdz);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(dydz)).c_str(), &dydz);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(chisq)).c_str(), &chisq);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(nHits)).c_str(), &nHits);
	}

	unsigned int size() const {
		return mu_id ? mu_id->size() : 0;
	}

	std::vector<int>* mu_id; //id associated with the muon
	std::vector<int>* ch_id; //id associated with chamber
	std::vector<float>* pos_x; // [strips]
	std::vector<float>* pos_y;
	std::vector<float>* dxdz; // [strips / layer]
	std::vector<float>* dydz;
	std::vector<float>* chisq;
	std::vector<size8>* nHits;

};

class RecHits : public Object{
	public:
	RecHits() : Object("rh"){
		mu_id = 0;
		ch_id = 0;
		lay = 0;
		pos_x = 0;
		pos_y = 0 ;
		e = 0;
		max_adc = 0;
	}
	RecHits(TTree* t) : RecHits() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(mu_id)).c_str(), &mu_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(lay)).c_str(), &lay);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pos_x)).c_str(), &pos_x);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pos_y)).c_str(), &pos_y);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(e)).c_str(), &e);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(max_adc)).c_str(), &max_adc);
	}
	unsigned int size() const {
		return mu_id ? mu_id->size() : 0;
	}
	std::vector<int>* mu_id;
	std::vector<int>* ch_id;
	std::vector<size8>* lay;
	std::vector<float>* pos_x;
	std::vector<float>* pos_y;
	std::vector<float>* e; //energy
	std::vector<float>* max_adc; // time bin with the max ADCs

};

class LCTs :public Object {
public:
	LCTs() : Object("lct") {
		ch_id = 0;
		quality = 0;
		pattern = 0;
		bend = 0;
		keyWireGroup = 0;
		keyHalfStrip = 0;
		bunchCross = 0;
	}
	LCTs(TTree* t) : LCTs() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pattern)).c_str(), &pattern);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(bend)).c_str(), &bend);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyWireGroup)).c_str(), &keyWireGroup);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyHalfStrip)).c_str(), &keyHalfStrip);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(bunchCross)).c_str(), &bunchCross);
	}

	unsigned int size() const {
		return ch_id ? ch_id->size() : 0;
	}

	std::vector<size16>* ch_id;
	std::vector<size8>* quality;
	std::vector<size8>* pattern;
	std::vector<size8>* bend;
	std::vector<size8>* keyWireGroup;
	std::vector<size8>* keyHalfStrip;
	std::vector<size8>* bunchCross;

};

class CLCTs : public Object{
public:
	CLCTs() : Object("clct") {
		ch_id = 0;
		isValid = 0;
		quality = 0;
		pattern = 0;
		stripType = 0;
		bend = 0;
		halfStrip = 0;
		CFEB = 0;
		BX = 0;
		trkNumber = 0;
		keyStrip = 0;
	}
	CLCTs(TTree* t) : CLCTs() {
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(isValid)).c_str(), &isValid);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(pattern)).c_str(), &pattern);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(stripType)).c_str(), &stripType);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(bend)).c_str(), &bend);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(halfStrip)).c_str(), &halfStrip);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(CFEB)).c_str(), &CFEB);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BX)).c_str(), &BX);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(trkNumber)).c_str(), &trkNumber);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyStrip)).c_str(), &keyStrip);
	}

	unsigned int size() const {
		return ch_id ? ch_id->size() : 0;
	}

	/* size:
	 *	- requires: the designated index [chamber_index]
	 * 	- returns: amount of clcts, [count] in the chamber [chamber_index]
	 */
	unsigned int size(unsigned int chamber_index) const {
		if(!ch_id) return 0;
		unsigned int count =0;
		for(auto id : *ch_id){
			if(chamber_index == id) count++;
		}
		return count;
	}

	std::vector<size16>* ch_id;
	std::vector<size8>* isValid;
	std::vector<size16>* quality;
	std::vector<size8>* pattern;
	std::vector<size8>* stripType;
	std::vector<size8>* bend;
	std::vector<size8>* halfStrip;
	std::vector<size8>* CFEB;
	std::vector<size8>* BX;
	std::vector<size8>* trkNumber;
	std::vector<size8>* keyStrip;

};

class ALCTs : public Object 
{
	public: 
		ALCTs() : Object("alct")
		{
			ch_id = 0;
			isValid = 0;
			quality = 0; 
			accelerator = 0;
			collisionB = 0;
			keyWG = 0;
			BX = 0;
			trkNumber = 0;
			fullBX = 0;
		}

		ALCTs(TTree* t) : ALCTs()
		{
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(isValid)).c_str(), &isValid);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(accelerator)).c_str(), &accelerator);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(collisionB)).c_str(), &collisionB);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyWG)).c_str(), &keyWG);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BX)).c_str(), &BX);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(trkNumber)).c_str(), &trkNumber);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(fullBX)).c_str(), &fullBX);
		}

		unsigned int size() const 
		{
			return ch_id ? ch_id->size() : 0;
		}

		std::vector<size16>* ch_id;
		std::vector<size8>* isValid;
		std::vector<size8>* quality;
		std::vector<size8>* accelerator;
		std::vector<size8>* collisionB;
		std::vector<size8>* keyWG;
		std::vector<size8>* BX; 			//bunch crossing
		std::vector<size8>* trkNumber;
		std::vector<size16>* fullBX;
};

class Wires : public Object 
{
	public: 
		Wires() : Object("wire")
		{
			ch_id = 0;
			group = 0;
			lay = 0;
			timeBin = 0;
			BX = 0;
			timeBinWord = 0;
			//timeBinsOn = 0;
		}

		Wires(TTree* t) : Wires()
		{
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(group)).c_str(), &group);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(lay)).c_str(), &lay);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(timeBin)).c_str(), &timeBin);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BX)).c_str(), &BX);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(timeBinWord)).c_str(), &timeBinWord);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(timeBinsOn)).c_str(), &timeBinsOn);
		}

		unsigned int size() const 
		{
			return ch_id ? ch_id->size() : 0;
		}

		std::vector<size16>* ch_id;
		std::vector<int>* group;
		std::vector<size8>* lay;
		std::vector<int>* timeBin;
		std::vector<int>* BX;
		std::vector<uint32_t>* timeBinWord;
		//std::vector<std::vector<int>>* timeBinsOn;
};

class Strips : public Object
{
	public:
		Strips() : Object("strip")
		{
			ch_id = 0;
			lay = 0;
			num = 0;
			//ADC = 0;
			//L1APhase = 0;
			//ADCOverflow = 0;
			//OverlappedSample = 0;
			//Errorstat = 0;
		}

		Strips(TTree* t) : Strips()
		{
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(lay)).c_str(), &lay);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(num)).c_str(), &num);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ADC)).c_str(), &ADC);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(L1APhase)).c_str(), &L1APhase);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ADCOverflow)).c_str(), &ADCOverflow);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(OverlappedSample)).c_str(), &OverlappedSample);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(Errorstat)).c_str(), &Errorstat);
		}

		unsigned int size() const 
		{
			return ch_id ? ch_id->size() : 0;
		}
	
		std::vector<size16>* ch_id;
		std::vector<size8>* lay;
		std::vector<int>* num;
		//std::vector<std::vector<int>>* ADC;
		//std::vector<std::vector<int>>* L1APhase;
		//std::vector<std::vector<uint16_t>>* ADCOverflow;
		//std::vector<std::vector<uint16_t>>* overlappedSample;
		//std::vector<std::vector<uint16_t>>* errorstat;
};

class Comparators : public Object {
public:
	Comparators() : Object("comp") {
		ch_id = 0;
		lay = 0;
		strip = 0;
		halfStrip = 0;
		bestTime = 0;
		nTimeOn = 0;
	}
	Comparators(TTree* t) : Comparators(){
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(lay)).c_str(), &lay);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(strip)).c_str(), &strip);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(halfStrip)).c_str(), &halfStrip);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(bestTime)).c_str(), &bestTime);
		t->SetBranchAddress(branchify(GET_VARIABLE_NAME(nTimeOn)).c_str(), &nTimeOn);
	}

	unsigned int size() const {
		return ch_id ? ch_id->size() : 0;
	}

	std::vector<int>* ch_id;
	std::vector<size8>* lay;
	std::vector<size8>* strip;
	std::vector<size8>* halfStrip; //0 or 1
	std::vector<size8>* bestTime; // time bin, 0-15
	std::vector<size8>* nTimeOn; //amount of times comparator was registered as "on" in 0-15 window

};

}

#endif /*CSCUCLA_CSCDIGITUPLES_CSCINFO_H*/

