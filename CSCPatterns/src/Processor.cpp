/*
 * Processor.cpp
 *
 *  Created on: Jun 12, 2019
 *      Author: wnash
 */

#include "../include/Processor.h"

#include <iostream>
#include <stdio.h>
#include <chrono>
#include <time.h>


//define main function here to be used by processors that inherit from this class

int Processor::main(int argc, char* argv[])
{
	auto t1 = std::chrono::high_resolution_clock::now();

	try {
		switch(argc){
		case 3:
			return run(string(argv[1]), string(argv[2]));
			break;
		case 4:
			return run(string(argv[1]), string(argv[2]),0, atoi(argv[3]));
			break;
		case 5:
			return run(string(argv[1]), string(argv[2]),atoi(argv[3]), atoi(argv[4]));
			break;
		default:
			std::cout << "Gave "<< argc-1 << " arguments, usage is:" << std::endl;
			std::cout << "./<Processor> inputFile outputFile (events)" << std::endl;
			return -1;
		}
	}catch( const char* msg) {
		std::cerr << "ERROR: " << msg << std::endl;
		return -1;
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	cout << "Time elapsed: " << chrono::duration_cast<chrono::seconds>(t2-t1).count() << " s" << endl;


	return 0;
}

