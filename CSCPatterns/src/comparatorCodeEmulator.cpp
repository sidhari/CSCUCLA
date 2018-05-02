/*
 * comparatorCodeEmulator.cpp
 *
 *  Created on: Apr 27, 2018
 *      Author: williamnash
 */

#include "../include/PatternFinderHelperFunctions.h"

//root
#include <TGraph.h>
#include <TF1.h>

//c++
#include <fstream>

using namespace std;

int comparatorCodeEmulator(){

	//all the patterns we will fit
	vector<ChargePattern>* newPatterns = createNewEnvelopes();

	//output file stream to write the fits
	ofstream output;
	output.open("../data/linearFits.txt");
	output << "Pattern\tCC\tOffset\tSlope\tChi2\tNDF\n";


	// for each pattern
	for(auto patt = newPatterns->begin(); patt != newPatterns->end(); ++patt){
		// iterate through each possible comparator code
		for(int code = 0; code < 4096; code++){


			if(DEBUG >0) {
				cout << "Evaluating Pattern: " << patt->name() << " CC: " << code << endl;
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
			double offset = fit->GetParameter(0);
			double slope = fit->GetParameter(1);
			double chi2 = fit->GetChisquare();
			double ndf = fit->GetNDF();

			if(DEBUG > 0) cout << "Offset: " << offset<<
					" Slope: " << slope << endl;

			// record the offset (centered) and slope of the line
			output << patt->name() << "\t" << code << "\t" <<
					offset<< "\t" << slope << "\t" <<
					chi2 << "\t" << ndf << "\n";

			delete gr;
		}
	}

	output.close();
	return 0;
}

