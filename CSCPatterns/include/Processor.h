/*
 * Processor.h
 *
 *  Created on: Jun 12, 2019
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_PROCESSOR_H_
#define CSCPATTERNS_INCLUDE_PROCESSOR_H_

#include <string>
#include <iostream>

using namespace std;

class Processor{
public:
	Processor(){};
	virtual int run(std::string inputfile, std::string outputfile, int start=0, int end=-1){
		std::cout << "Defined run function in derived class" << std::endl;
		return -1;
	};
	virtual ~Processor(){}

	//effectively runs main for each processor
	int main(int argc, char* argv[]);
};


#endif /* CSCPATTERNS_INCLUDE_PROCESSOR_H_ */
