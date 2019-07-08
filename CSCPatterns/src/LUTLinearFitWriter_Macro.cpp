/*
 * comparatorCodeEmulator.cpp
 *
 *  Created on: Apr 27, 2018
 *      Author: williamnash
 */

#include "../include/CSCHelperFunctions.h"
#include <TGraph.h>
#include <TF1.h>
#include <TSystem.h>
#include <TH1F.h>

//c++
#include <fstream>
#include <math.h>

using namespace std;

float HS_ERROR = 0.288675; //uncertainty per layer in half strips 1/sqrt(12)


/* get fit parameter errors for y = mx + b fit
 * see page 188 in Taylor - Error Analysis for derivation
 */
int getErrors(const vector<int>& x,const vector<int>& y, float& sigmaM, float& sigmaB) {

	int N = x.size();
	if(!N) return -1;

	float sumx = 0;
	float sumx2 = 0;
	for(int i =0; i < N; i++){
		sumx += x[i];
		sumx2 += x[i]*x[i];
	}

	float delta = N*sumx2 - sumx*sumx;

	sigmaM = HS_ERROR * sqrt(1.*N/delta);
	sigmaB = HS_ERROR * sqrt(sumx2/delta);

	return 0;
}


int LUTLinearFitWriter_Macro(){
	gSystem->Load("../lib/PatternFinderClasses_cpp");
	gSystem->Load("../lib/PatternFinderHelperFunctions_cpp");


	//all the patterns we will fit
	vector<CSCPattern>* newPatterns = createNewPatterns();



	//output file stream to write the fits
	const string outName = "../dat/linearFits.lut";

	ofstream output;
	output.open(outName.c_str());
	if(!output.is_open()){
		cout << "can't open file:" << endl;
	}
	//output << "Pattern\tCC\tOffset\tSlope\tChi2\tNDF\tSlopeErr\tOffErr\n";


	// for each pattern
	for(auto patt = newPatterns->begin(); patt != newPatterns->end(); ++patt){
		// iterate through each possible comparator code
		for(int code = 0; code < 4096; code++){


			if(DEBUG >0) {
				cout << "Evaluating Pattern: " << patt->getName() << " CC: " << code << endl;
			}
			int hits [MAX_PATTERN_WIDTH][NLAYERS];

			if(patt->recoverPatternCCCombination(code, hits)){
				cout << "Error: CC evaluation has failed" << endl;
			}

			//put the coordinates in the hits into a vector
			// x = layer, y = position in that layer
			vector<int> x;
			vector<int> y;

			for(unsigned int i =0; i < NLAYERS; i++){
				for(unsigned int j =0; j < MAX_PATTERN_WIDTH; j++){
					if(DEBUG > 0) cout << hits[j][i];
					if(hits[j][i]){
						x.push_back(i-2); //shift to key half strip layer (layer 3)
						y.push_back(j-(MAX_PATTERN_WIDTH-1)/2.);
					}
				}
				if(DEBUG > 0) cout << endl;
			}

			//skip if we only have less than two hits (can't fit a line)
			if(x.size() <  2) continue;

			// for each combination fit a straight line
			TGraph* gr = new TGraph(x.size(), &x[0], &y[0]);

			TF1* fit = new TF1("fit", "[0]+[1]*x", -3, 4);
			gr->Fit("fit");

			//
			// Some funky sign issues, slope is opposite the expected sign,
			// and offset is off by 0.5 strips, and need to convert to strips
			//

			//double offset = 0.5*fit->GetParameter(0) + 0.25; //offset between the two
			//double slope = -0.5*fit->GetParameter(1);
			double offset = 0.5*fit->GetParameter(0) - 0.75; //offset between the two
			double slope = -0.5*fit->GetParameter(1);
			double chi2 = fit->GetChisquare();
			double ndf = fit->GetNDF();



			float sigmaM, sigmaB;

			getErrors(x,y,sigmaM,sigmaB);

			sigmaM *=0.5; //hs to strips
			sigmaB *=0.5;

			if(DEBUG > 0) cout << "Offset: " << offset<<
					" Slope: " << slope << endl;


            // formatted as: pattern (cc) - position slope nsegments (quality layers chi2)

			int layers = ndf+2;
			int nsegments = 0; //set default
			float quality = -1.; //set default

			output << patt->getName() << " " << code << " ~ " <<
					offset << " " << slope << " " <<
					nsegments << " " << quality << " " <<
					layers << " " << chi2 << "\n";
					//sigmaB << "\t" << sigmaM <<"\n";

			delete gr;
		}
	}

	cout << "Wrote to: " << outName<< endl;
	output.close();
	return 0;
}

