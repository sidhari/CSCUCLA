/* 
 * ALCTHelperFunctions.cpp
 * 
 * Created on: 7 August 2019
 *      Author: Chau Dao 
 */


#include "../include/ALCTHelperFunctions.h"
#include "../include/CSCClasses.h"

std::vector<int> pulse_to_vec (unsigned int pulse)
{
    std::vector<int> tbins;
    uint32_t tbit = pulse;
    uint32_t one = 1;
    for (int i = 0; i < 32; i++) 
    {
        if (tbit & one) tbins.push_back(i);
        tbit = tbit >> 1;
        if (tbit == 0) break;
    }
  return tbins;
}

void print_pulse(unsigned int pulse)
{   
    uint32_t tbit = pulse;
    uint32_t one = 1;
    std::cout << "Begin:";
    int prev_state = 0;
    for (int i = 0; i < 32; i++)
    {
        if (tbit & one)
        {   
            if (prev_state == 0) std::cout << "|-"; prev_state = 1;
            if (prev_state == 1) std::cout << "-";
        }
        else
        {
            if (prev_state == 0) std::cout << "_"; 
            if (prev_state == 1) std::cout << "|_"; prev_state = 0;
        }
        tbit = tbit >> 1; 
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
