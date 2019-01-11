/*
 * PatternFinderClasses.cpp
 *
 *  Created on: Jul 21, 2018
 *      Author: wnash
 */


#include <stdlib.h>

#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"

#include "../include/CSCHelper.h"

//
// ComparatorCode
//

ComparatorCode::ComparatorCode(bool hits[NLAYERS][3]) {
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < 3; j++){
			_hits[i][j] = hits[i][j];
		}
	}
	//only calculate them once, to keep things efficient
	calculateId();
	calculateLayersMatched();
};

ComparatorCode::ComparatorCode(const ComparatorCode& c){
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(int j = 0; j < 3; j++){
			_hits[i][j] = c._hits[i][j];
		}
	}
	_id = c._id;
	_layersMatched = c._layersMatched;
}

ComparatorCode::ComparatorCode(){
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(int j = 0; j < 3; j++){
			_hits[i][j] = 0;
		}
	}
	//only calculate them once, to keep things efficient
	calculateId();
	calculateLayersMatched();
}

unsigned int ComparatorCode::getLayersMatched() const {
	return _layersMatched;
}

//hexidecimal code used to identify each possible arrangement of the charge in the ComparatorCode
int ComparatorCode::getId() const {
	return _id;
}

//prints the code itself, without regard to the pattern it came from
void ComparatorCode::printCode() const{
	int patCode = getId();
	printf("Comparator Code: %#04x\nLayers matched: %u\n", patCode,getLayersMatched());
	if(DEBUG > 0) cout << "ComparatorCode is: "  << bitset<12>(patCode) << endl;
	for(unsigned int i =0; i < NLAYERS; i++){
		for(int j =0; j < 3; j++){
			printf("%i", _hits[i][j]);

		}
		printf("\n");
	}
}


string ComparatorCode::getStringInBase4(int code){
	if(code > 4095) return "";
	if(code < 4){
		return to_string(code);
	}
	int rem = code % 4;
	int div = code / 4;
	return getStringInBase4(div) + to_string(rem);
}


//calculates the id based on location of hits
void ComparatorCode::calculateId(){
	//only do this iteration once, to keep things efficient
	_id = 0;
	for(unsigned int column = 0; column < NLAYERS; column++){
		int rowPat = 0; //physical arrangement of the three bits
		int rowCode = 0; //code used to identify the arrangement
		for(int row = 0; row < 3; row++){
			rowPat = rowPat << 1; //bitshift the last number to the left
			rowPat += _hits[column][row];
		}
		switch(rowPat) {
		case 0 : //000
			rowCode = 0;
			break;
		case 1 : //001
			rowCode = 1;
			break;
		case 2 : //010
			rowCode = 2;
			break;
		case 4 : //100
			rowCode = 3;
			break;
		default:
			if(DEBUG >= 0) std::cout << "Error: unknown rowPattern - " << std::bitset<3>(rowPat) << " defaulting to rowCode: 0" << std::endl;
			_id = -1;
			return;
			//rowCode = 0; //if we come up with a robust enough algorithm, we can do this eventually
			//break;
		}
		//each column has two bits of information, largest layer is most significant bit
		_id += (rowCode << 2*column);
	}
}


void ComparatorCode::calculateLayersMatched(){
	_layersMatched = 0;
	for(unsigned int i = 0; i < NLAYERS; i++){
		bool matched = false;
		for(int j = 0; j < 3; j++){
			if(_hits[i][j]) matched = 1;
		}
		_layersMatched += matched;
	}
}


//
// Pattern
//

CSCPattern::CSCPattern(unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) :
				_id(id),
				_isLegacy(isLegacy) {
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
			_pat[j][i] = pat[j][i];
		}
	}
	_name = "";
}

CSCPattern::CSCPattern(string name, unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) :
		_id(id),
		_isLegacy(isLegacy){
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
			_pat[j][i] = pat[j][i];
		}
	}
	_name = name;
}

CSCPattern::CSCPattern(const CSCPattern &obj) :
				_id(obj._id),
				_isLegacy(obj._isLegacy) {
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
			_pat[j][i] = obj._pat[j][i];
		}
	}
	_name = obj._name;
}

CSCPattern::CSCPattern() :
		_id(-1),
		_isLegacy(false) {
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
			_pat[j][i] = 0;
		}
	}
	_name = "";
}

//Create a new pattern with based off the old pattern
// by symmetrically flipping it identified by id "id"
CSCPattern CSCPattern::makeFlipped(unsigned int id) const{
	return makeFlipped("", id);
}

CSCPattern CSCPattern::makeFlipped(string name, unsigned int id) const{
	bool flippedPattern[MAX_PATTERN_WIDTH][NLAYERS];
	for(unsigned int i = 0; i < NLAYERS; i++){
		for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
			flippedPattern[j][i] = _pat[MAX_PATTERN_WIDTH-1-j][i];
		}
	}
	return CSCPattern(name, id, _isLegacy, flippedPattern);
}


//given a code, prints out how it looks within the pattern
void CSCPattern::printCode(int code) const{
	printf("For Pattern: %i - printing code: %i (layer 1->6)\n", _id,code);
	if(code >= 4096) {//2^12
		printf("Error: invalid pattern code\n");
		return;
	}

	//iterator 1
	int it = 1;

	for(unsigned int j=0; j < NLAYERS; j++){
		if(code < 0)
			for(unsigned int i=0; i < MAX_PATTERN_WIDTH; i++){
				_pat[i][j] ? printf("x") : printf("-");
			}
		else {
			//0,1,2 or 3
			int layerPattern = (code & (it | it << 1))/it;
			if(layerPattern <0 || layerPattern > 3){
				printf("Error: invalid code\n");
				return;
			}
			int trueCounter = 3;//for each layer, should only have 3
			for(unsigned int i =0; i < MAX_PATTERN_WIDTH; i++){
				if(!_pat[i][j]){
					printf("-");
				}else{
					if(trueCounter == layerPattern) printf("X");
					else printf("_");
					trueCounter--;
				}
			}
			it = it << 2; //bitshift the iterator to look at the next part of the code
		}
		printf("\n");
	}
}

//fills "code_hits" with how the code "code" would look inside the pattern
int CSCPattern::recoverPatternCCCombination(const int code, int code_hits[MAX_PATTERN_WIDTH][NLAYERS]) const{
	if(code >= 4096) {//2^12
		printf("Error: invalid pattern code\n");
		return -1;
	}

	//iterator 1
	int it = 1;

	for(unsigned int j =0; j < NLAYERS; j++){
		if(code < 0)
			for(unsigned int i=0; i < MAX_PATTERN_WIDTH; i++){
				code_hits[i][j] = _pat[i][j];
			}
		else {
			//0,1,2 or 3
			int layerPattern = (code & (it | it << 1))/it;
			if(layerPattern <0 || layerPattern > 3){
				printf("Error: invalid code\n");
				return -1;
			}
			int trueCounter = 3;//for each layer, should only have 3
			for(unsigned int i =0; i < MAX_PATTERN_WIDTH; i++){
				if(!_pat[i][j]){
					code_hits[i][j] = 0;
				}else{
					if(trueCounter == layerPattern) code_hits[i][j] = 1;
					else code_hits[i][j] = 0;
					trueCounter--;
				}
			}
			it = it << 2; //bitshift the iterator to look at the next part of the code
		}
	}

	return 0;
}

string CSCPattern::getName() const{
	if(_name.size()) return _name;
	string name = "";
	for(unsigned int i = 0; i< MAX_PATTERN_WIDTH; i++){
		long long int trueCount = 0;
		for(unsigned int j = 0; j <NLAYERS; j++){
			if(_pat[i][j]) trueCount++;
		}
		if(trueCount < 10) name += to_string(trueCount);
		else return "ERROR";
	}
	return name;
}

//
// CLCTCandidate
//

CLCTCandidate::CLCTCandidate(CSCPattern p, int horInd, int startTime, bool hits[NLAYERS][3]):
				_pattern(p),
				_horizontalIndex(horInd),
				_startTime(startTime) {
	_code = new ComparatorCode(hits);
	_layerMatchCount = _code->getLayersMatched();
	_lutEntry = 0;
}

CLCTCandidate::CLCTCandidate(CSCPattern p, int horInd, int startTime,
		int layMatCount) :
				_pattern(p),
				_horizontalIndex(horInd),
				_startTime(startTime){
	_code = 0;
	_layerMatchCount = layMatCount;
	_lutEntry = 0;
}

CLCTCandidate::~CLCTCandidate() {
	if(_code) delete _code;
}


//gets the comparator hits associated with this candidate, returns 0 if successful
int CLCTCandidate::getHits(int code_hits[MAX_PATTERN_WIDTH][NLAYERS]) const{
	return _pattern.recoverPatternCCCombination(_code->getId(),code_hits);
}


//center position of the track [strips]
float CLCTCandidate::keyStrip() const{
	//return (_horizontalIndex-1 + 0.5*(MAX_PATTERN_WIDTH - 1))/2.;
	//NOT FULLY TESTED NOV 28
	return keyHalfStrip()/2.+1;
	//return keyHalfStrip()/2+1; NOV 28 config
	//return (_horizontalIndex + (MAX_PATTERN_WIDTH+1)/2 )/2.;
}

int CLCTCandidate::keyHalfStrip() const {
	//-1 from the method of storing the comparator data, which even and odd layers are offset, needing
	// counting to start from 1 instead of 0, see "fillCompHits" in helper functions
	// TODO: should implement this more seamlessly
	return _horizontalIndex + MAX_PATTERN_WIDTH/2 - 1;
}

//local position of clct candidate, accounting for half strips and lut
float CLCTCandidate::position() const {
	if( _lutEntry){
		return keyStrip() + _lutEntry->position();
	} else {
		cout << "Warning, no lutEntry set" << endl;
		return keyStrip();
	}
}

float CLCTCandidate::slope() const {
	if( _lutEntry){
		return _lutEntry->slope();
	} else {
		cout << "Warning, no lutEntry set" << endl;
		return 0;
	}
}

void CLCTCandidate::print3x6Pattern() const{
	if(_code) _code->printCode();
	else {
		printf("Layers Match = %i\n", _layerMatchCount);
		printf("Code not available for this candidate\n");
	}
}


void CLCTCandidate::printCodeInPattern() const{
	if(!_code) return;
	printf("Horizontal index (from left) is %i half strips, position is %f\n", _horizontalIndex, keyStrip());
	for(unsigned int j=0; j < NLAYERS; j++){
		int trueCounter = 0;//for each layer, should only have 3
		for(unsigned int i =0; i < MAX_PATTERN_WIDTH; i++){
			if(!_pattern._pat[i][j]){
				printf("0");
			}else{
				if(_code->_hits[j][trueCounter]) printf("1");
				else printf("0");
				trueCounter++;
			}
		}
		printf("\n");
	}
}

int CLCTCandidate::comparatorCodeId() const{
	if(_code){
		return _code->getId();
	} else {
		return -1;
	}
}

int CLCTCandidate::layerCount() const {
	return _layerMatchCount;
}

const ComparatorCode CLCTCandidate::getComparatorCode() const {
	if(_code){
	return *_code;
	}else{
		printf("Error: no code associated with CLCT candidate\n");
		return ComparatorCode();
	}
}


int CLCTCandidate::patternId() const{
	return _pattern._id;
}

const LUTKey CLCTCandidate::key() const {
	return LUTKey(patternId(),comparatorCodeId());
}

CLCTCandidate::QUALITY_SORT CLCTCandidate::quality =
		[](CLCTCandidate* c1, CLCTCandidate* c2){

	const LUTEntry* l1 = c1->_lutEntry;
	const LUTEntry* l2 = c2->_lutEntry;

	/*We want this function to sort the CLCT's
	 * in a way that puts the best quality candidate
	 * the lowest in the list, i.e. return true
	 * if the parameters associated with c1 are
	 * better than those of c2
	 */


	// we don't have an entry for c2,
	// so take c1 as being better
	if(!l2) return true;

	// we know we have something for c2,
	// which should be by default better than nothing
	if(!l1) return false;


	//priority (layers, chi2, slope)
	if (l1->_layers > l2->_layers) return true;
	else if(l1->_layers == l2->_layers){
		if(l1->_chi2 < l2->_chi2) return true;
		else if (l1->_chi2 == l2->_chi2){
			if(abs(l1->slope()) < abs(l2->slope())) return true;
		}
	}
	return false;
};

//
// ChamberHits
//

ChamberHits::ChamberHits(unsigned int station, unsigned int ring,
		unsigned int endcap, unsigned int chamber, bool isComparator) :
				_isComparator(isComparator),
				_station(station),
				_ring(ring),
				_endcap(endcap),
				_chamber(chamber)
{
	_nhits = 0;
	_minHs = 0;
	bool me11a = _station == 1 && _ring == 4;
	bool me11b = _station == 1 && _ring == 1;
	bool me13 = _station == 1 && _ring == 3;
	if(me11a){
		_maxHs = 2*48;
	}else if (me11b || me13){
		_maxHs = 2*64;
	} else {
		_maxHs = 2*80;
	}
	for(unsigned int i =0; i < N_MAX_HALF_STRIPS; i++){
		for(unsigned int j = 0; j < NLAYERS; j++){
			_hits[i][j] = 0;
		}
	}
}

ChamberHits::ChamberHits(const ChamberHits& c) :
			_isComparator(c._isComparator),
			_station(c._station),
			_ring(c._ring),
			_endcap(c._endcap),
			_chamber(c._chamber) {
	_nhits = c._nhits;
	_minHs = c._minHs;
	_maxHs = c._maxHs;
	for(unsigned int i =0; i < N_MAX_HALF_STRIPS; i++){
		for(unsigned int j = 0; j < NLAYERS; j++){
			_hits[i][j] = c._hits[i][j];
		}
	}
}

//odd layers shift down an extra half strip
//me11a/b, and even layers are all shifted by one half strip for storage in an array
bool ChamberHits::shift(unsigned int lay) const {
	bool me11a = (_station == 1 && _ring == 4);
	bool me11b = (_station == 1 && _ring == 1);
	return me11a ||me11b || !(lay%2);
}

/* @brief fills the comparator hits class with the comparators given
 *
 */
int ChamberHits::fill(const CSCInfo::Comparators& c){

	int chSid = CSCHelper::serialize(_station, _ring, _chamber, _endcap);
	bool me11a = (_station == 1 && _ring == 4);
	bool me11b = (_station == 1 && _ring == 1);
	for(unsigned int i = 0; i < c.size(); i++){
		if(chSid != c.ch_id->at(i)) continue; //only look at where we are now
		unsigned int lay = c.lay->at(i)-1;
		unsigned int str = c.strip->at(i);
		if(str < 1) {
			printf("compStrip = %i, how did that happen?\n", str);
			return -1;
		}
		unsigned int hs = c.halfStrip->at(i);
		unsigned int timeOn = c.bestTime->at(i);
		if((me11a || me11b) && str > 64) str -= 64;

		int halfStripVal;

		halfStripVal = 2*(str-1)+hs;
		if(halfStripVal >= (int)maxHs() || halfStripVal < (int)minHs()){
			cout << "Error: hs" << halfStripVal << " outside of [" << minHs() << ", "<< maxHs() << "]" << endl;
			return -1;
		}

		halfStripVal+=shift(lay);
		if(halfStripVal < 0 || halfStripVal >= (int)N_MAX_HALF_STRIPS){
			cout << "Error: not enough allocated memory for halfstrip placement, something is wrong" << endl;
			return -1;
		}

		if(timeOn >= 16) {
			printf("Error timeOn is an invalid number: %i\n", timeOn);
			return -1;
		} else {
			_hits[halfStripVal][lay] = timeOn+1; //store +1, so we dont run into trouble with hexadecimal
			_nhits++;
		}
	}

	return 0;
}


//TODO Jan 4, 2019
int ChamberHits::fill(const CSCInfo::RecHits& r){


	int chSid = CSCHelper::serialize(_station, _ring, _chamber, _endcap);
	bool me11a = (_station == 1 && _ring == 4);
	bool me11b = (_station == 1 && _ring == 1);
	for(unsigned int thisRh = 0; thisRh < r.size(); thisRh++)
	{
		int thisId = r.ch_id->at(thisRh);

		if(chSid != thisId) continue; //just look at matches
		//rhLay goes 1-6
		unsigned int iLay = r.lay->at(thisRh)-1;

		//goes 1-80
		float thisRhPos = r.pos_x->at(thisRh);

		int iRhStrip = round(2.*thisRhPos-.5)-1; //round and shift to start at zero
		if(me11a ||me11b || !(iLay%2)) iRhStrip++; // add one to account for staggering, if even layer

		if((unsigned int)iRhStrip >= N_MAX_HALF_STRIPS || iRhStrip < 0){
			printf("ERROR: recHit index %i invalid\n", iRhStrip);
			return -1;
		}

		//_hits[iRhStrip][iLay] = true; //store +1, so we dont run into trouble with hexadecimal
		_hits[iRhStrip][iLay] = r.mu_id->at(thisRh)+2; //store +2, so we dont run into trouble with hexadecimal
		// rechits not associated with muons have mu_id = -1, and we want them to be positive so we see them -> +2
		_nhits++;
	}
	return 0;
}

void ChamberHits::print() const {
	printf("==== Printing Chamber  ST = %i, RI = %i, CH = %i, EC = %i====\n", _station, _ring, _chamber, _endcap);
	for(unsigned int y = 0; y < NLAYERS; y++) {
		if(shift(y)) printf(" ");
		for(unsigned int x = minHs() + shift(y); x < maxHs()+shift(y); x++){
			if(!((x-shift(y))%CFEB_HS)) printf("|");
			if(_hits[x][y]) printf("%X",_hits[x][y]-1); //print one less, so we stay in hexadecimal (0-15)
			else printf("-");
		}
		printf("|\n");
	}
	if(shift(0)) printf(" ");
	for(unsigned int x = minHs();x < maxHs()+1; x++){
		if(!(x%(CFEB_HS+1))) printf("%i", x/(CFEB_HS+1));
		else printf(" ");
	}
	printf("\n");
}

//takes the hits associated with clct "mi" out of the chamber
ChamberHits& ChamberHits::operator -=(const CLCTCandidate& mi) {
	const CSCPattern p = mi._pattern;
	int horPos = mi._horizontalIndex;
	int startTimeWindow = mi._startTime;


	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){

			//check if we have a 1 in our pattern
			if(p._pat[px][y]){

				//this accounts for checking patterns along the edges of the chamber that may extend
				//past the bounds
				if( horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) {
					continue;
				}
				// if there is an overlap, erase the one in the chamber
				if(validComparatorTime(_hits[horPos+px][y], startTimeWindow)) {
					_hits[horPos+px][y] = 0;
				}
			}
		}
	}
	return *this;
}
