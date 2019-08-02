#ifndef CSCUCLA_CSCDIGITUPLES_CSCINFO_H
#define CSCUCLA_CSCDIGITUPLES_CSCINFO_H


/*
 *   	    Author: William Nash
 *  Date Inherited: September 11 2018
 *
 */

#include <vector>

#include <TTree.h>

#define GET_VARIABLE_NAME(Variable) (#Variable)

typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

//
//TODO: Can clean up a lot of boiler plate using boost libraries and
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

};

class Event : public Object{
public:
	Event() : Object("Event"){
		EventNumber = 0;
		RunNumber = 0;
		LumiSection = -1;
		BXCrossing = -1;
		NSegmentsInEvent = 0;
	}
	Event(TTree* t): Event(){
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(EventNumber))).c_str(),
				&EventNumber);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(RunNumber))).c_str(),
						&RunNumber);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(LumiSection))).c_str(),
						&LumiSection);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(BXCrossing))).c_str(),
						&BXCrossing);
		if(t->GetBranch((name+'_'+string(GET_VARIABLE_NAME(NSegmentsInEvent))).c_str())) { //in the event you are using legacy trees, defaults the value to 0
			t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(NSegmentsInEvent))).c_str(),
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pdg_id))).c_str(),
				&pdg_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pt))).c_str(),
				&pt);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(eta))).c_str(),
				&eta);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(phi))).c_str(),
				&phi);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(q))).c_str(),
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(),
				&ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pdg_id))).c_str(),
				&pdg_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(layer))).c_str(),
				&layer);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(energyLoss))).c_str(),
				&energyLoss);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(thetaAtEntry))).c_str(),
				&thetaAtEntry);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(phiAtEntry))).c_str(),
				&phiAtEntry);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pAtEntry))).c_str(),
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(energyEM))).c_str(),
								&energyEM);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(energyHad))).c_str(),
								&energyHad);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(eta))).c_str(),
								&eta);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(phi))).c_str(),
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pdg_id))).c_str(),
				&pdg_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(particleId))).c_str(),
						&particleId);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(eta))).c_str(),
						&eta);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(phi))).c_str(),
						&phi);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ecalEnergy))).c_str(),
						&ecalEnergy);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(hcalEnergy))).c_str(),
						&hcalEnergy);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(h0Energy))).c_str(),
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pt))).c_str(),
				&pt);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(eta))).c_str(),
				&eta);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(phi))).c_str(),
				&phi);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(q))).c_str(),
				&q);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(isGlobal))).c_str(),
				&isGlobal);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(isTracker))).c_str(),
				&isTracker);
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(mu_id))).c_str(), &mu_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(), &ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pos_x))).c_str(), &pos_x);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pos_y))).c_str(), &pos_y);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(dxdz))).c_str(), &dxdz);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(dydz))).c_str(), &dydz);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(chisq))).c_str(), &chisq);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(nHits))).c_str(), &nHits);
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(mu_id))).c_str(), &mu_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(), &ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(lay))).c_str(), &lay);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pos_x))).c_str(), &pos_x);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pos_y))).c_str(), &pos_y);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(e))).c_str(), &e);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(max_adc))).c_str(), &max_adc);
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(), &ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(quality))).c_str(), &quality);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pattern))).c_str(), &pattern);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(bend))).c_str(), &bend);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(keyWireGroup))).c_str(), &keyWireGroup);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(keyHalfStrip))).c_str(), &keyHalfStrip);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(bunchCross))).c_str(), &bunchCross);
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(), &ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(isValid))).c_str(), &isValid);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(quality))).c_str(), &quality);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(pattern))).c_str(), &pattern);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(stripType))).c_str(), &stripType);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(bend))).c_str(), &bend);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(halfStrip))).c_str(), &halfStrip);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(CFEB))).c_str(), &CFEB);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(BX))).c_str(), &BX);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(trkNumber))).c_str(), &trkNumber);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(keyStrip))).c_str(), &keyStrip);
	}

	unsigned int size() const {
		return ch_id ? ch_id->size() : 0;
	}

	//returns the amount of clcts in the chamber <chamber_index>
	unsigned int size(unsigned int chamber_index) const {
		if(!ch_id) return 0;
		unsigned int count =0;
		for(auto id : *ch_id){
			if(chamber_index == id) count++;
		}
		return count;
	}

	std::vector<size16> *ch_id;
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
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)), &ch_id);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(lay)), &lay);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(num)), &num);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ADC)), &ADC);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(L1APhase)), &L1APhase);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ADCOverflow)), &ADCOverflow);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(OverlappedSample)), &OverlappedSample);
			//t->SetBranchAddress(branchify(GET_VARIABLE_NAME(Errorstat)), &Errorstat);
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

class Comparators :public Object {
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
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(ch_id))).c_str(), &ch_id);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(lay))).c_str(), &lay);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(strip))).c_str(), &strip);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(halfStrip))).c_str(), &halfStrip);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(bestTime))).c_str(), &bestTime);
		t->SetBranchAddress((name+'_'+string(GET_VARIABLE_NAME(nTimeOn))).c_str(), &nTimeOn);
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
