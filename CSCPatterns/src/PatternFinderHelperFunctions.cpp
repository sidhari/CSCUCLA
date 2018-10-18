/*
 * PatternFinderHelperFunctions.cpp
 *
 *  Created on: Jul 21, 2018
 *      Author: wnash
 */


#include "../include/PatternFinderHelperFunctions.h"
#include "../../CSCDigiTuples/include/CSCHelper.h"


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

void printChamber(const ChamberHits &c){
	printf("==== Printing Chamber Distribution ST = %i, RI = %i, CH = %i, EC = %i====\n", c._station, c._ring, c._chamber, c._endcap);
	bool me11 = (c._station == 1 &&(c._ring == 1 || c._ring == 4));
	for(unsigned int y = 0; y < NLAYERS; y++) {
		if(!me11 && !(y%2)) printf(" ");
		for(unsigned int x = 0; x < N_MAX_HALF_STRIPS-1; x++){
			if(!(x%16)) printf("|");
			if(c._hits[x][y]) printf("%X",c._hits[x][y]-1); //print one less, so we stay in hexadecimal (0-15)
			else printf("-");
		}
		if(!me11 && !(y%2)) printf(" ");
		printf("\n");
	}
	for(unsigned int x = 0;x < N_MAX_HALF_STRIPS-1; x++){
		if(!(x%17)) printf("%i", x/17);
		else printf(" ");
	}
	printf("\n");
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
int containsPattern(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi){

	//overlap between tested super pattern and chamber hits
	bool overlap [NLAYERS][3];
	int bestHorizontalIndex = 0;
	for(unsigned int i=0; i < NLAYERS; i++){
		for(unsigned int j =0; j < 3; j++){
			overlap[i][j] = false; //initialize all as false
		}
	}

	unsigned int maxMatchedLayers = 0;
	unsigned int bestTimeBin = (USE_COMP_HITS ? 3 : 1); //default as 1

	//iterate through the entire body of the chamber, we look for overlapping patterns
	//everywhere starting at the left most edge to the rightmost edge
	for(int x = -MAX_PATTERN_WIDTH+1; x < (int)N_MAX_HALF_STRIPS; x++){
		// Cycle through each time window, if comphits, look in all possible windows (1-4 to 13-16).
		// also ignore bins 1 & 2 if using comp hits, talk with Cameron.
		for(unsigned int time = (USE_COMP_HITS ? 3 : 1); time < (USE_COMP_HITS ? 16 - TIME_CAPTURE_WINDOW + 2 : 2); time++){

			int matchedLayerCount = 0;

			//legacy code doesnt use overlaps, so we have a slightly different algorithm
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
				bestTimeBin = time;
			}
		}
	}

	//refill the overlap with the best found location
	if(!p._isLegacy && getOverlap(c,p,bestHorizontalIndex,bestTimeBin, overlap) < 0){
		printf("Error: cannot get overlap for pattern\n");
		return -1;
	}

	if(p._isLegacy){
		mi = new CLCTCandidate(p, bestHorizontalIndex, bestTimeBin, maxMatchedLayers);
	}else {
		mi = new CLCTCandidate(p, bestHorizontalIndex, bestTimeBin, overlap);
		if(mi->comparatorCodeId() < 0) return -1;
	}
	if(DEBUG > 1){
		printChamber(c);
		printPattern(p);
		mi->print3x6Pattern();
	}
	return maxMatchedLayers;
}

//look for the best matched pattern, when we have a set of them, and fill the set match info
int searchForMatch(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m){

	ChamberHits shrinkingChamber = c;

	CLCTCandidate *bestMatch = 0;

	//loop through all the patterns we have
	for(unsigned int ip = 0; ip < ps->size(); ip++) {
		CLCTCandidate *thisMatch = 0;
		if(containsPattern(c,ps->at(ip),thisMatch) < 0) {
			if(DEBUG >= 0){
				printf("Error: pattern algorithm failed - isLegacy = %i\n", ps->at(ip)._isLegacy);
				printChamber(c);
			}
			return -1;
		}
		if(!bestMatch || bestMatch->layerCount() < thisMatch->layerCount()){
			if(bestMatch) delete bestMatch;
			bestMatch = thisMatch;
		}else{
			delete thisMatch;
		}
	}

	//we have a valid best match
	if(bestMatch && bestMatch->layerCount() >=(int) N_LAYER_REQUIREMENT){
		if(DEBUG > 0){
			printChamber(c);
			printPattern(bestMatch->_pattern);
		}
		m.push_back(bestMatch);
		shrinkingChamber-=*bestMatch; //subtract all the hits associated with the match from the chamber
		return searchForMatch(shrinkingChamber, ps, m); //find the next one
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
    float patX = 0;
    float legacyLctX = 0;

    t->SetBranchAddress("patternId", &patternId);
    t->SetBranchAddress("ccId", &ccId);
    t->SetBranchAddress("legacyLctId", &legacyLctId);
    t->SetBranchAddress("EC", &EC);
    t->SetBranchAddress("ST", &ST);
    t->SetBranchAddress("RI", &RI);
    t->SetBranchAddress("CH", &CH);
    t->SetBranchAddress("segmentX", &segmentX);
    t->SetBranchAddress("segmentdXdZ", &segmentdXdZ);
    t->SetBranchAddress("patX", &patX);
    t->SetBranchAddress("legacyLctX", &legacyLctX);

    for(unsigned int i = 0; i < NCHAMBERS; i++){
    	newLUTs.addEntry(CHAMBER_NAMES[i],
    			CHAMBER_ST_RI[i][0], CHAMBER_ST_RI[i][1]);
    	legacyLUTs.addEntry(CHAMBER_NAMES[i]+LEGACY_SUFFIX,
    			CHAMBER_ST_RI[i][0], CHAMBER_ST_RI[i][1]);
    }

    //pointers to whatever LUT were looking at
    LUT* newLUT = 0;
    LUT* legacyLUT = 0;


    for(int i =0; i < t->GetEntriesFast(); i++){
    	t->GetEntry(i);

    	//skip bad patterns TODO: verify everything works correctly here!
    	if (ccId == -1) continue;

    	if(newLUTs.editLUT(ST,RI, newLUT) ||
    			legacyLUTs.editLUT(ST,RI,legacyLUT)){
    		cout << "Error: can't find chamber in lut" << endl;
    		return -1;
    	}

    	LUTKey newKey    = LUTKey(patternId, ccId);
    	LUTKey legacyKey = LUTKey(legacyLctId);

    	LUTEntry* newEntry = 0;
    	LUTEntry* legacyEntry = 0;


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

    	if(newEntry->addSegment(newPosDiff, newSlopeDiff) ||
    	legacyEntry->addSegment(legacyPosDiff, legacySlopeDiff)){
    		return -1;
    	}

    }

    //set the LUTs so they can't be changed
    newLUTs.makeFinal();
    legacyLUTs.makeFinal();

    return 0;
}


//creates the new set of envelopes
vector<CSCPattern>* createNewPatterns(){

	vector<CSCPattern>* thisVector = new vector<CSCPattern>();

	CSCPattern id1("100",PATTERN_IDS[0],false,IDSV1_A);
	CSCPattern id4("90",PATTERN_IDS[1],false,IDSV1_C);
	CSCPattern id5 = id4.makeFlipped("80",PATTERN_IDS[2]);
	CSCPattern id8("70",PATTERN_IDS[3], false, IDSV1_E);
	CSCPattern id9 = id8.makeFlipped("60",PATTERN_IDS[4]);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

//creates the currently implemented patterns in the TMB, here treated as envelopes
vector<CSCPattern>* createOldPatterns(){
	vector<CSCPattern>* thisVector = new vector<CSCPattern>();

	//fill in the correctly oriented matrices, should change eventually...
	for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
		for(unsigned int y = 0; y< NLAYERS; y++){
			ID2_BASE[x][y] = id2Bools[NLAYERS-1-y][x];
			ID3_BASE[MAX_PATTERN_WIDTH-x-1][y] =id2Bools[NLAYERS-1-y][x];
			ID4_BASE[x][y] = id4Bools[NLAYERS-1-y][x];
			ID5_BASE[MAX_PATTERN_WIDTH-x-1][y] =id4Bools[NLAYERS-1-y][x];
			ID6_BASE[x][y] = id6Bools[NLAYERS-1-y][x];
			ID7_BASE[MAX_PATTERN_WIDTH-x-1][y] =id6Bools[NLAYERS-1-y][x];
			ID8_BASE[x][y] = id8Bools[NLAYERS-1-y][x];
			ID9_BASE[MAX_PATTERN_WIDTH-x-1][y] =id8Bools[NLAYERS-1-y][x];
			IDA_BASE[x][y] = idABools[NLAYERS-1-y][x];
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

int chamberSerial( int ec, int st, int ri, int ch ) {

    int kSerial = ch;
    if (st == 1 && ri == 1) kSerial = ch;
    if (st == 1 && ri == 2) kSerial = ch + 36;
    if (st == 1 && ri == 3) kSerial = ch + 72;
    if (st == 1 && ri == 4) kSerial = ch;
    if (st == 2 && ri == 1) kSerial = ch + 108;
    if (st == 2 && ri == 2) kSerial = ch + 126;
    if (st == 3 && ri == 1) kSerial = ch + 162;
    if (st == 3 && ri == 2) kSerial = ch + 180;
    if (st == 4 && ri == 1) kSerial = ch + 216;
    if (st == 4 && ri == 2) kSerial = ch + 234;  // one day...
    if (ec == 2) kSerial = kSerial + 300;

    return kSerial;
}


int fillCompHits(ChamberHits& theseCompHits,
		const CSCInfo::Comparators& c) {

	unsigned int EC = (int)theseCompHits._endcap;
	unsigned int ST = (int)theseCompHits._station;
	unsigned int RI = (int)theseCompHits._ring;
	unsigned int CH = (int)theseCompHits._chamber;

	//int chSid = chamberSerial(EC, ST, RI, CH);
	int chSid = CSCHelper::serialize(ST, RI, CH, EC);

	bool me11a = (ST == 1 && RI == 4);
	bool me11b = (ST == 1 && RI == 1);
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
		//account for size of  me11a/b
		if((me11a || me11b) && str > 64) str -= 64;

		int halfStripVal;
		if(me11a ||me11b || !(lay%2)){ //if we are in me11 or an even layer (opposite from Cameron's code, since I shift to zero)
			halfStripVal = 2*(str-1)+hs+1;
		} else { //odd layers shift down an extra half strip
			halfStripVal = 2*(str-1)+hs;
		}

		if((unsigned int)halfStripVal >= N_MAX_HALF_STRIPS || halfStripVal < 0) {
			printf("Error: For compId = %i, ST=%i, RI=%i, Comp Half Strip Value out of range index = %i --- compStrip = %i, compHStrip = %i, layer = %i\n",
					chSid,ST,RI, halfStripVal, str, hs, lay);
			return -1;
		} else {
			if(timeOn >= 16) {
				printf("Error timeOn is an invalid number: %i\n", timeOn);
				return -1;
			} else {
				theseCompHits._hits[halfStripVal][lay] = timeOn+1; //store +1, so we dont run into trouble with hexadecimal
			}
		}
	/*
	for(unsigned int icomp = 0; icomp < compId->size(); icomp++){
		if(chSid != (*compId)[icomp]) continue; //only look at where we are now

		unsigned int thisCompLay = (*compLay)[icomp]-1;
		for(unsigned int icompstr = 0; icompstr < (*compStr)[icomp].size(); icompstr++){
			//goes from 1-80
			int compStrip = compStr->at(icomp).at(icompstr);
			int compHStrip = compHS->at(icomp).at(icompstr);
			if(compStrip < 1.0) printf("compStrip = %i, how did that happen?\n", compStrip);

			int timeOn = 0;

			//look at the time on value, to fill the chamber array
			if(!compTimeOn->at(icomp)[icompstr].size()){
				printf("Error dimensions of comparator time on vector are incorrect. size %lu= \n",
						compTimeOn->at(icomp)[icompstr].size());
				return -1;
			} else {
				timeOn = compTimeOn->at(icomp)[icompstr].front();
			}

			//account for weird me11a/b
			if((me11a || me11b) && compStrip > 64) compStrip -= 64;


			int halfStripVal;
			if(me11a ||me11b || !(thisCompLay%2)){ //if we are in me11 or an even layer (opposite from Cameron's code, since I shift to zero)
				halfStripVal = 2*(compStrip-1)+compHStrip+1;
			} else { //odd layers shift down an extra half strip
				halfStripVal = 2*(compStrip-1)+compHStrip;
			}


			if((unsigned int)halfStripVal >= N_MAX_HALF_STRIPS || halfStripVal < 0) {
				printf("Error: For compId = %i, ST=%i, RI=%i, Comp Half Strip Value out of range index = %i --- compStrip = %i, compHStrip = %i, layer = %i\n",
						chSid,ST,RI, halfStripVal, compStrip, compHStrip, thisCompLay);
				return -1;
			} else {
				if(timeOn < 0 || timeOn >= 16) {
					printf("Error timeOn is an invalid number: %i\n", timeOn);
					return -1;
				} else {
					theseCompHits._hits[halfStripVal][thisCompLay] = timeOn+1; //store +1, so we dont run into trouble with hexadecimal
				}
			}
		}
		*/
	}
	return 0;
}

int fillRecHits(ChamberHits& theseRecHits,
		const CSCInfo::RecHits& r){
	int EC = (int)theseRecHits._endcap;
	int ST = (int)theseRecHits._station;
	int RI = (int)theseRecHits._ring;
	int CH = (int)theseRecHits._chamber;


	int chSid = CSCHelper::serialize(ST, RI, CH, EC);
	bool me11a = (ST == 1 && RI == 4);
	bool me11b = (ST == 1 && RI == 1);
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

		theseRecHits._hits[iRhStrip][iLay] = true;
	}
	return 0;
}


