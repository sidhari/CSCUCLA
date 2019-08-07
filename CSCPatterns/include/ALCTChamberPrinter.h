/*
 * ALCTChamberPrinter.h
 *
 *  Created on: 06 August 2019
 *      Author: Chau Dao
 */

#ifndef CSCPATTERNS_INCLUDEs_ALCTChamberPrinter_H_
#define CSCPATTERNS_INCLUDE_ALCTChamberPrinter_H_

#include "../include/Processor.h"

class ALCTChamberPrinter
{
    public:
        void AlCTChamberPrinter(){};
        int run(string inputfile, unsigned int ST, unsigned int RI, unsigned int CH, unsigned int EC);
        int main(int argc, char* argv[])
        {
            std::string s(argv[1]);
            ALCTChamberPrinter::run(s,  (unsigned int) atoi(argv[2]),
                                        (unsigned int) atoi(argv[3]),
                                        (unsigned int) atoi(argv[4]),
                                        (unsigned int) atoi(argv[5]));
            return 0;
        } 
};


#endif