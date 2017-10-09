/*
 * PatternFinder.h
 *
 *  Created on: Sep 27, 2017
 *      Author: root
 */

#ifndef PATTERNFINDER_H_
#define PATTERNFINDER_H_

#include <iostream>

using namespace std;



unsigned int MAX_ENTRY = 200000; //how many events you look at
const bool USE_COMP_HITS = 1; //false uses recHits
const int MAX_PATTERN_WIDTH = 11;
const int N_MAX_HALF_STRIPS = 2*80 + 1; //+1 from staggering of chambers
const int MAX_COMP_TIME_BIN = 15;
const int TIME_RANGE = USE_COMP_HITS ? 2: 1e9; //don't care about range for recHits
const int NLAYERS = 6; //6 layers
const int DEBUG = 0;
const bool MAKE_MATCH_LAYER_COMPARISON = true;
const int N_LAYER_REQUIREMENT = 3;
//maximum number of patterns we can have, this is currently arbitrary and has no relationship to what fits on the card
const int N_MAX_PATTERN_SET = 20;
const int TESTING_GROUP_INDEX = 3; //which of the 4 groups you are looking at (0-3)



bool IDSV1_A[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};


bool IDSV1_B[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,1,0,0},
		{1,1,1,1,1,1},
		{0,0,1,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};


bool IDSV1_C[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,1,0,0},
		{0,1,1,1,1,0},
		{0,0,1,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};

bool IDSV1_D[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,0,0,0},
		{0,1,1,0,0,0},
		{0,0,1,1,0,0},
		{0,0,0,1,1,0},
		{0,0,0,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1},
		{0,0,0,0,0,0}
};

bool IDSV1_E[MAX_PATTERN_WIDTH][NLAYERS] = {
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,0,0,0,0},
		{0,1,1,0,0,0},
		{0,0,1,0,0,0},
		{0,0,1,1,0,0},
		{0,0,0,1,0,0},
		{0,0,0,1,1,0},
		{0,0,0,0,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1}
};


struct ChamberHits {
	bool isComparator;
	unsigned int station;
	unsigned int ring;
	unsigned int endcap;
	int hits[N_MAX_HALF_STRIPS][NLAYERS];
};

struct SetMatchInfo {
	unsigned int bestLayerMatchCount;
	unsigned int bestPatternId;
	unsigned int bestSetIndex;
	unsigned int patternMatchCount[N_MAX_PATTERN_SET]; //arbitrary value could cause problems if we try larger sets!
};

class ChargePattern {
public:

	const unsigned int m_id;
	bool m_hits[MAX_PATTERN_WIDTH][NLAYERS]; //layers

	ChargePattern(unsigned int id, bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
	}
	ChargePattern(string name, unsigned int id, bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
		m_name = name;
	}


	ChargePattern(const ChargePattern &obj) : m_id(obj.m_id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = obj.m_hits[j][i];
			}
		}
		m_name = obj.m_name;
	}


	ChargePattern() :m_id(-1){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = 0;
			}
		}
		m_name = "";
	}

	~ChargePattern() {}
	ChargePattern returnFlipped(unsigned int id){
		return returnFlipped("", id);
	}
	ChargePattern returnFlipped(string name, unsigned int id){
		bool flippedPattern[MAX_PATTERN_WIDTH][NLAYERS];
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < MAX_PATTERN_WIDTH; j++){
				flippedPattern[j][i] = m_hits[MAX_PATTERN_WIDTH-1-j][i];
			}
		}
		return ChargePattern(name, id, flippedPattern);
	}

	string name() {
		if(m_name.size()) return m_name;
		string name = "";
		for(unsigned int i = 0; i< MAX_PATTERN_WIDTH; i++){
			int trueCount = 0;
			for(unsigned int j = 0; j <NLAYERS; j++){
				if(m_hits[i][j]) trueCount++;
			}
			if(trueCount < 10) name += to_string(trueCount);
			else return "ERROR";
		}
		return name;
	}

private:
	string m_name;

};

class PatternIDMatchPlots {
public:
	vector<TH1F*> m_plots;
	vector<vector<int>> m_matchIds; //tells us what pattern ids are contained in each respective plot
	const string m_name;
	TH1F* m_denominator;
	vector<ChargePattern>* m_patterns; //patterns used to test against

	PatternIDMatchPlots(string name, vector<ChargePattern>* patterns) : m_name(name){
		//m_patterns = patterns;

		m_patterns = new vector<ChargePattern>();
		for(unsigned int i =0; i < patterns->size(); i++){
			m_patterns->push_back(patterns->at(i));
		}

		//m_patterns = patterns;
		m_denominator = 0;
	}
	~PatternIDMatchPlots() {
		//should also delete all the stuff in the vector
		delete m_denominator;
	};
};


void printPattern(const ChargePattern &p) {
	printf("-- Printing Pattern: %i ---\n", p.m_id);
	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
			if(p.m_hits[x][y]) printf("X");
			else printf("~");
		}
		printf("\n");
	}
}

void printChamber(const ChamberHits &c){
	printf("==== Printing Chamber Distribution ST = %i, RI = %i, EC = %i====\n", c.station, c.ring, c.endcap);
	bool me11 = (c.station == 1 &&(c.ring == 1 || c.ring == 4));
	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int x = 0; x < N_MAX_HALF_STRIPS; x++){
			if(c.hits[x][y]) printf("%X",c.hits[x][y]-1); //print one less, so we stay in hexadecimal (0-15)
			else {
				if(!me11 && !(y%2) && x == 0){ //not me11, even, put a space on the first one
					printf(" ");
				}else if(!me11 && y%2 && x == N_MAX_HALF_STRIPS - 1){
					printf(" ");
				} else	printf("-");
			}
		}
		printf("\n");
	}
}


bool validComparatorTime(bool isComparator, int time){
	if(isComparator){
		float lowTimeLimit = 0.5*MAX_COMP_TIME_BIN - TIME_RANGE;
		float highTimeLimit = 0.5*MAX_COMP_TIME_BIN + TIME_RANGE;

		return (time -1 > lowTimeLimit) && (time - 1 < highTimeLimit);
	} else return time;
}

//looks if a chamber "c" contains a pattern "p". returns
//the maximum number of matched layers
int containsPattern(const ChamberHits &c, const ChargePattern &p){


	unsigned int maxMatchedLayers = 0;

	if(DEBUG) {
		printChamber(c);
		printPattern(p);
		if(DEBUG>1)printf("++++ Printing Match Output ++++\n");
	}

	//iterate through the entire body of the chamber, we look for overlapping patterns
	//everywhere starting at the left most edge to the rightmost edge
	for(int x = -MAX_PATTERN_WIDTH+1; x < N_MAX_HALF_STRIPS; x++){
		string printOuts[NLAYERS];

		if(DEBUG>1) {
			//fill the beginning part of the print out array
			for(int iprinty = 0; iprinty < NLAYERS; iprinty++){
				for(int iprintx = 0; iprintx < x; iprintx++){
					printOuts[iprinty] += to_string((c.hits)[iprintx][iprinty]);
				}
			}
		}


		bool matchedLayers[NLAYERS];
		for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayers[imlc] = false; //initialize

		//for each x position in the chamber, were going to iterate through
		// the pattern to see how much overlap there is at that position
		for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){
			//go through the entire vertical dimension as well
			for(unsigned int y = 0; y < NLAYERS; y++) {
				//printf("%i\n", x+px);
				//this accounts for checking patterns along the edges of the chamber that may extend
				//past the bounds
				if( (int)x+(int)px < 0 ||  x+px >= N_MAX_HALF_STRIPS) continue;


				if(validComparatorTime(c.isComparator, (c.hits)[x+px][y]) && p.m_hits[px][y]) {
					matchedLayers[y] = true;
					printOuts[y] += "$";
				} else {
					printOuts[y] += "-";
				}
			}
		}

		if(DEBUG>1) {
			//fill the rest of the print out array
			for(unsigned int iprinty = 0; iprinty < NLAYERS; iprinty++){
				for(unsigned int iprintx = x+MAX_PATTERN_WIDTH; iprintx < N_MAX_HALF_STRIPS; iprintx++){
					printOuts[iprinty] += to_string((c.hits)[iprintx][iprinty]);
				}
			}
		}

		unsigned int matchedLayerCount = 0;
		for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayerCount += matchedLayers[imlc];

		if(DEBUG>1){
			for(int iprint = 0; iprint < NLAYERS; iprint++) cout << printOuts[iprint] << endl;
			printf("for position x = %i, matchedLayerCount = %i\n",x, matchedLayerCount);
		}

		//the pattern is said to match if hits are matched within three or more layers,
		if(matchedLayerCount > maxMatchedLayers) maxMatchedLayers = matchedLayerCount;
		if(maxMatchedLayers == NLAYERS) return NLAYERS;
	}

	return maxMatchedLayers;
}


//look for the best matched pattern, when we have a set of them, and fill the set match info
int searchForMatch(const ChamberHits &c, const vector<ChargePattern>* ps, SetMatchInfo &m){
	if(ps->size() > N_MAX_PATTERN_SET) {
		printf("ERROR: Pattern size too large\n");
		return -1;
	}

	//initialize to zero
	for(unsigned int i = 0; i < N_MAX_PATTERN_SET; i++){
		m.patternMatchCount[i] = 0;
	}

	unsigned int maxMatchCount = 0;
	unsigned int maxPatternId = 0;
	unsigned int maxSetIndex = 0;

	//now we have all the rh for this segment, so check if the patterns are there
	for(unsigned int ip = 0; ip < ps->size(); ip++) {
		unsigned int thisMatchCount = containsPattern(c,ps->at(ip));

		m.patternMatchCount[ip] = thisMatchCount;

		if(thisMatchCount > maxMatchCount) {
			maxMatchCount = thisMatchCount;
			maxPatternId = ps->at(ip).m_id;
			maxSetIndex = ip;
		}
	}
	m.bestLayerMatchCount = maxMatchCount;
	m.bestPatternId = maxPatternId;
	m.bestSetIndex = maxSetIndex;
	return 0;
}





vector<ChargePattern>* createGroup1Pattern(){

	vector<ChargePattern>* thisVector = new vector<ChargePattern>();


	ChargePattern id1("100",100,IDSV1_A);
	ChargePattern id2("200",200,IDSV1_B);
	ChargePattern id3 = id2.returnFlipped("300",300);
	ChargePattern id4("400",400,IDSV1_C);
	ChargePattern id5 = id4.returnFlipped("500",500);
	ChargePattern id6("600",600,IDSV1_D);
	ChargePattern id7 = id6.returnFlipped("700",700);
	ChargePattern id8("800",800, IDSV1_E);
	ChargePattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargePattern>* createGroup2Pattern(){

	vector<ChargePattern>* thisVector = new vector<ChargePattern>();


	ChargePattern id1("100",100,IDSV1_A);
	ChargePattern id2("200",200,IDSV1_B);
	ChargePattern id3 = id2.returnFlipped("300",300);
	ChargePattern id4("400",400,IDSV1_C);
	ChargePattern id5 = id4.returnFlipped("500",500);
	ChargePattern id6("600",600,IDSV1_D);
	ChargePattern id7 = id6.returnFlipped("700",700);
	ChargePattern id8("800",800, IDSV1_E);
	ChargePattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargePattern>* createGroup3Pattern(){

	vector<ChargePattern>* thisVector = new vector<ChargePattern>();


	ChargePattern id1("100",100,IDSV1_A);
	ChargePattern id2("200",200,IDSV1_B);
	ChargePattern id3 = id2.returnFlipped("300",300);
	ChargePattern id4("400",400,IDSV1_C);
	ChargePattern id5 = id4.returnFlipped("500",500);
	ChargePattern id6("600",600,IDSV1_D);
	ChargePattern id7 = id6.returnFlipped("700",700);
	ChargePattern id8("800",800, IDSV1_E);
	ChargePattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargePattern>* createGroup4Pattern(){

	vector<ChargePattern>* thisVector = new vector<ChargePattern>();


	ChargePattern id1("100",100,IDSV1_A);
	ChargePattern id2("200",200,IDSV1_B);
	ChargePattern id3 = id2.returnFlipped("300",300);
	ChargePattern id4("400",400,IDSV1_C);
	ChargePattern id5 = id4.returnFlipped("500",500);
	ChargePattern id6("600",600,IDSV1_D);
	ChargePattern id7 = id6.returnFlipped("700",700);
	ChargePattern id8("800",800, IDSV1_E);
	ChargePattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

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
    //cout << endl << endl << "Endcap: " << ec << "  Station: " << st << "  Ring: " << ri << "  Chamber: " << ch << "  serialID: " << kSerial << endl;
    return kSerial;
}


#endif /* PATTERNFINDER_H_ */
