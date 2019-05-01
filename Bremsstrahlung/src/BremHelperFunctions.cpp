/*
 * BremHelperFunctions.cpp
 *
 *  Created on: Feb 19, 2019
 *      Author: wnash
 */

#include <TH1F.h>
#include <TF1.h>
#include "Math/DistFunc.h"

#include "../include/BremClasses.h"
#include "../include/BremHelperFunctions.h"

bool extrapolate(float x1, float y1, float x2, float y2, float x, float &y){
	if(x1==x2) return false;
	float m = (y1-y2)/(x1-x2);
	float b = y1 - m*x1;

	y = m*x+b;
	return true;
}


bool calculateMomentumEstimate(PointEstimate& est,TH1D* pLikelihood, bool debug) {
	const unsigned int mostProbableBin = pLikelihood->GetMaximumBin();
	float mostProbableP = pLikelihood->GetBinCenter(mostProbableBin);
	float likelihoodAtMostProbableP = pLikelihood->GetMaximum();
	float minLnLikelihood = -2.*TMath::Log(likelihoodAtMostProbableP);
	if(debug) cout << "Most Probable P: " << mostProbableP << endl;

	//make -2lnL distributions
	TH1F* lnLikelihood = (TH1F*)pLikelihood->Clone(string("h_likelihood").c_str());
	for(unsigned int i=0; (int)i < lnLikelihood->GetNbinsX()+1; i++){
		lnLikelihood->SetBinContent(i, -2.*TMath::Log(lnLikelihood->GetBinContent(i)));
	}

	const float Delta_CHI2 = 1; //one parameter of interest, so need log likelihood well to rise by 1

	//initialize to edges of histograms
	float lowEdgeP =pLikelihood->GetBinLowEdge(1);
	//float lowEdgeP = pLikelihood->GetBinCenter(1);
	float highEdgeP = pLikelihood->GetBinLowEdge(pLikelihood->GetNbinsX()+1);
	//float highEdgeP = pLikelihood->GetBinCenter(pLikelihood->GetNbinsX());

	//find lower bound of confidence interval
	for(unsigned int ibin =mostProbableBin-1; ibin > 0; ibin--){
		float likelihood = pLikelihood->GetBinContent(ibin);
		float lnLikelihood = -2*TMath::Log(likelihood);
		float deltaLnLikelihood = lnLikelihood - minLnLikelihood;

		lowEdgeP = pLikelihood->GetBinLowEdge(ibin);
		//lowEdgeP = pLikelihood->GetBinCenter(ibin);
		//cout << "lowEdge: " << lowEdgeP << " deltaLnLikelihood = " << deltaLnLikelihood << endl;
		if(deltaLnLikelihood > Delta_CHI2){
			//interpolate
			//float lowEdgePPrev = pLikelihood->GetBinCenter(ibin+1);
			float lowEdgePPrev = pLikelihood->GetBinLowEdge(ibin+1);
			float likelihoodPrev = pLikelihood->GetBinContent(ibin+1);
			float lnLikelihoodPrev = -2*TMath::Log(likelihoodPrev);
			float deltaLnLikelihoodPrev = lnLikelihoodPrev - minLnLikelihood;

			//interpolate to where exactly we hit -2*Dln(L) = 1, assign to lowEdgeP
			extrapolate(deltaLnLikelihoodPrev, lowEdgePPrev,
					deltaLnLikelihood,lowEdgeP,
					1, lowEdgeP);
			break;
		}
	}


	//find upper bound of confidence interval
	for(unsigned int ibin =mostProbableBin+1; (int)ibin < pLikelihood->GetNbinsX()+1; ibin++){
		float likelihood = pLikelihood->GetBinContent(ibin);
		float lnLikelihood = -2*TMath::Log(likelihood);
		float deltaLnLikelihood = lnLikelihood -minLnLikelihood ;

		highEdgeP = pLikelihood->GetBinLowEdge(ibin+1);
		//highEdgeP = pLikelihood->GetBinCenter(ibin);
		//cout << "highEdge: " << highEdgeP << " deltaLnLikelihood = " << deltaLnLikelihood << endl;
		if(deltaLnLikelihood > Delta_CHI2){
			//float highEdgePPrev = pLikelihood->GetBinCenter(ibin-1);
			float highEdgePPrev = pLikelihood->GetBinLowEdge(ibin);
			float likelihoodPrev = pLikelihood->GetBinContent(ibin-1);
			float lnLikelihoodPrev = -2*TMath::Log(likelihoodPrev);
			float deltaLnLikelihoodPrev = lnLikelihoodPrev - minLnLikelihood;

			//interpolate to where exactly we hit -2*Dln(L) = 1, assign to lowEdgeP
			extrapolate(deltaLnLikelihoodPrev, highEdgePPrev,
					deltaLnLikelihood,highEdgeP,
					1, highEdgeP);
			break;
		}
	}

	//float sigmaEst = (highEdgeP-lowEdgeP)/2.;


	est.set(mostProbableP,lowEdgeP, highEdgeP);


	delete lnLikelihood;
	return true;
}




/* pass reference to point estimate, looks as histogram "pLikelihood"
 * to create estimate, returns true or false if fit works.
 *
 */
bool calculateMomentumEstimate_QuadraticFit(PointEstimate& est,TH1D* pLikelihood, bool debug) {
	if(debug) cout << "\t== Calculating Momentum Estimate ==" << endl;

	//bottom of well
	const unsigned int mostProbableBin = pLikelihood->GetMinimumBin();


	//guess as the center of the most probable bin to start
	float pEst = pLikelihood->GetBinCenter(mostProbableBin);

	//range of where to fit parabola

	const unsigned int binsToFit = 7; //3 to left, 3 to right, of bottom

	int lowerEdgeBin = mostProbableBin - (binsToFit-1)/2;
	int upperEdgeBin = mostProbableBin + (binsToFit-1)/2;

	if(lowerEdgeBin < 1) lowerEdgeBin = 1;
	if(upperEdgeBin > pLikelihood->GetNbinsX()) upperEdgeBin = pLikelihood->GetNbinsX();


	float lowerEdge = pLikelihood->GetBinCenter(lowerEdgeBin);
	float upperEdge = pLikelihood->GetBinCenter(upperEdgeBin);

	TF1* f = new TF1("fit", "pol2", lowerEdge, upperEdge);

	pLikelihood->Fit("fit","RQ");


	//fit is y = ax^2 + bx + cx
	float a = f->GetParameter(2);
	float b = f->GetParameter(1);
	float c = f->GetParameter(0);

	if(debug){
		cout << "\tMost Likely bin is: " <<  mostProbableBin << " - fitting in bins: [" << lowerEdgeBin << ", " << upperEdgeBin << "]" << endl;
		cout << "\tMomentum Fit Range: [ " << lowerEdge << ", " << upperEdge << "]" << endl;
		cout << "\tFit is: a: " << a << " b: " << b << " c: " << c << endl;
	}


	//check if curvature is correct -> d^2y/dx^2 > 0 -> a > 0
	if(a>0){
		//want dy/dx = 0 -> 0 = b + 2ax -> x = -b/2a
		pEst = -b/(2*a);

		//confidence intervals, 68.27%, 1 sigma
		const float dChi2 = 1;
		float intervalLow = 0;
		float intervalUpp = 0;


		/*
		 * want
		 *
		 * y0 + dChi2 = ax^2 + bx + c
		 *
		 * y0 = ax_min^2 + b_x_min + x
		 *
		 *
		 */

		float y0 = a*pEst*pEst +b*pEst + c;

		/* want to find solution to
		 *
		 * a*x^2 + b*x + c -(y0+dChi2) = 0
		 *
		 * -> d = c - (y0+dChi2)
		 */
		float d = c - (y0+dChi2);
		 /*
		 * quadratic equation
		 *
		 * x = -b +- sqrt(b^2 -4ad)/2a
		 *
		 */
		//want dChi2 =  bx + ax^2 -> quadratic equation
		// x = -b +- sqrt(b^2 -4a(c-1) / 2a
		float x1 =(-b + TMath::Sqrt(b*b -4.*a*d)) / (2.*a);
		float x2 =(-b - TMath::Sqrt(b*b -4.*a*d)) / (2.*a);
		if(x1 > x2){
			intervalUpp = x1;
			intervalLow = x2;
		} else {
			intervalUpp = x2;
			intervalLow = x1;
		}

		cout << "---- Estimate is: " << pEst << " within [ " << intervalLow << ", " << intervalUpp << "]" << endl;
		est.set(pEst,intervalLow, intervalUpp);
		return true;

	} else {
		cout << "-- Fit is: a: " << a << " b: " << b << " c: " << c << endl;
		cout << "Curvature wrong" << endl;
		//TODO: could get more sophisicated with what to do here
		return false;
	}
/*

	//want to find minimum, so

	if(debug){
		cout << "Most Likely bin is: " <<  mostProbableBin << " - fitting in bins: [" << lowerEdgeBin << ", " << upperEdgeBin << "]" << endl;
		cout << "Momentum Fit Range: [ " << lowerEdge << ", " << upperEdge << "]" << endl;
	}


	//find minimum of parabola

	//find where bounds are

	const unsigned int mostProbableBin = bestGuess->GetMaximumBin();
	float mostProbableP = bestGuess->GetBinCenter(mostProbableBin);
	float likelihoodAtMostProbableP = bestGuess->GetMaximum();

	//initialize to edges of histograms
	//float lowEdgeP = bestGuess->GetBinLowEdge(1);
	float lowEdgeP = bestGuess->GetBinCenter(1);
	//float highEdgeP = bestGuess->GetBinLowEdge(bestGuess->GetNbinsX()+1);
	float highEdgeP = bestGuess->GetBinCenter(bestGuess->GetNbinsX());

	//find lower bound of confidence interval
	for(unsigned int ibin =mostProbableBin; ibin > 0; ibin--){
		float likelihood = bestGuess->GetBinContent(ibin);
		float lnLikelihood = -2*TMath::Log(likelihood);
		float deltaLnLikelihood = lnLikelihood - minLnLikelihood;

		//lowEdgeP = bestGuess->GetBinLowEdge(ibin);
		lowEdgeP = bestGuess->GetBinCenter(ibin);
		//cout << "lowEdge: " << lowEdgeP << " deltaLnLikelihood = " << deltaLnLikelihood << endl;
		if(deltaLnLikelihood > dChi2[it]){
			break;
		}
	}


	//find upper bound of confidence interval
	for(unsigned int ibin =mostProbableBin; (int)ibin < bestGuess->GetNbinsX()+1; ibin++){
		float likelihood = bestGuess->GetBinContent(ibin);
		float lnLikelihood = -2*TMath::Log(likelihood);
		float deltaLnLikelihood = lnLikelihood -minLnLikelihood ;

		//highEdgeP = bestGuess->GetBinLowEdge(ibin+1);
		highEdgeP = bestGuess->GetBinCenter(ibin);
		//cout << "highEdge: " << highEdgeP << " deltaLnLikelihood = " << deltaLnLikelihood << endl;
		if(deltaLnLikelihood > dChi2[it]){
			break;
		}
	}

	if(i < WRITE_LIMIT)cout << " Brem " << 100*confLevels[it] << "% P Limits: [ " << lowEdgeP<< ", " << highEdgeP << " ]" << endl;
	if(it == 0){
		float sigmaEst = (highEdgeP-lowEdgeP)/2.;
		pullDistribution->Fill((genP-mostProbableP)/sigmaEst);
		sigmaDistribution->Fill(sigmaEst);
		genPVsSigma->Fill(sigmaEst, genP);
	}
	genMinusBrem->Fill(genP-mostProbableP);
	genMinusReco->Fill(genP-recoP);
}


	return PointEstimate(0,0,0);
	*/
}


/* Calculates clopper pearson interval and
 * does point estimation
 */
const PointEstimate ClopperPearson(float nOn, float nTotal, float alpha){
	if(nOn < 0 || nTotal <= 0) return PointEstimate();

	float lower = 0;
	float upper = 1;

	if(nOn > 0){
		lower = ROOT::Math::beta_quantile(alpha, nOn, nTotal - nOn  +1);
	}
	if(nTotal - nOn > 0){
		upper = ROOT::Math::beta_quantile_c(alpha, nOn+1, nTotal-nOn);
	}
	return PointEstimate((float)nOn/nTotal, lower, upper);
}
