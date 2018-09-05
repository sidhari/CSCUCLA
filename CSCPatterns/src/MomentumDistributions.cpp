/*
 * MomentumDistributions.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: williamnash
 */

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>

#include <vector>


#include "../include/PatternConstants.h"

using namespace std;

int MomentumDistributions(){
	TFile* f = TFile::Open("../data/you-need-to-fix-this");

	if(!f)
	{
		printf("Can't open file\n");
		return -1;
	}

	TTree* t =  (TTree*)f->Get("CSCDigiTree");
	if(!t){
		printf("Can't find tree\n");
		return -1;
	}


    double Pt = 0;
    double eta = 0;
    bool os = 0;

    //segments
    vector<int>     *segEc = 0;
    vector<int>     *segSt = 0;
    vector<int>     *segRi = 0;
    vector<int>     *segCh = 0;

    t->SetBranchAddress("Pt", &Pt);
    t->SetBranchAddress("eta", &eta);
    t->SetBranchAddress("segEc", &segEc);
    t->SetBranchAddress("segSt",&segSt);
    t->SetBranchAddress("segRi", &segRi);
    t->SetBranchAddress("segCh", &segCh);
    t->SetBranchAddress("os", &os);

    TH1F* momDis = new TH1F("pt","Selected Momentum Distribution from Charmonium 2016F Data", 70, 0, 140);
    momDis->GetXaxis()->SetTitle("p_{T} [GeV]");
    momDis->GetXaxis()->CenterTitle();
    momDis->GetYaxis()->SetTitle("Entries / 2 GeV");
    momDis->GetYaxis()->CenterTitle();




    for(unsigned int i = 0; i < t->GetEntriesFast(); i++) {
            if(!(i%10000)) printf("%3.2f%% Done --- Processed %u Events\n", 100.*i/t->GetEntries(), i);
            t->GetEntry(i);

            if(!os) continue;
            momDis->Fill(Pt);


            //for(unsigned int thisSeg = 0; thisSeg < segCh->size(); thisSeg++){

                      // int EC = (*segEc)[thisSeg];
                       //int ST = (*segSt)[thisSeg];
                       //int RI = (*segRi)[thisSeg];
                       //int CH = (*segCh)[thisSeg];

                       /* Chamber ID can be deciphered from this
                        *
                        *  if((ST == 1 && RI == 4 )||(ST == 1 && RI == 2)) groupIndex = 0; //ME11a || ME12
                        *	else if((ST == 1 && RI == 1) || (ST == 1 && RI == 3)) groupIndex = 1; //ME11b || ME13
                        *	else if((ST == 2 && RI == 1) || (ST == 2 && RI == 2)) groupIndex = 2; //ME21 || ME22
                        *	else if(ST == 3 || ST == 4) groupIndex = 3; //ME3X || ME4X
                        */

                       //here, just take one of the chambers and look at its momentum distribution
                       //if(ST == 2 && RI == 1) { //ME21
                    	  // momDis->Fill(Pt);
                       //}
            //}


    }
    momDis->Draw();


    return 0;
}
