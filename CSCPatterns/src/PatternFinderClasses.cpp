/*
 * PatternFinderClasses.cpp
 *
 *  Created on: Jul 21, 2018
 *      Author: wnash
 */




#include "../include/PatternFinderClasses.h"
#include "../include/PatternFinderHelperFunctions.h"



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
	_quality = -1;
}

CLCTCandidate::CLCTCandidate(CSCPattern p, int horInd, int startTime,
		int layMatCount) :
				_pattern(p),
				_horizontalIndex(horInd),
				_startTime(startTime){
	_code = 0;
	_layerMatchCount = layMatCount;
	_quality = -1;
}

CLCTCandidate::~CLCTCandidate() {
	if(_code) delete _code;
}


//center position of the track [strips]
float CLCTCandidate::x() const{
	return (_horizontalIndex-1 + 0.5*(MAX_PATTERN_WIDTH - 1))/2.;
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
	printf("Horizontal index (from left) is %i half strips, position is %f\n", _horizontalIndex, x());
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

int CLCTCandidate::setQuality(float quality){
	_quality = quality;
	return 0;
}

float CLCTCandidate::quality() const{
	return _quality;
}

bool CLCTCandidate::operator()(const CLCTCandidate& c) const{
	return quality() < c.quality();
}


//
// ChamberHits
//

ChamberHits::ChamberHits(bool isComparator, unsigned int station, unsigned int ring,
		unsigned int endcap, unsigned int chamber) :
				_isComparator(isComparator),
				_station(station),
				_ring(ring),
				_endcap(endcap),
				_chamber(chamber)
{
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
	for(unsigned int i =0; i < N_MAX_HALF_STRIPS; i++){
		for(unsigned int j = 0; j < NLAYERS; j++){
			_hits[i][j] = c._hits[i][j];
		}
	}
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
