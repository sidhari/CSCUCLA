#ifndef CSCUCLA_CSCDIGITUPLES_CSCHELPER_H
#define CSCUCLA_CSCDIGITUPLES_CSCHELPER_H

//#include "DataFormats/MuonDetId/interface/CSCDetId.h"




namespace CSCHelper {


//Conversion function to check validity
template<typename Target, typename Source>
Target convertTo(Source source, const char name[], bool lenient = false, bool* good = 0)
{
  Target            converted = static_cast<Target>(source);

  if (static_cast<Source>(converted) != source) {
    const Target    lowest    = !std::numeric_limits<Target>::is_signed
                              ? 0
                              : std::numeric_limits<Target>::has_infinity
                              ? -std::numeric_limits<Target>::infinity()
                              :  std::numeric_limits<Target>::min()
                              ;

    std::string problem = "convertTo(): Source value " + std::to_string((double)  source) + " outside of target range "
                         +"["+std::to_string((double)  lowest)+","+std::to_string((double)  std::numeric_limits<Target>::max())+"]"
                         + " for '" + name +"'";

    if (good)      *good      = false;
    if (lenient) {
      std::cerr << "WARNING: " << problem << std::endl;
      return  ( source > static_cast<Source>(std::numeric_limits<Target>::max())
              ? std::numeric_limits<Target>::max()
              : lowest
              );
    }
    throw std::range_error( problem);
  }

  return converted;
}

unsigned int MAX_CHAMBER_HASH = 2048;

bool isValidChamber(unsigned int st, unsigned int ri, unsigned int ch, unsigned int ec){
	//set them to be 0 based
	 st--;ri--;ch--;ec--;
	 if(ec >= 2 || st >= 4 || ri >= 4 || ch >= 36) {
		 return false;
	 }
	 return true;
}

bool segmentIsOnEdgeOfChamber(float strip, unsigned int ST, unsigned int RI){
	if(strip < 1) return true;
	bool me11a = (ST == 1 && RI == 4);
	bool me11b = (ST == 1 && RI == 1);
	bool me13 = (ST == 1 && RI == 3);
	if(me11a){
		if(strip > 47) return true;
	} else if (me11b || me13) {
		if(strip > 63) return true;
	} else {
		if(strip > 79) return true;
	}
	return false;
}

//written to remove ambiguity between ME11A and ME11B
unsigned int serialize(unsigned int st, unsigned int ri, unsigned int ch, unsigned int ec){
	//sanity check
	if(!isValidChamber(st,ri,ch,ec)){
    	std::cout << "Error: Trying to serialize invalid chamber:" <<
    			" ST = " << st <<
    			" RI = " << ri <<
				" CH = " << ch <<
				" EC = " << ec << std::endl;
    	return 0xffffffff;
	}
	//set them to be 0 based
	st--;ri--;ch--;ec--;


	/* EC = 1 -> 2  [1 bit]
	 * ST = 1 -> 4  [2 bits]
	 * RI = 1 -> 4  [2 bits]
	 * CH = 1 -> 36 [6 bits]
	 *
	 * -> need 1+2+2+6 = 11 bits = 2 bytes -> unsigned int
	 *
	 * least significant is channel, goes up to endcap as most significant
	 */
    return  (ec << 10)|(st << 8)|(ri << 6)| ch;
}

struct ChamberId {
	unsigned int station;
	unsigned int ring;
	unsigned int chamber;
	unsigned int endcap;
};

unsigned int serialize(ChamberId id){
    unsigned int st = id.station;
    unsigned int ri = id.ring;
    unsigned int ch = id.chamber;
    unsigned int ec = id.endcap;
    return CSCHelper::serialize(st,ri,ch,ec);
}

/* @brief Unserializes chamber id. Everything <= 1
 *
 */
ChamberId unserialize(unsigned int serial){
	ChamberId c;
	c.chamber		= (serial & 0x0000003f)+1; //6 bits
	serial 			= serial >> 6;
	c.ring 			= (serial & 0x00000003)+1; //2 bits
	serial			= serial >> 2;
	c.station 		= (serial & 0x00000003)+1; //2 bits
	serial			= serial >> 2;
	c.endcap		= serial+1;
	return c;

}

/* first 128 hs are in ME11B, remaining 96 are in ME11A
 */
int MAX_ME11B_HALF_STRIP = 127;
int MAX_ME11B_STRIP = 64;

/*
//returns the bounds [lowest, highest] in strips of the given chamber.
// highest bound is an included index
std::pair<unsigned int, unsigned int> chamberBounds(ChamberId chamber){
	unsigned int lowerBound = 0;
	unsigned int upperBound = 79; //standard for majority of chambers
	bool me11a = (chamber.station == 1 && chamber.ring == 4);
	bool me11b = (chamber.station == 1 && chamber.ring ==1);
	bool me13 = (chamber.station ==1 && chamber.ring == 3);
	if(me11a) upperBound = 47;
	else if(me11b || me13) upperBound = 63;
	return make_pair(lowerBound, upperBound);
}


//returns the bounds [lowest, highest] in strips of the given chamber
std::pair<unsigned int, unsigned int> chamberBounds(unsigned int serial){
	return chamberBounds(unserialize(serial));
}
*/


};

#endif /*CSCUCLA_CSCDIGITUPLES_FILLCSCINFO_H*/
