/*
 * TMBEmulationTester.h
 *
 *  Created on: Jun 18, 2019
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_TMBEMULATIONTESTER_H_
#define CSCPATTERNS_INCLUDE_TMBEMULATIONTESTER_H_

#include "../include/Processor.h"

class TMBEmulationTester : public  Processor {
	int run(std::string inputfile, std::string outputfile, int start=0, int end=-1);
};



#endif /* CSCPATTERNS_INCLUDE_THREELAYERCLCTEMULATIONANALYZER_H_ */
