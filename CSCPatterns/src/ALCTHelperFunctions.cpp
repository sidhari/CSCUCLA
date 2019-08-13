/* 
 * ALCTHelperFunctions.cpp
 * 
 * Created on: 7 August 2019
 *      Author: Chau Dao 
 */


#include "../include/ALCTHelperFunctions.h"
#include "../include/CSCClasses.h"

std::vector<int> pulse_to_vec (const unsigned int pulse)
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

unsigned int extend_time(const unsigned int pulse, const int p_ext)
{
    uint32_t tbit = pulse;
    std::vector<int> timevec = pulse_to_vec(pulse); 
    for (int i = 0; i<timevec.size(); i++)
    for (int i=0; i<timevec.size(); i++)
    {
        uint32_t one = 1;
        int time = timevec.at(i);
        one = one << time; 
        for (int j = 1; j<p_ext; j++)
        {
            if(time+j>15) break;
            else
            {
                one = one << 1; 
                tbit = (tbit | one);
            }
        }
    }
    return tbit;
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
            if (prev_state == 0) {std::cout << "|-"; prev_state = 1;}
            else if (prev_state == 1) std::cout << "-";
        }
        else
        {
            if (prev_state == 0) std::cout << "_"; 
            else if (prev_state == 1) {std::cout << "|_"; prev_state = 0;}
        }
        tbit = tbit >> 1; 
    }
    std::vector<int> vec_pulse = pulse_to_vec(pulse);
    for (int i=0; i<vec_pulse.size(); i++)
    {
        std::cout << vec_pulse.at(i) << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


