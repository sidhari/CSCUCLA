/*
 * PatternFinderHelperFunctions.cpp
 *
 *  Created on: Jul 21, 2018
 *      Author: wnash
 */

#include "../include/CSCHelperFunctions.h"
#include "../include/CSCHelper.h"

//#include <set>
#include <algorithm>
#include <vector>


bool validComparatorTime(const unsigned int time, const unsigned int startTimeWindow) {
	//numbers start at 1, so time bins really go 1-16 here
	for(unsigned int validTime = startTimeWindow; validTime < startTimeWindow+TIME_CAPTURE_WINDOW; validTime++){
		if(time < validTime) return false; //speed up zero case
		if(time == validTime) return true;
	}
	return false;
}

int findClosestToSegment(vector<CLCTCandidate*> matches, float segmentX){
	int closestMatchIndex = 0;
	float closestMatchSeperation = 9e9; //arbirarily large distance
	for(unsigned int imatch = 0; imatch < matches.size(); imatch++){
		if(DEBUG > 0) cout << matches.at(imatch)->keyStrip() << (imatch < matches.size() -1 ? ", " : "");
		float seperation = fabs(segmentX - matches.at(imatch)->keyStrip());
		if(seperation < closestMatchSeperation){
			closestMatchIndex = imatch;
			closestMatchSeperation = seperation;
		}
	}
	return closestMatchIndex;
}

void printPattern(const CSCPattern &p) {
	printf("-- Printing Pattern: %i ---\n", p._id);
	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
			if(p._pat[x][y]) printf("X");
			else printf("~");
		}
		printf("\n");
	}
}

//only has new patterns now
int printPatternCC(unsigned int pattID,int cc){

	//all the new patterns
	vector<CSCPattern>* newPatterns = createNewPatterns();

	for(auto patt = newPatterns->begin(); patt != newPatterns->end(); ++patt){
		if(patt->_id == pattID)	{
			patt->printCode(cc);
			break;
		}
	}

	//all the new patterns
	vector<CSCPattern>* oldPatterns = createOldPatterns();

	for(auto patt = oldPatterns->begin(); patt != oldPatterns->end(); ++patt){
		if(patt->_id == pattID)	{
			patt->printCode(cc);
			break;
		}
	}

	return 0;
}


//calculates the overlap of a pattern on a chamber at a given position and time bin window, returns layers matched
int getOverlap(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow, bool overlap[NLAYERS][3]){


	unsigned int layersMatched  = 0;
	//for each x position in the chamber, were going to iterate through
	// the pattern to see how much overlap there is at that position
	for(unsigned int y = 0; y < NLAYERS; y++) {
		bool inLayer = false;
		unsigned int overlapColumn = 0;
		for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){

			//check if we have a 1 in our Envelope
			if(p._pat[px][y]){
				if(overlapColumn >= 3){ //we are considering only patterns which, for each row, should only have at most 3 true spots
					printf("Error, we have too many true booleans in a Envelope\n");
					return -1;
				}

				//this accounts for checking patterns along the edges of the chamber that may extend
				//past the bounds
				if( horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) {
					overlap[y][overlapColumn] = false;
					overlapColumn++;
					continue;
				}

				//check the overlap of the actual chamber distribution
				if(validComparatorTime((c._hits)[horPos+px][y], startTimeWindow)) {
					overlap[y][overlapColumn] = true;
					inLayer = true;
				} else {
					overlap[y][overlapColumn] = false;
				}
				overlapColumn++; //we are now in the next column of overlap
			}
		}
		if(overlapColumn != 3) {
			printf("Error: we don't have enough true booleans in a Envelope. overlapColumn = %i\n", overlapColumn);
			return -1;
		}
		layersMatched +=inLayer;
	}
	return layersMatched;
}

//looks if a chamber "c" contains an envelope "p" at the location horPos returns
//the number of matched layers
int legacyLayersMatched(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow){

	bool matchedLayers[NLAYERS];
	for(unsigned int imlc = 0; imlc < NLAYERS; imlc++) matchedLayers[imlc] = false; //initialize

	//for each x position in the chamber, were going to iterate through
	// the pattern to see how much overlap there is at that position
	for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){
		//go through the entire vertical dimension as well
		for(unsigned int y = 0; y < NLAYERS; y++) {

			//this accounts for checking patterns along the edges of the chamber that may extend
			//past the bounds
			if( (int)horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) continue;
			if(validComparatorTime((c._hits)[horPos+px][y],startTimeWindow) && p._pat[px][y]) {
				matchedLayers[y] = true;
			}
		}
	}

	unsigned int matchedLayerCount = 0;
	for(unsigned int imlc = 0; imlc < NLAYERS; imlc++) matchedLayerCount += matchedLayers[imlc];

	return matchedLayerCount;
}

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers if ,
// run successfully, match info is stored in variable mi
// previousCandidates are a list of clcts you found earlier, which tell you which regions in the chamber not to look
int containsPattern(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi,const vector<CLCTCandidate*>&previousCandidates){

	//overlap between tested super pattern and chamber hits
	bool overlap [NLAYERS][3];
	int bestHorizontalIndex = 0;
	for(unsigned int i=0; i < NLAYERS; i++){
		for(unsigned int j =0; j < 3; j++){
			overlap[i][j] = false; //initialize all as false
		}
	}

	unsigned int maxMatchedLayers = 0;
	unsigned int time=7;//valid time starts at 7 (given first bin is 1)

	//iterate through the entire body of the chamber, we look for overlapping patterns
	//everywhere starting at the left most edge to the rightmost edge
	//for(int x = -MAX_PATTERN_WIDTH+1; x < (int)N_MAX_HALF_STRIPS; x++){

	/* Allow matches only in regions where the key half strip is within the chamber,
	 * +1 to MAX_PATTERN_WIDTH puts the key half strip at effectively 0 in the chamber
	 * since the layers are offset for non-me11a/b chambers
	 */
	for(int x = (int)c.minHs() -(int)MAX_PATTERN_WIDTH/2+1; x < (int)c.maxHs() - (int)MAX_PATTERN_WIDTH/2+1; x++){
		//check if region is in the busy window, if using old tmb logic
		bool isInBusyWindow = false;
		for(auto cand : previousCandidates){
			if(x <= cand->_horizontalIndex + (int)BUSY_WINDOW &&
					x >= cand->_horizontalIndex - (int)BUSY_WINDOW){
				isInBusyWindow = true;
				break;
			}
		}
		if(isInBusyWindow) continue;

		// Cycle through each time window, if comphits, look in all possible windows (1-4 to 13-16).
		// also ignore bins 1 & 2 if using comp hits, talk with Cameron.
		//Nov 5. - Only time bins that are used are 6,7,8,9 from zero or 7,8,9,10 here
		int matchedLayerCount = 0;
		if(p._isLegacy){
			matchedLayerCount = legacyLayersMatched(c,p,x,time);
		} else {
			matchedLayerCount = getOverlap(c,p,x,time, overlap);
			if(matchedLayerCount < 0) {
				if(DEBUG >= 0) printf("Error: cannot get overlap for pattern\n");
				return -1;
			}

		}
		//if we have a better match than we have had before
		if(matchedLayerCount == NLAYERS){ //optimization
			if(p._isLegacy){
				mi = new CLCTCandidate(p,x, time,matchedLayerCount);
			}else{
				mi = new CLCTCandidate(p,x, time,overlap);
				if(mi->comparatorCodeId() < 0) return -1;
			}
			return matchedLayerCount;
		}
		if(matchedLayerCount > (int)maxMatchedLayers) {
			maxMatchedLayers = (unsigned int)matchedLayerCount;
			bestHorizontalIndex = x;
		}
	}

	if(!p._isLegacy && getOverlap(c,p,bestHorizontalIndex,time, overlap) < 0){
		printf("Error: cannot get overlap for pattern\n");
		return -1;
	}

	if(p._isLegacy){
		mi = new CLCTCandidate(p, bestHorizontalIndex, time, maxMatchedLayers);
	}else {
		mi = new CLCTCandidate(p, bestHorizontalIndex, time, overlap);
		if(mi->comparatorCodeId() < 0) return -1;
	}
	if(DEBUG > 1){
		c.print();
		//printChamber(c);
		printPattern(p);
		mi->print3x6Pattern();
	}
	return maxMatchedLayers;
}


//look for the best matched pattern, when we have a set of them, and fill the set match info,useBusyWindow
// makes a window  of [low, high] comparator
// values of where NOT to search, following the current implementation of the TMB described here:
// https://github.com/csc-fw/otmb_fw_docs/blob/master/tmb2013-2005_spec.pdf
// note that this is currently NOT the key half strip, but some constant off of it ( MAX_PATTERN_WIDTH / 2? )
int searchForMatch(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, bool useBusyWindow){

	ChamberHits shrinkingChamber = c;

	CLCTCandidate *bestMatch = 0;

	//loop through all the patterns we have
	for(unsigned int ip = 0; ip < ps->size(); ip++) {
		CLCTCandidate *thisMatch = 0;
		if(useBusyWindow){
			//need to pass the previous candidates if using busy window, to block out region of where to look
			if(containsPattern(c,ps->at(ip),thisMatch,m) < 0) {
				if(DEBUG >= 0){
					printf("Error: pattern algorithm failed - isLegacy = %i\n", ps->at(ip)._isLegacy);
					c.print();
					//printChamber(c);
				}
				return -1;
			}
		}else{
			if(containsPattern(c,ps->at(ip),thisMatch) < 0) {
				if(DEBUG >= 0){
					printf("Error: pattern algorithm failed - isLegacy = %i\n", ps->at(ip)._isLegacy);
					c.print();
					//printChamber(c);
				}
				return -1;
			}
		}

		/*
		set<CLCTCandidate*, CLCTCandidate::QUALITY_SORT> matches =
				set<CLCTCandidate*, CLCTCandidate::QUALITY_SORT>( CLCTCandidate::cfebQuality);
		matches.insert(bestMatch);
		matches.insert(thisMatch);

		for(unsigned int im=0; im < matches.size(); im++){

		}
		bestMatch = matches.begin();
		matches.erase(matches.end()-1);
		*/




		//make a vector of matches so we can sort them according to the cfeb quality
		/*
		if(CLCTCandidate::cfebQuality(bestMatch,thisMatch)){
			delete thisMatch;
		}else{
			if(bestMatch)delete bestMatch;
			bestMatch = thisMatch;
		}
		*/

		vector<CLCTCandidate*> matches;
		matches.push_back(bestMatch);
		matches.push_back(thisMatch);

		//vector<CLCTCandidate*, CLCTCandidate::QUALITY_SORT>::iterator begin() { return
		//std::sort(matches.begin(), matches.end(), CLCTCandidate::QUALITY_SORT(CLCTCandidate::cfebQuality));
		sort(matches.begin(), matches.end(), CLCTCandidate::cfebQuality);
		//sort(matches.begin(), matches.end());


		//remove the last element from the array, i.e. the worst of the two
		matches.pop_back();

		bestMatch = matches.front();


		/*
		if(!bestMatch || bestMatch->layerCount() < thisMatch->layerCount()){
			if(bestMatch) delete bestMatch;
			bestMatch = thisMatch;
		}else{
			delete thisMatch;
		}
		*/
	}

	//we have a valid best match
	if(bestMatch && bestMatch->layerCount() >=(int) N_LAYER_REQUIREMENT){
		if(DEBUG > 0){
			c.print();
			//printChamber(c);
			printPattern(bestMatch->_pattern);
		}
		m.push_back(bestMatch);
		//WARNING: using a busy window smaller than the max pattern size may cause this emulation to perform
		// differently than expected, since we are removing hits here
		shrinkingChamber-=*bestMatch; //subtract all the hits associated with the match from the chamber
		return searchForMatch(shrinkingChamber, ps, m,useBusyWindow); //find the next one
	}else return 0; //add nothing if we don't find anything
}


int makeLUT(TTree* t, DetectorLUTs& newLUTs, DetectorLUTs& legacyLUTs){
    int patternId = 0;
    int ccId = 0;
    int legacyLctId = 0;
    int EC = 0; // 1-2
    int ST = 0; // 1-4
    int RI = 0; // 1-4
    int CH = 0;
    float segmentX = 0;
    float segmentdXdZ = 0;
    float pt = -1; //TODO
    float patX = 0;
    float legacyLctX = 0;

    t->SetBranchAddress("patternId", &patternId);
    t->SetBranchAddress("ccId", &ccId);
    t->SetBranchAddress("legacyLctId", &legacyLctId);
    t->SetBranchAddress("EC", &EC);
    t->SetBranchAddress("ST", &ST);
    t->SetBranchAddress("RI", &RI);
    t->SetBranchAddress("CH", &CH);
    t->SetBranchAddress("pt", &pt);
    t->SetBranchAddress("segmentX", &segmentX);
    t->SetBranchAddress("segmentdXdZ", &segmentdXdZ);
    t->SetBranchAddress("patX", &patX);
    t->SetBranchAddress("legacyLctX", &legacyLctX);

    for(unsigned int i = 0; i < NCHAMBERS; i++){
    	try {
    	newLUTs.addEntry(CHAMBER_NAMES[i],
    			CHAMBER_ST_RI[i][0], CHAMBER_ST_RI[i][1]);
    	legacyLUTs.addEntry(CHAMBER_NAMES[i]+LEGACY_SUFFIX,
    			CHAMBER_ST_RI[i][0], CHAMBER_ST_RI[i][1]);
    	} catch (const char* msg) {
    		cerr << msg << endl;
    		return -1;
    	}
    }

    //pointers to whatever LUT were looking at
    LUT* newLUT = 0;
    LUT* legacyLUT = 0;


    for(int i =0; i < t->GetEntriesFast(); i++){
    	t->GetEntry(i);
    	if(!(i%10000)) cout << "Loaded: " << i << "/"<< t->GetEntriesFast() << endl;

    	//skip bad patterns TODO: verify everything works correctly here!
    	if (ccId == -1) continue;

    	if(newLUTs.editLUT(ST,RI, newLUT) ||
    			legacyLUTs.editLUT(ST,RI,legacyLUT)){
    		cout << "Error: can't find chamber in lut" << endl;
    		return -1;
    	}

    	LUTKey newKey    = LUTKey(patternId, ccId);
    	LUTKey legacyKey = LUTKey(legacyLctId);

    	if(DEBUG > 0){
    		cout << "Adding Entry to LUT from ROOT Tree: patt: " << patternId <<
    				" cc: " << ccId <<
					" legacyId: " << legacyLctId << endl;
    	}

    	LUTEntry* newEntry = 0;
    	LUTEntry* legacyEntry = 0;

    	//newLUT->print();


    	if(newLUT->editEntry(newKey, newEntry) ||
    			legacyLUT->editEntry(legacyKey, legacyEntry)){
    		cout << "Error: can't get entry in LUT" << endl;
    		cout << "patt: "<<patternId << " cc: " << ccId << " legacy: " << legacyLctId << endl;
    		return -1;
    	}



    	float newPosDiff   = segmentX - patX; //strips
    	float newSlopeDiff = segmentdXdZ; //strips / layer

    	float legacyPosDiff   = segmentX - legacyLctX;
    	float legacySlopeDiff = segmentdXdZ;

    	//use multiplicity 0, as default, TODO: write correct multiplicity later
    	if(newEntry->addCLCT(0,pt,newPosDiff, newSlopeDiff) ||
    	legacyEntry->addCLCT(0,pt,legacyPosDiff, legacySlopeDiff)){
    		return -1;
    	}

    }

    //set the LUTs so they can't be changed
    newLUTs.makeFinal();
    legacyLUTs.makeFinal();

    return 0;
}


int setLUTEntries(vector<CLCTCandidate*> candidates, const DetectorLUTs& luts, int station, int ring) {
	const LUT* thisLUT = 0;
	const LUTEntry* thisEntry = 0;

	if(luts.getLUT(station,ring,thisLUT)) {
		printf("Error: can't access LUT for: %i %i\n", station,ring);
		return -1;
	}

	//TODO: make debug printout of this stuff
	for(auto & clct: candidates){
		if(thisLUT->getEntry(clct->key(), thisEntry)){
			printf("Error: unable to get entry for clct: pat: %i cc: %i\n", clct->patternId(), clct->comparatorCodeId());
			return -1;
		}
		//assign the clct the LUT entry we found to be associated with it
		clct->_lutEntry = thisEntry;
	}
	return 0;
}


//creates the new set of envelopes
vector<CSCPattern>* createNewPatterns(){

	vector<CSCPattern>* thisVector = new vector<CSCPattern>();

	CSCPattern id100("100",PATTERN_IDS[0],false,IDSV1_A);
	CSCPattern id90("90",PATTERN_IDS[1],false,IDSV1_C);
	CSCPattern id80 = id90.makeFlipped("80",PATTERN_IDS[2]);
	CSCPattern id70("70",PATTERN_IDS[3], false, IDSV1_E);
	CSCPattern id60 = id70.makeFlipped("60",PATTERN_IDS[4]);

	thisVector->push_back(id100);
	thisVector->push_back(id90);
	thisVector->push_back(id80);
	thisVector->push_back(id70);
	thisVector->push_back(id60);

	return thisVector;
}

//creates the currently implemented patterns in the TMB, here treated as envelopes
vector<CSCPattern>* createOldPatterns(){
	vector<CSCPattern>* thisVector = new vector<CSCPattern>();

	//fill in the correctly oriented matrices, should change eventually...
	//correct, tested nov 27

	for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
		for(unsigned int y = 0; y< NLAYERS; y++){
			ID2_BASE[x][y] = id2Bools[y][x];
			ID3_BASE[MAX_PATTERN_WIDTH-x-1][y] =id2Bools[y][x];
			ID4_BASE[x][y] = id4Bools[y][x];
			ID5_BASE[MAX_PATTERN_WIDTH-x-1][y] =id4Bools[y][x];
			ID6_BASE[x][y] = id6Bools[y][x];
			ID7_BASE[MAX_PATTERN_WIDTH-x-1][y] =id6Bools[y][x];
			ID8_BASE[x][y] = id8Bools[y][x];
			ID9_BASE[MAX_PATTERN_WIDTH-x-1][y] =id8Bools[y][x];
			IDA_BASE[x][y] = idABools[y][x];
		}
	}


	CSCPattern id2("ID2",2,true, ID2_BASE);
	CSCPattern id3("ID3",3,true,ID3_BASE);
	CSCPattern id4("ID4",4,true,ID4_BASE);
	CSCPattern id5("ID5",5,true,ID5_BASE);
	CSCPattern id6("ID6",6,true,ID6_BASE);
	CSCPattern id7("ID7",7,true,ID7_BASE);
	CSCPattern id8("ID8",8,true,ID8_BASE);
	CSCPattern id9("ID9",9,true,ID9_BASE);
	CSCPattern idA("IDA",10,true,IDA_BASE);

	thisVector->push_back(idA);
	thisVector->push_back(id9);
	thisVector->push_back(id8);
	thisVector->push_back(id7);
	thisVector->push_back(id6);
	thisVector->push_back(id5);
	thisVector->push_back(id4);
	thisVector->push_back(id3);
	thisVector->push_back(id2);

	return thisVector;
}



/*
 *  Writes .mem files, read by A. Pecks firmware tester.
 * Writes multiple files, each one containing all the comparator
 * hits for a given CFEB. Each file is put in the format
 *
 * <fileidentifier><CFEB number>.mem
 *
 *
 *TODO: make it so ME11A gets put in CFEBs 5-7
 */
void writeToMEMFiles(const ChamberHits& c, std::ofstream CFEBStreams[MAX_CFEBS]){
	bool me11a = c._station == 1 && c._ring == 4;
	bool me11b = c._station == 1 && c._ring == 1;
	//test using only one CFEB
	bool oneCFEB = c._station == 0 && c._ring == 0;

	//for now, only works with ME11 chambers and fake demo chamber (oneCFEB)
	if(!(me11a || me11b || oneCFEB)) return;


	for(unsigned int iCFEB=0; iCFEB < MAX_CFEBS; iCFEB++){

		/*
		A single line would be, for example,

		000000001111111122222222333333334444444455555555

		Where 00000000 are the 32 halfstrips in ly0, 11111111 are the 32 halfstrips in ly1, etc...

		So for example:

			000000010000000100000001000000010000000100000001

		corresponds to a 6 layer straight pattern on hs0.

		This array stores the correct number for each layer
		 */

		//pad with 7 empty time bins
		for(unsigned int i=0; i < 7; i++){
			CFEBStreams[iCFEB] << setw(CFEB_HS/4*NLAYERS) << setfill('0') <<0<< endl;
		}

		if(iCFEB >=  c.nCFEBs()) { //if we are writing to more CFEB files than the chamber actually has
			//fill them with zeros
			CFEBStreams[iCFEB] << setw(CFEB_HS/4*NLAYERS) << setfill('0') <<0<< endl;
		} else{
			//otherwise, do some math to convert to properly formatted line
			int comparatorLocationNumberEncoding[NLAYERS][CFEB_HS/4] = {0};
			for(unsigned int  ilay=0; ilay < NLAYERS; ilay++){
			for(unsigned int ihs=0;ihs < CFEB_HS;ihs++){ //iterate through hs within cfeb
					if(c._hits[ihs+c.shift(ilay)+iCFEB*CFEB_HS][ilay]) comparatorLocationNumberEncoding[ilay][(CFEB_HS-(ihs+1))/4] += pow(2, ihs%4);
				}
			}

			for(unsigned int i=0; i < NLAYERS;i++){
				for(unsigned int j=0; j < CFEB_HS/4; j++){
					CFEBStreams[iCFEB] <<dec << comparatorLocationNumberEncoding[i][j];
				}
			}
			CFEBStreams[iCFEB] << endl;
		}

	}
}
