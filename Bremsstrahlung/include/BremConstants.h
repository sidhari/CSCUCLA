/*
 * BremConstants.h
 *
 *  Created on: Feb 19, 2019
 *      Author: wnash
 */

#ifndef BREMSSTRAHLUNG_INCLUDE_BREMCONSTANTS_H_
#define BREMSSTRAHLUNG_INCLUDE_BREMCONSTANTS_H_

namespace brem {
	const unsigned int NPBINS = 16;
	const float P_LOW = 0;
	const float P_HIGH = 4000;
	const unsigned int ENERGY_BINS = 50;
	const float DR_CUT = 0.3;

	//currently arbitrary, seems to capture majority of distribution
	// TODO: figure out how to do this right?
	const float ECAL_ENERGY_MAX = 20; //GeV
	const float HCAL_ENERGY_MAX = 2; //GeV
	const float ME_ENERGY_MAX = 1e-3;
}




#endif /* BREMSSTRAHLUNG_INCLUDE_BREMCONSTANTS_H_ */
