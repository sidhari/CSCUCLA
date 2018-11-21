/*
 * LUTClasses.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: wnash
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>


//TEMP
#include "TFile.h"
#include "TH1F.h"
#include "../include/PatternFinderHelperFunctions.h"


#include "../include/PatternConstants.h"
#include "../include/PatternFinderClasses.h"
#include "../include/LUTClasses.h"

using namespace std;

//
// LUTKey
//

LUTKey::LUTKey(int pattern, int code) :
		_pattern(pattern),
		_code(code),
		_isLegacy(false){

}

LUTKey::LUTKey(int pattern) :
		_pattern(pattern),
		_code(-1),
		_isLegacy(true) {

}

LUTKey::LUTKey(const LUTKey&  k) :
	_pattern(k._pattern),
	_code(k._code),
	_isLegacy(k._isLegacy){

}

bool LUTKey::operator<(const LUTKey& l) const{
	if (_pattern > l._pattern) return true;
	if (_pattern == l._pattern){
		if(_code > l._code) return true;
	}
	return false;
}

bool LUTKey::operator==(const LUTKey& l) const{
	return (_pattern == l._pattern &&
			_code == l._code &&
			_isLegacy == l._isLegacy);
}


//
// LUTEntry
//

LUTEntry::LUTEntry():
						_layers(0),
						_chi2(0),
						_position(0.),
						_slope(0.),
						_nsegments(0),
						_pt(-1), //default to non-physical pt to keep things clear
						_nclcts(0),
						_multiplicity(-1),
						_quality(-1)
{
	_isFinal = false;
}

LUTEntry::LUTEntry(float position, float slope, unsigned long nsegments, float pt, unsigned long nclcts,
		float multiplicity, float quality, unsigned int layers, float chi2) :
						_layers(layers),
						_chi2(chi2),
						_position(position),
						_slope(slope),
						_nsegments(nsegments),
						_pt(pt),
						_nclcts(nclcts),
						_multiplicity(multiplicity),
						_quality(quality){
	_isFinal = false;
}

int LUTEntry::loadTree(TTree* tree) {
	if(_isFinal) return -1;
	bool hasSegment;
	float position;
	float slope;
	float pt;
	int multiplicity;
	tree->SetBranchAddress("hasSegment", &hasSegment);
	tree->SetBranchAddress("position", &position);
	tree->SetBranchAddress("slope", &slope);
	tree->SetBranchAddress("pt",&pt);
	tree->SetBranchAddress("multiplicity", &multiplicity);

	_nclcts = tree->GetEntries();
	_nsegments = 0;
	for(unsigned int i =0; i < _nclcts; i++){
		tree->GetEntry(i);
		if(hasSegment)_nsegments++;
		_hasSegment.push_back(hasSegment);
		_positionOffsets.push_back(position);
		_slopeOffsets.push_back(slope);
		_pts.push_back(pt);
		_clctMultiplicities.push_back(multiplicity);
	}
	return makeFinal();
}

/*@brief adds a CLCT to the entry, if pt > -1., we are also adding a segment
 *
 */
int LUTEntry::addCLCT(unsigned int multiplicity, float pt,float posOffset, float slopeOffset){
	if(_isFinal){
		cout << "Error: already finalized, shouldn't add more clcts!" << endl;
		return -1;
	}

	//way to differentiate matched to segment or not
	_hasSegment.push_back(pt >= 0);
	_pts.push_back(pt);
	_positionOffsets.push_back(posOffset);
	_slopeOffsets.push_back(slopeOffset);
	_clctMultiplicities.push_back(multiplicity);
	return 0;
}

/*@brief
 *  Sort the LUT using nsegments, since it will make searching
 *  the LUT faster. This is not the same thing as quality sorting
 *  at present (July 28th, 2018).
 *
 *  Changed segments -> probability Nov 8
 */
bool LUTEntry::operator<(const LUTEntry& l) const{


	if (nclcts() < l.nclcts()) return false;
	else if (nclcts() == l.nclcts()){
	//if(multiplicity() < l.multiplicity()) return false;
	//else if (multiplicity() == l.multiplicity()) {
		if(nsegments() < l.nsegments()) return false;
		else if (nsegments() == l.nsegments()){
			return _layers < l._layers ? false : true;
		}
	}
	return true;

}

bool LUTEntry::operator==(const LUTEntry& l) const{
	return (_position == l._position &&
			_slope == l._slope &&
			_nsegments == l._nsegments &&
			_pt == l._pt &&
			nclcts() == l.nclcts() &&
			_quality == l._quality &&
			_layers == l._layers &&
			_chi2 == l._chi2 &&
			_isFinal == l._isFinal);
}


float LUTEntry::position() const{
	if(!_isFinal && _positionOffsets.size()){
		cout << "Error: Need to run lutEntry.makeFinal()" << endl;
	}
	return _position;
}

float LUTEntry::slope() const{
	if(!_isFinal && _positionOffsets.size()){
		cout << "Error: Need to run lutEntry.makeFinal()" << endl;
	}
	return _slope;
}

unsigned int LUTEntry::nsegments() const{
	if( !_hasSegment.size()) return _nsegments;

	unsigned int segments = 0;
	for (bool hasSeg : _hasSegment) segments += hasSeg;
	return segments;
}

float LUTEntry::pt() const{
	if(!_isFinal && _pts.size()){
		cout << "Error: Need to run lutEntry.makeFinal()" << endl;
	}
	return _pt;
}

unsigned int LUTEntry::nclcts() const{
	return _clctMultiplicities.size() ? _clctMultiplicities.size() : _nclcts; //vector is filled once for each time the clct shows up
}

float LUTEntry::quality() const{
	return _quality;
}

/* @brief Probability clct will later be matched to a muon
 *
 */
float LUTEntry::probability() const{
	if(nclcts()) return 1.*nsegments()/nclcts();
	else return 0;
}

/* @brief How many (on average) clcts show up in the same chamber as this one
 *
 */
float LUTEntry::multiplicity() const{
	return _multiplicity;
}

/* @brief Makes a tree out of the variables obtained from each individual clct / segment
 *
 */
TTree* LUTEntry::makeTree(const string& name) const {
	unsigned int clcts = nclcts();
	bool hasSegment;
	float position;
	float slope;
	float pt;
	int multiplicity;
	TTree* tree = new TTree(name.c_str(),name.c_str());
	tree->Branch("hasSegment",&hasSegment);
	tree->Branch("position", &position, "position/F");
	tree->Branch("slope",&slope, "slope/F");
	tree->Branch("pt",&pt, "pt/F");
	tree->Branch("multiplicity", &multiplicity, "multiplicity/I");
	if(_hasSegment.size() != clcts ||
			_positionOffsets.size() != clcts ||
			_slopeOffsets.size() != clcts ||
			_pts.size() != clcts ||
			_clctMultiplicities.size() != clcts) {
		return 0;
	}



	for(unsigned int i = 0; i < clcts; i++){
		hasSegment = _hasSegment.at(i);
		position = _positionOffsets.at(i);
		slope = _slopeOffsets.at(i);
		pt = _pts.at(i);
		multiplicity = _clctMultiplicities.at(i);
		tree->Fill();
	}
	return tree;

}

/* @brief Looks at the entries for each segments and recalculates
 * the position and slope, and find multiplicity of clcts
 */
int LUTEntry::makeFinal(){
	unsigned int clcts = nclcts();
	if(!clcts) return 0; //cant do anything
	if(_hasSegment.size() != clcts ||
			_positionOffsets.size() != clcts ||
			_slopeOffsets.size() != clcts ||
			_pts.size() != clcts ||
			_clctMultiplicities.size() != clcts) {
		return -1;
	}
	unsigned int segments = nsegments();

	float multSum = 0;
	float posSum = 0;
	float slopeSum = 0;
	float ptSum = 0;
	for(unsigned int i =0; i < clcts; i++){
		multSum += _clctMultiplicities.at(i);
		if(_hasSegment.at(i)){
			posSum += _positionOffsets.at(i);
			slopeSum += _slopeOffsets.at(i);
			ptSum += _pts.at(i);
		}
	}

	_multiplicity = multSum / clcts;
	_nclcts = clcts;
	_nsegments = segments;
	if(segments) {
		_position = posSum / segments;
		_slope = slopeSum / segments;
		_pt = ptSum / segments;
	}
	_isFinal = true;
	return 0;
}



//
// LUT
//

LUT::LUT():
	_name("Default")
{
	_isFinal = false;
	_nclcts = 0;
	_nsegments = 0;
	_sortOrder = "pclm";
	_orderedLUT = set<pair<LUTKey,LUTEntry>, LUTLambda>(_lutFunc);
}

LUT::LUT(const string& name):
	_name(name)
{
	_isFinal = false;
	_nclcts = 0;
	_nsegments = 0;
	_sortOrder = "pclmm";
	_orderedLUT = set<pair<LUTKey,LUTEntry>, LUTLambda>(_lutFunc);
}

LUT::LUT(const string& name, const string& filepath):
	_name(name)
{
	_isFinal = false;
	_sortOrder = "pclm";
	_orderedLUT = set<pair<LUTKey,LUTEntry>, LUTLambda>(_lutFunc);
	//cout << "\033[94m=== Loading LUT ===\033[0m" << endl;
	//cout << "Loading from file: " << filepath << endl;
	//this = LUT();
	string line;
	ifstream myfile(filepath.c_str());
	if(myfile.is_open()){
		while(getline(myfile, line)){
			vector<string> elements;
			char delim = ' ';
			size_t current, previous = 0;
			current = line.find(delim);
			while(current != string::npos){
				elements.push_back(line.substr(previous, current-previous));
				previous = current+1;
				current = line.find(delim,previous);
			}
			elements.push_back(line.substr(previous,current-previous));

			//how many elements we expect to get from an LUT using the new scheme
			const unsigned int sizeOfNewLUT = 9;
			const unsigned int sizeOfLegacyLUT = sizeOfNewLUT-1;


			if(!elements.size()) cout << "Error, something went wrong reading the LUT" << endl;

			unsigned int counter = 0;
			int pattern            = stoi(elements[counter++]);

			LUTKey key = LUTKey(pattern);
			if(elements.size() == sizeOfNewLUT) {
				int code = stoi(elements[counter++]);
				key = LUTKey(pattern,code);
			} else if (elements.size() != sizeOfLegacyLUT) {
				cout << "Error reading .lut file, entry has "<< elements.size() <<
						" elements" << endl;
				return;
			}

			counter++; //skip delimeter word
			float position          = stof(elements[counter++]);
			float slope             = stof(elements[counter++]);
			unsigned long nsegments = stoul(elements[counter++]);
			float pt				= 0; //TODO
			unsigned long nclcts	= 0;
			float multiplicity		= 0;
			float quality           = stof(elements[counter++]);
			unsigned int layers     = stoul(elements[counter++]);
			float chi2              = stof(elements[counter++]);


			if(DEBUG >2){
				cout << line << endl;
				cout << "patt: " << key._pattern<<
						" cc: " << key._code <<
						" pos: " << position << //strips
						" slope: " << slope << //strip /layer
						" nseg: " << nsegments <<
						" pt " << pt << //GeV
						" nclcts: " << nclcts <<
						" mult: " << multiplicity <<
						" quality: " << quality <<
						" layers: " << layers <<
						" chi2: " << chi2 << endl;
			}

			LUTEntry entry = LUTEntry(position, slope, nsegments, pt, nclcts, multiplicity,
					quality, layers, chi2);
			setEntry(key,entry);
		}
		myfile.close();
		//cout << "Completed loading of line fit table" << endl;
	} else {
		cout << "Error: unable to open file:" << filepath << endl;
	}
}

int LUT::setEntry(const LUTKey& k,const LUTEntry& e){
	if(_isFinal) {
		return -1;
	}
	_lut.insert(make_pair(k,e));
	return 0;
}

/* @brief Takes a key "k" and a reference to an entry "e", which
 * is filled. The function returns 0 if the lut has the entry,
 * and -1 if it does not
 */
int LUT::editEntry(const LUTKey& k, LUTEntry*& e) {
	if(_isFinal) return -1;
	auto it = _lut.find(k);
	if(it != _lut.end()) {
		e = &(it->second);
		return 0;
	}
	if(k._isLegacy){ //no default setting for legacy lut
		setEntry(k, LUTEntry());
		return editEntry(k,e);
	} else { //all other patterns should have a default
		return -1;
	}
}

int LUT::getEntry(const LUTKey& k, const LUTEntry*& e, bool debug) const{
	if(!_isFinal) {
		cout << "Need to finalize LUT to access entries" <<endl;
		return -1;
	}
	if(debug) cout << "Looking for [ " << k._pattern << ", " << k._code << "]" << endl;
	for(auto& x: _orderedLUT){
		if(debug) cout << "[" << x.first._pattern << ", " << x.first._code << "]: " <<
				"[qual = " << x.second.quality() << "]" << endl;

		if(k == x.first){
			e = &(x.second);
			return 0;
		}
	}
	return -1;
}


/*@brief Prints the tree, up until the entries
 * are less than the min segments
 *
 */
void LUT::print(unsigned int minClcts,unsigned int minSegments,unsigned int minLayers){
	printf("\033[94m=== Printing LUT - Chamber: %s ===\033[0m\n", _name.c_str());
	printf("[%4s,%5s,%7s] -> [%8s,%8s,%8s,%6s,%8s,%6s,%6s,%5s,%5s,%8s,%8s]\n",
			"patt",
			"cc",
			"cc(b4)",
			"P(mu|cc)",
			//"P(cc|mu)",
			"P(cc)",
			//"prob",
			"nseg",
			"pt",
			"nclct",
			"mult",
			"qual",
			"lays",
			"chi2",
			"pos(s)",
			"slp(s/l)");

	if(!_isFinal) makeFinal();


	/* calculate the normalization, bayes theorem sayes
	 *
	 * P(mu | code) = P(code | mu)* P(mu) / P(code)
	 *
	 * We have
	 *
	 * P(code) 		= nclcts(code) / nclcts(total)
	 * P(code | mu) = nsegments(code) / nsegments(total)
	 *
	 * We know
	 *
	 * SUM P(mu|code) = 1
	 *
	 * so
	 *
	 * P(mu) SUM P(code | mu) / P(code) = 1
	 * -> P(mu) = 1 / SUM P(code | mu) / P(code)
	 *
	 * call norm = SUM P(code | mu) / P(code)
	 */
/*
	double norm = 0;
	for(auto& x: _orderedLUT){
		//probability the code shows up at all
		double p_cc = (double)x.second.nclcts()/nclcts();
		//probability the code shows up, given we have a real muon
		double p_cc_mu = (double)x.second.nsegments()/nsegments();
		norm += p_cc ? p_cc_mu/p_cc : 0.;
	}
	double p_mu = 1./norm;
*/

	for(auto& x: _orderedLUT){
		const LUTKey& k = x.first;
		const LUTEntry& e = x.second;

		if(e.nclcts() < minClcts) continue;
		if(e.nsegments() < minSegments) continue;
		if(e._layers < minLayers) continue;

		//probability the code shows up at all
		double p_cc = nclcts() ? (double)x.second.nclcts()/nclcts() : 0;
		//probability the code shows up, given we have a real muon
		double p_mu_cc = x.second.nclcts()?(double)x.second.nsegments()/x.second.nclcts() : 0;

		//double p_mu_cc = p_cc ? p_cc_mu*p_mu/p_cc : 0.;

		printf("[%4i,%5i,%7s] -> [%8.4f,%8.4f,%8.1e,%6.2f,%8.1e,%6.2f,%6.2f,%5i,%5.2f,%8.3f,%8.3f]\n",
				k._pattern,
				k._code,
				ComparatorCode::getStringInBase4(k._code).c_str(),
				e.probability(),
				//p_mu_cc,
				//p_cc_mu,
				p_cc,
				(double)e.nsegments(),
				e.pt(),
				(double)e.nclcts(),
				e.multiplicity(),
				e.quality(),
				e._layers,
				e._chi2,
				e.position(),
				e.slope());
	}
	//printf("P(mu) = %3.3f\n", p_mu);
}


int LUT::loadROOT(const string& rootfile) {
	cout << "\033[94m=== Loading LUT ===\033[0m" << endl;
	cout << "Loading file: " << rootfile << endl;
	TFile* f = TFile::Open(rootfile.c_str());
	if(!f) return -1;

	unsigned int count = 0;
	for(auto& it: _lut){
		count++;
		if(count % (_lut.size()/10) == 0) cout << "Processed " << round(100.*count/_lut.size()) << " %" << endl;

		int patt = it.first._pattern;
		int cc = it.first._code;
		string treeName = string("p" + to_string(patt) + "_cc" + to_string(cc));
		TTree* t = (TTree*)f->Get(treeName.c_str());
		if(!t) return -1;
		if(it.second.loadTree(t)) return -1;
	}

	return 0;
}

int LUT::writeToText(const string& filename) {
		cout << "\033[94m=== Writing LUT ===\033[0m" << endl;
		cout << "Writing to file: " << filename << endl;
		ofstream myfile;
		myfile.open(filename);
		if(!myfile.is_open()){
			cout << "Error: can't write file" << endl;
			return -1;
		}
		if(!_isFinal)makeFinal();

		for(auto& it : _orderedLUT){

			// pat code - pos slope nseg qual layers chi2
			myfile << it.first._pattern << " ";
			if(!it.first._isLegacy) myfile << it.first._code << " ";
			myfile << "~ ";
			myfile << it.second.position() << " ";
			myfile << it.second.slope() << " ";
			myfile << it.second.nsegments() << " ";
			myfile << it.second.quality() << " ";
			myfile << it.second._layers << " ";
			myfile << it.second._chi2 << endl;
		}

		myfile.close();
		return 0;

}

int LUT::writeToROOT(const string& filename){
	if(!_isFinal)makeFinal();

	cout << "\033[94m=== Writing LUT ===\033[0m" << endl;
	cout << "Writing to file: " << filename << endl;
	TFile * outF = new TFile(filename.c_str(),"RECREATE");
	if(!outF){
		printf("Failed to open output file: %s\n", filename.c_str());
		return -1;
	}


	outF->cd();
	for(auto& it :_orderedLUT){
		int patt = it.first._pattern;
		int cc = it.first._code;
		string treeName = string("p" + to_string(patt) + "_cc" + to_string(cc));
		TTree* thisTree = it.second.makeTree(treeName);
		thisTree->Write();
	}
	outF->Close();
	cout << "LUT Write completed" << endl;

	return 0;
}

/* @brief Writes pattern Specific LUTs (PSLs),
 * readable by the OTMB. One for each pattern
 */
int LUT::writeToPSLs(const string& fileprefix){
	cout << "\033[94m=== Writing PSL ===\033[0m" << endl;
	if(!_isFinal)makeFinal();

	/* Stored if we every need these plots later...
	cout << "writing test.root file" << endl;
	TFile * outF = new TFile("temp.root","RECREATE");
	TH1F* h_poffsets = new TH1F("pos_offsets","Linear Fit Position Offsets", 200, -2, 2);
	h_poffsets->GetXaxis()->SetTitle("Centered Segment Offset [half-strips]");
	h_poffsets->GetYaxis()->SetTitle("Comparator Codes");

	TH1F* h_soffsets = new TH1F("slope_offsets", "Linear Fit Slope Offsets", 200, -2, 2);
	h_soffsets->GetXaxis()->SetTitle("Segment Slope Offset [ hs / layer]");
	h_soffsets->GetYaxis()->SetTitle("Comparator Codes");
	 */


	for(unsigned int i = 0; i < NPATTERNS; i++){
		unsigned int pattern = PATTERN_IDS[i];
		string thisPattFilename = fileprefix + "-" + to_string(pattern) + ".psl";
		cout << "Writing to file: " << thisPattFilename << endl;
		ofstream outfile;
		outfile.open(thisPattFilename);
		if(!outfile){
			cerr << "Error: can't write file" << endl;
			return -1;
		}

		for(unsigned int ccode = 0; ccode < NCOMPARATOR_CODES; ccode++){
			unsigned int outnum = 0;
			const LUTEntry* e = 0;
			if(getEntry(LUTKey(pattern,ccode), e) || (e && e->_layers < 3)){
				//we couldn't find the key in the LUT
				outnum = convertToPSLLine(LUTEntry());
			}else {
				outnum = convertToPSLLine(*e);
				/*
				h_poffsets->Fill(2.*e->position()-0.5);
				h_soffsets->Fill(2.*e->slope());
				if(abs(2.*e->position()-0.5) > 2){
					printPatternCC(pattern, (int) ccode);
				}
				*/
			}

			outfile << hex << setw(5) << setfill('0')  << outnum << endl;
		}
		outfile.close();
	}
	/*
	h_poffsets->Write();
	h_soffsets->Write();
	outF->Close();
	*/
	return 0;
}

/* @brief Once all segments have been put into the LUT,
 * this recalculates the positions / slopes and puts them
 * all in order
 */
int LUT::makeFinal(){
	if(_isFinal) return 0;
	_nclcts = 0;
	_nsegments = 0;
	for(auto& x: _lut) {
		x.second.makeFinal();
		_nclcts += x.second.nclcts();
		_nsegments += x.second.nsegments();
		_orderedLUT.insert(x);
	}
	_isFinal = true;
	return 0;
}

int LUT::sort(const string& sortOrder){
	if(makeFinal()) return -1;

	//TODO: could be done in a nicer way...
	_sortOrder = sortOrder;

	_orderedLUT.clear();

	for(auto& x: _lut){
		_orderedLUT.insert(x);
	}
	//_orderedLUT = newOrderedLUT;

	return 0;
}

/*
 * @brief Total amount of clcts used in LUT creation
 */
int LUT::nclcts() {
	if(!_isFinal) makeFinal();
	return _nclcts;
}

/*
 * @brief Total amount of segments used in LUT creation
 */
int LUT::nsegments() {
	if(!_isFinal) makeFinal();
	return _nsegments;
}

int LUT::convertToPSLLine(const LUTEntry& e){
	if(e == LUTEntry()){ //we just have the default constructor
		return 0;
	}


	const unsigned int nOutBits = 18; //amount of bits we can write to

	// See email "CLCT output to Track Finder"
	// Need position offset range of [-2,2] hs, and 0.125 half-strip resolution to make proper use of our new scheme
	// [2 - (-2)]/ 0.25 = 16 -> 4 bits
	const unsigned int positionRange = 2;
	const unsigned int nPositionBits = 4;
	float epsilon = 0.00001; // for putting quantities just below maxmimum
	//const unsigned int positionBins = (2*positionRange)*(nPositionBits);

	//convert to halfstrips, and center distribution in positive region (don't have to worry about signs) for shipping
	float centeredHsPosition = 2.*e.position()-0.5 + positionRange;
	if(centeredHsPosition < 0) centeredHsPosition = 0; //cut things outside bounds, shouldn't be many
	else if(centeredHsPosition >= 2.*positionRange) centeredHsPosition = 2.*positionRange-epsilon;
	//make it fit within 4 bits, this expression seems a bit shaky... only works for even ranges?
	unsigned int positionBits = (unsigned int)floor(centeredHsPosition*pow(2, nPositionBits-positionRange));

	//
	//Need slope offset range of [-2,2] hs/layer and 0.125 hs/layer resolution
	// [2- (-2)] / 0.125 = 32 -> 5 bits

	const unsigned int slopeRange = 2;
	const unsigned int nSlopeBits = 5;
	float centeredHsSlope = 2.*e.slope() + slopeRange;
	if(centeredHsSlope < 0) {
		//cout << centeredHsSlope << endl;
		centeredHsSlope = 0;
	}
	else if(centeredHsSlope >=2.*slopeRange) {
		centeredHsSlope = 2.*slopeRange-epsilon;
	}

	unsigned int slopeBits = (unsigned int)floor(centeredHsSlope*pow(2,nSlopeBits-slopeRange));


	//
	// use 9 bits to store quality
	// temporarily just the number of layers in the code
	//
	const unsigned int nQualityBits = nOutBits - nSlopeBits - nPositionBits;
	unsigned int qualityBits = e._layers;

	//bits we will eventually convert to the string we write
	unsigned int outBits = (positionBits << (nOutBits-nPositionBits)) | (slopeBits << (nOutBits-nPositionBits-nSlopeBits)) | qualityBits;
	//if(DEBUG > 3){
	if(outBits > 262143){
		bitset<nPositionBits> p(positionBits);
		bitset<nSlopeBits> s(slopeBits);
		bitset<nQualityBits> q(qualityBits);
		cout << "pos: " << p << " slope: " << s << " qual: "<< q << endl;
		bitset<nOutBits> o(outBits);
		cout << "out: " << o << endl;
		cout << outBits << endl;
	}

	return outBits;
}


//
// DetectorLUTs
//

DetectorLUTs::DetectorLUTs(bool isLegacy):
	_isLegacy(isLegacy){
}

/*@brief Takes a path that contains all the necessary LUTs needed
 * to create the LUTs
 *
 */
int DetectorLUTs::loadAll(const string& path){
	for(unsigned int i = 0; i < NCHAMBERS; i++){
		string filepath = path+CHAMBER_NAMES[i];
		if(_isLegacy) filepath += LEGACY_SUFFIX;
		filepath += ".lut";
		if(addEntry(CHAMBER_NAMES[i],
				CHAMBER_ST_RI[i][0],CHAMBER_ST_RI[i][1],
				filepath)) {
			cout << "Error can't add file:" << filepath << endl;
			return -1;
		}
	}
	return 0;
}

int DetectorLUTs::addEntry(const string& name, int station, int ring, const string& lutpath){
	auto key = make_pair(station, ring);
	auto it = _luts.find(key);
	if(it != _luts.end()){
		cout << "Warning: LUT already exists for " << station <<
				" " << ring << " , overwriting" <<endl;

	}
	//default to line fits if not legacy
	if(!_isLegacy){
		_luts.insert(make_pair(key, LUT(name, lutpath)));
	} else {
		_luts.insert(make_pair(key, LUT(name)));
	}
	return 0;
}


int DetectorLUTs::editLUT(int station, int ring, LUT*& lut){
	auto k = make_pair(station,ring);
	auto it = _luts.find(k);
	if(it != _luts.end()) {
		it->second.makeFinal();
		lut = &(it->second);
		return 0;
	}else return -1;
}

int DetectorLUTs::getLUT(int station, int ring, const LUT*& lut){
	LUT* unconstLUT = 0;
	if(editLUT(station,ring, unconstLUT)){
		return -1;
	}
	lut = (const LUT*&)unconstLUT;
	return 0;
}


int DetectorLUTs::makeFinal(){
	for(auto& l: _luts) l.second.makeFinal();
	return 0;
}


unsigned int DetectorLUTs::size() const {
	return _luts.size();
}

int DetectorLUTs::writeAll(const string& path) {
	//TODO: make it so we have a bool that keeps track of if its final
	makeFinal();
	for(auto& l : _luts){
		l.second.writeToText(path+l.second._name+".lut");
	}
	return 0;
}




