/*
 * BremHelperFunctions.h
 *
 *  Created on: Feb 19, 2019
 *      Author: wnash
 */

#ifndef BREMSSTRAHLUNG_INCLUDE_BREMHELPERFUNCTIONS_H_
#define BREMSSTRAHLUNG_INCLUDE_BREMHELPERFUNCTIONS_H_


const bool LIKELIHOOD_DEBUG = false;

//returns false if fails. calculates the corresponding "y" value from the line made by points 1 and 2
bool extrapolate(float x1, float y1, float x2, float y2, float x, float &y);

bool calculateMomentumEstimate(PointEstimate& est,TH1D* pLikelihood, bool debug=false);

bool calculateMomentumEstimate_QuadraticFit(PointEstimate& est,TH1D* pLikelihood, bool debug=false);

const PointEstimate ClopperPearson(float nOn, float nTotal, float alpha = 1-0.6827);

#endif /* BREMSSTRAHLUNG_INCLUDE_BREMHELPERFUNCTIONS_H_ */
