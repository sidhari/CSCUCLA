/*
 * CLCTLayerAnalyzer.h
 *
 *  Created on: Jun 18, 2019
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_CLCTLAYERANALYZER_H_
#define CSCPATTERNS_INCLUDE_CLCTLAYERANALYZER_H_


#include "../include/Processor.h"

#include <TH1F.h>
#include <map>

class CLCTLayerAnalyzer : public  Processor {
	int run(std::string inputfile, std::string outputfile, int start=0, int end=-1);
public:
	static void fillHist(map<unsigned int, TH1F*> hists, unsigned int key, float histValue);
	static map<unsigned int, TH1F*> makeHistPermutation(
			string name, string title, unsigned int bins, unsigned int low, unsigned int high);
};


#endif /* CSCPATTERNS_INCLUDE_CLCTLAYERANALYZER_H_ */
