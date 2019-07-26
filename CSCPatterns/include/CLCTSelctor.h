/*
 * CLCTSelector.h
 *
 *  Created on: July 15, 2019
 *      Author: Siddharth Hariprakash
 */

#ifndef CSCPATTERNS_INCLUDE_CLCTSELECTOR_H_
#define CSCPATTERNS_INCLUDE_CLCTSELECTOR_H_

#include "../include/Processor.h"

class CLCTSelector : public  Processor {
	int run(std::string inputfile, std::string outputfile, int start=0, int end=-1);
};


#endif