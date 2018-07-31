/*
 * LUTClasses.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: wnash
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <set>


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
		_position(0.),
		_slope(0.),
		_nsegments(0),
		_quality(-1),
		_layers(0),
		_chi2(0){
	_calculatedMeans = false;
}

LUTEntry::LUTEntry(float position, float slope, unsigned int nsegments,
			float quality, unsigned int layers, float chi2) :
		_position(position),
		_slope(slope),
		_nsegments(nsegments),
		_quality(quality),
		_layers(layers),
		_chi2(chi2){
	_calculatedMeans = false;
}

int LUTEntry::addSegment(float positionOffset, float slopeOffset){
	if(_calculatedMeans){
		cout << "Error: already calculated means, shouldn't add more segments!" << endl;
		return -1;
	}
	_positionOffsets.push_back(positionOffset);
	_slopeOffsets.push_back(slopeOffset);
	return 0;
}

/*@brief
 *  Sort the LUT using nsegments, since it will make searching
 *  the LUT faster. This is not the same thing as quality sorting
 *  at present (July 28th, 2018).
 */
bool LUTEntry::operator<(const LUTEntry& l) const{
	return nsegments() < l.nsegments() ? false : true;
}


float LUTEntry::position() const{
	if(!_calculatedMeans && _positionOffsets.size()){
		cout << "Error: Need to run lutEntry.calculateMeans()" << endl;
	}
	return _position;
}

float LUTEntry::slope() const{
	if(!_calculatedMeans && _positionOffsets.size()){
		cout << "Error: Need to run lutEntry.calculateMeans()" << endl;
	}
	return _slope;
}

unsigned int LUTEntry::nsegments() const{
	return _positionOffsets.size()? _positionOffsets.size() : _nsegments;
}

float LUTEntry::quality() const{
	return _quality;
}

/* @brief Looks at the entries for each segments and recalculates
 * the position and slope
 */
int LUTEntry::calculateMeans(){
	unsigned int entries = _positionOffsets.size();
	if(!entries) return 0;
	if(_slopeOffsets.size() != entries){
		cout << "Error: position and slope vector different lengths" << endl;
		return -1;
	}
	float posSum = 0;
	float slopeSum = 0;
	for(unsigned int i=0; i < entries; i++){
		posSum += _positionOffsets.at(i);
		slopeSum += _slopeOffsets.at(i);
	}
	//assign new position and slope values
	_position = posSum/entries;
	_slope = slopeSum/entries;
	_nsegments = entries;
	_calculatedMeans = true;
	return 0;
}

/*@brief Reassigns quality parameter
 *
 */

int LUTEntry::calculateQuality(){
	_quality = _layers;
	return 0;
}



//
// LUT
//

LUT::LUT():
	_name("Default")
{
	_isFinal = false;
	_orderedLUT = set<pair<LUTKey,LUTEntry>, LUTLambda>(LUT_FUNT);
}

LUT::LUT(const string& name, const string& filepath):
	_name(name)
{
	_isFinal = false;
	_orderedLUT = set<pair<LUTKey,LUTEntry>, LUTLambda>(LUT_FUNT);
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
			float quality           = stof(elements[counter++]);
			unsigned int layers     = stoul(elements[counter++]);
			float chi2              = stof(elements[counter++]);


			if(DEBUG >2){
				cout << line << endl;
				cout << "patt: " << key._pattern<<
						" cc: " << key._code <<
						" pos: " << position <<
						" slope: " << slope <<
						" nseg: " << nsegments <<
						" quality: " << quality <<
						" layers: " << layers <<
						" chi2: " << chi2 << endl;
			}

			LUTEntry entry = LUTEntry(position, slope, nsegments,
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

int LUT::getEntry(const LUTKey& k, const LUTEntry*& e) const{
	if(!_isFinal) {
		cout << "Need to finalize LUT to access entries" <<endl;
		return -1;
	}
	for(auto& x: _orderedLUT){
		if(k == x.first){
			e = &(x.second);
			return 0;
		}
	}
	return -1;
}

/*@brief Looks in the LUT for the entry associated with the
 * CLCTCandidate. If successful, sets the quality parameter
 * of the CLCTCandidate
 */
/*
int LUT::getEntry(CLCTCandidate*& c, const LUTEntry*& e) const{
	if(getEntry(LUTKey(c->patternId(),c->comparatorCodeId()), e)){
		cout << "Error: can't find entry for CLCTCandidate" << endl;
		return -1;
	}
	//return c->setQuality(e->quality());
	return 0;
}
*/


/*@brief Prints the tree, up until the entries
 * are less than the min segments
 *
 */
void LUT::print(unsigned int minSegments){
	printf("\033[94m=== Printing LUT - Chamber: %s ===\033[0m\n", _name.c_str());
	printf("[%4s,%5s,%7s] -> [%8s,%6s,%5s,%5s,%8s,%8s]\n",
			"patt",
			"cc",
			"cc(b4)",
			"nseg",
			"qual",
			"lays",
			"chi2",
			"pos(s)",
			"slp(s/l)");

	if(!_isFinal) makeFinal();

	for(auto& x: _orderedLUT){
		const LUTKey& k = x.first;
		const LUTEntry& e = x.second;
		if(e.nsegments() < minSegments) break;
		printf("[%4i,%5i,%7s] -> [%8.1e,%6.2f,%5i,%5.2f,%8.3f,%8.3f]\n",
				k._pattern,
				k._code,
				ComparatorCode::getStringInBase4(k._code).c_str(),
				(double)e.nsegments(),
				e.quality(),
				e._layers,
				e._chi2,
				e.position(),
				e.slope());
	}
}

int LUT::write(const string& filename) {
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

/* @brief Once all segments have been put into the LUT,
 * this recalculates the positions / slopes and puts them
 * all in order
 */
int LUT::makeFinal(){
	if(_isFinal) return 0;
	for(auto& x: _lut) {
		x.second.calculateMeans();
		x.second.calculateQuality();
		_orderedLUT.insert(x);
	}
	_isFinal = true;
	return 0;
}


//
// DetectorLUTs
//

int DetectorLUTs::addEntry(const string& name, int station, int ring){
	auto key = make_pair(station, ring);
	auto it = _luts.find(key);
	if(it != _luts.end()){
		cout << "Warning: LUT already exists for " << station <<
				" " << ring << " , overwriting" <<endl;

	}
	_luts.insert(make_pair(key, LUT(name, LINEFIT_LUT_PATH)));
	return 0;
}

int DetectorLUTs::getLUT(int station, int ring, LUT*& lut){
	auto k = make_pair(station,ring);
	auto it = _luts.find(k);
	if(it != _luts.end()) {
		lut = &(it->second);
		return 0;
	}else return -1;
}

int DetectorLUTs::makeFinal(){
	for(auto& l: _luts) l.second.makeFinal();
	return 0;
}






