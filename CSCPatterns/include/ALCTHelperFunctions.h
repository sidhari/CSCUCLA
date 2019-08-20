/* 
 * ALCTHelperFunctions.h
 * 
 * Created on: 7 August 2019
 *      Author: Chau Dao 
 */

#ifndef ALCTHELPERFUNCTIONS_H_
#define ALCTHELPERFUNCTIONS_H_

#include "../include/CSCClasses.h"
#include "../include/CSCInfo.h"

#include <math.h>

#include "TTree.h"

//******************************
// LEGACY CODE 
//******************************

const int hit_persist = 6; 

const int pattern_envelope[N_ALCT_PATTERNS][MAX_WIRES_IN_PATTERN] = 
{
    // Each digit indicates layer
    {0, 0, 1, 1, 2, 3, 4, 4, 4, 5, 5, 5},

    // Offset from KWG Number for ME1 and ME2
    {-2, 1, 0, -1, 0, 0, 0, 1, 0, 1, 2, 0, 1, 2},

    // KWG Number offset for ME3 and ME4
    {2, 1, 0, 1, 0, 0, 0, -1, 0, -1, -2, 0, -1, -2}
};

const int pattern_mask_open[N_ALCT_PATTERNS][MAX_WIRES_IN_PATTERN] =
{
    // Accelerator pattern
    {0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0},

    // Collision pattern A
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

    // Collision pattern B
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// Special option for narrow pattern for ring 1 stations
const int pattern_mask_r1[N_ALCT_PATTERNS][MAX_WIRES_IN_PATTERN] = 
{
    // Accelerator pattern
    {0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0},

    // Collision pattern A
    {0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0},

    // Collision pattern B
    {0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0}
};

class ALCTConfig
{
    public:
        ALCTConfig()
        {
            _fifo_tbins = 16;
            _fifo_pretrig = 10;
            _drift_delay = 2; 
            _nplanes_hit_pretrig = 3; 
            _nplanes_hit_pattern = 4;
            _nplanes_accel_pretrig = 3;
            _nplanes_accel_pattern = 4;
            _trig_mode = 2;
            _accel_mode = 0;
            _window_width = 7; 
            _hit_persist = 6; 
        }

        int get_fifo_tbins() const {return _fifo_tbins};
        int get_fifo_pretrig() const {return _fifo_pretrig};
        int get_drift_delay() const {return _drift_delay};
        int get_nplanes_hit_pretrig() const {return _nplanes_hit_pretrig};
        int get_nplanes_hit_pattern() const {return _nplanes_hit_pattern};
        int get_nplanes_accel_pretrig() const {return _nplanes_accel_pretrig};
        int get_nplanes_accel_pattern() const {return _nplanes_accel_pattern};
        int get_trig_mode() const {return _trig_mode};
        int get_accel_mode() const {return _accel_mode};
        int get_window_width() const {return _window_width};
        int get_hit_persist() const {return _hit_persist};
        
    private:
        int _fifo_tbins;
        int _fifo_pretrig;
        int _drift_delay;
        int _nplanes_hit_pretrig;
        int _nplanes_hit_pattern;
        int _nplanes_accel_pretrig;
        int _nplanes_accel_pattern;
        int _nplanes_accel;
        int _trig_mode;
        int _accel_mode;
        int _window_width;
        int _hit_persist. 
};

// Converts a one-shot pulse to a vector of integers, where the elements of 
// the vectors are time bins in which the wire is on
std::vector<int> pulse_to_vec(unsigned int pulse);

// print out an ASCII representation of the pulse
void print_pulse(unsigned int pulse);

// stretches a one-shot pulse to a length of p_ext time bins
// set to a constant of 6, as in data
unsigned int extend_time(const unsigned int pulse, const int p_ext=hit_persist);

// Checks whether a threshold for a wire has been passed. If so returns the 
// bunch crossing in which the threshhold was passed. Note that in this instance 
// time is zero indexed. Takes in the key wire group we want to inspect as well
// as a vector of ALCT_ChamberHits pointers, the bunch crossing we want to start on
// and the config class. Returns -1 if the pretrigger was not satisfied, returns the 
// bunch crossing at which it is satisfied otherwise
int preTrigger(int kwg, int start_bx = 0, const std::vector<ALCT_ChamberHits*> &chamber, ALCTConfig &config);

bool patternDetection(const int key_wire, const std::vector<ALCT_ChamberHits*> &chamber_list, ALCTConfig &config);



#endif 