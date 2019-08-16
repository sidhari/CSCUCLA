/* 
 * ALCTHelperFunctions.cpp
 * 
 * Created on: 7 August 2019
 *      Author: Chau Dao 
 * 
 * Adapted from: https://github.com/cms-sw/cmssw/blob/master/L1Trigger/CSCTriggerPrimitives/src/CSCAnodeLCTProcessor.cc
 */


#include "../include/ALCTHelperFunctions.h"
#include "../include/CSCClasses.h"
#include "../include/CSCConstants.h"

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

unsigned int extend_time(const unsigned int pulse, const int p_ext)
{
    uint32_t tbit = pulse;
    uint32_t one = 1; 
    for (int i = 0; i < 16; i++) 
    {
        if (tbit & one)
        {
            for (int j = 0; j<p_ext-1; j++)
            {
                one <<= 1; 
                tbit|=one; 
                i++;
                if (i>=16) return tbit;
            }
        }
        one <<= 1;
    }
  return tbit;
}

int preTrigger( int kwg, 
                const int start_bx, 
                std::vector<ALCT_ChamberHits*> &chamber_list, 
                const ALCT_Config &config)
{
    unsigned int layers_hit;
    bool hit_layer[NLAYERS];

    const unsigned int nplanes_hit_pretrig_acc = 
        (config.get_nplanes_accel_pretrig() != 0) ? 
            config.get_nplanes_accel_pretrig() : 
            config.get_nplanes_hit_pretrig();
    const unsigned int pretrig_thresh[N_ALCT_PATTERNS] = 
    {
        nplanes_hit_pretrig_acc, config.get_nplanes_hit_pretrig(), config.get_nplanes_hit_pretrig()
    };

    unsigned int stop_bx = config.get_fifo_tbins() - config.get_drift_delay();
    for (int i = start_bx; i<=stop_bx; i++)
    {
        ALCT_ChamberHits* chamber = chamber_list.at(i);
        if (chamber->isEmpty()) continue; 
        int MESelect = (chamber->_station <= 2) ? 0 : 1;
        for (int i_pattern=0; i_pattern < N_ALCT_PATTERNS; i_pattern++)
        {
            for (int i_lay = 0; i_lay<NLAYERS; i_lay++)
                hit_layer[i_lay] = false;
        
            layers_hit = 0;
            for (int i_wire = 0; i_wire < MAX_WIRES_IN_PATTERN; i_wire++)
            {
                if (pattern_mask[i_pattern][i_wire])
                {
                    this_layer = pattern_envelope[0][i_wire];
                    this_wire = pattern_envelope[1 + MESelect][i_wire] + kwg;
                }
                if (this_wire < 0 || this_wire >= chamber->get_maxWi()) continue;
                if (chamber->_hits[this_wire][this_layer] && !hit_layers[this_layer])
                {
                    hit_layers[this_layer] = true;
                    layers_hit++;
                }
                if (layers_hit >= pretrig_thresh[i_pattern]) return i;
            }
        }
    }
    return -1; 
}


