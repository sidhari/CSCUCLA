/*
 * printPatternCC.cpp
 *
 *  Created on: Jul 28, 2018
 *      Author: wnash
 */

#include <iostream>

#include "../include/PatternFinderHelperFunctions.h"


int main(int argc, char* argv[])
{

	switch(argc){
	case 2:
		printPatternCC(atoi(argv[1]));
		break;
	case 3:
		printPatternCC(atoi(argv[1]), atoi(argv[2]));
		break;
	default:
		cout << "Gave "<< argc-1 << " arguments, usage is:" << endl;
		cout << "./printPatternCC patt (cc)" << endl;
		return -1;
	}
	return 0;
}
