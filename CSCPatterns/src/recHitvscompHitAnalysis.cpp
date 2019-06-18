//============================================================================
// Name        : recHitvscompHitAnalysis.cpp
// Author      : William Nash
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


//c++
#include <CSCConstants.h>
#include <vector>

//root
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>

//project

#ifdef __MAKECINT__
#pragma link C++ class vector<vector<int> >+;
#endif

using namespace std;

//takes the kSerial id, and fills an array stationRingEndcap which has size two,
//the first value is the station, the second value is the ring
int decodeID(int id , int (&stationRingEndcap)[3])
{
	id -= 1; //shift to start at zero
	if(id >= 300) {
		stationRingEndcap[2] = 2; //second endcap
		id -=300; //use results from both sides
	} else {
		stationRingEndcap[2] = 1; //first endcap
	}
	if(id < 36) //station = 1, ring = 1 -> ME11
	{
		stationRingEndcap[0] = 1;
		stationRingEndcap[1] = 1;
	}
	else if(id < 72) // station 1, ring 2 -> ME12
	{
		stationRingEndcap[0] = 1;
		stationRingEndcap[1] = 2;
	}
	else if(id <108) // station 1 ring 3 -> ME13
	{
		stationRingEndcap[0] = 1;
		stationRingEndcap[1] = 3;
	}
	else if(id < 126) // station 2 ring 1-> ME21
	{
		stationRingEndcap[0] = 2;
		stationRingEndcap[1] = 1;
	}
	else if(id < 162) // station 2 ring 2 -> ME22
	{
		stationRingEndcap[0] = 2;
		stationRingEndcap[1] = 2;
	}
	else if(id < 180) // station 3 ring 1 -> ME31
	{
		stationRingEndcap[0] = 3;
		stationRingEndcap[1] = 1;
	}
	else if(id < 216) // station 3 ring 2 -> ME32
	{
		stationRingEndcap[0] = 3;
		stationRingEndcap[1] = 2;
	}
	else if(id < 234) //station 4 ring 1 -> ME41
	{
		stationRingEndcap[0] = 4;
		stationRingEndcap[1] = 1;
	}
	else //station 4 ring 2 -> ME42
	{
		stationRingEndcap[0] = 4;
		stationRingEndcap[1] = 2;
	}
	return 0;
}

int recHitvscompHitAnalysis(){

	//
	// DECLARE ALL NON-HISTOGRAM VARIABLES
	//

	//comparators
	vector<vector<int>>* compStr = 0; //comparator strip #
	vector<vector<int>>* compHS = 0; //comparator half strip #
	vector<int>* compLay = 0; // y axis
	vector<int>* compId = 0; // index of what ring/station you are on
	bool os = false;

	//reconstructed offline hits
	vector<int>* rhId = 0; //id/serial
	vector<int>* rhLay = 0;
	vector<float>* rhPos = 0;
	vector<float>* rhE = 0; //energy

	TFile* f = TFile::Open((string("/Users/williamnash/CSCPatterns/data/fix-me")).c_str());
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

	//
	// SET BRANCH ADDRESSES
	//
	t->SetBranchAddress("compId",&compId);
	t->SetBranchAddress("compLay",&compLay);
	t->SetBranchAddress("compStr",&compStr);
	t->SetBranchAddress("compHS",&compHS);
	t->SetBranchAddress("os", &os);
	t->SetBranchAddress("rhId",&rhId);
	t->SetBranchAddress("rhLay", &rhLay);
	t->SetBranchAddress("rhPos", &rhPos);
	t->SetBranchAddress("rhE", &rhE);
	

	//
	// DECLARE HISTOGRAMS
	//

	TH2F* rhPositionDiff2D = new TH2F("h", "Position Difference of All Rec Hits",200, -1, 1, 100, 0, 1);
	rhPositionDiff2D->GetXaxis()->SetTitle("Comparator minus recHit Position [strips]");
	rhPositionDiff2D->GetXaxis()->CenterTitle();
	rhPositionDiff2D->GetYaxis()->SetTitle("Modulo Strip [strips]");
	rhPositionDiff2D->GetYaxis()->CenterTitle();


	for(Long64_t i = 0; i < t->GetEntriesFast(); i++){
		if(!(i%10000)) printf("%3.2f%% Done --- Processed %lli Events\n", 100.*i/t->GetEntries(), i);
		t->GetEntry(i);


		//only look at os muons
		if(!os) continue;


		//iterate through all the reconstructed hits for this muon
		for(unsigned int j =0; j < rhId->size(); j++)
		{
			//keep track of their ID and Layer
			int thisRhId = rhId->at(j);
			int thisRhLay = rhLay->at(j);
			//float thisRhE = rhE->at(j);
			float thisRhPos = rhPos->at(j);

			if (thisRhPos < 1 || thisRhPos > 79) continue;

			bool me11b = false;
			//bool me11a = false;

			bool foundMatchingCompHit = false;
			float minDiff = 10e9; //arbirartily large, larger than any value the detectors can possibly have

			//for each one of these, iterate through all the comparator hits as well
			for(unsigned int k =0; k < compId->size(); k++)
			{

				int thisCompId = compId->at(k);
				int thisCompLay = compLay->at(k);
				int stationRingId4[3] ={0,0,0};
				decodeID(thisCompId,  stationRingId4);
				if(stationRingId4[0] == 1 && stationRingId4[1] == 1 && compStr->at(k).size()){
					if(me11b && compStr->at(k).front() > 64)  continue;
					if(!me11b && compStr->at(k).front() <= 64)  continue;
				}

				if(thisCompId == thisRhId && thisRhLay == thisCompLay) {
					if(compStr->at(k).size() == 0) continue;
					for(unsigned int l=0; l < compStr->at(k).size(); l++) {
						int strip = compStr->at(k).at(l);
						if(stationRingId4[0] ==1 && stationRingId4[1] == 1 && !me11b) strip -= 64;
						int halfStrip = compHS->at(k).at(l);

						//
						float stripVal = strip  + 0.5*(1.*halfStrip-0.5);
						//

						float thisDiff =  stripVal - thisRhPos;
						if(abs(minDiff) > abs(thisDiff)) minDiff = thisDiff;
					}

					foundMatchingCompHit = true;
					break;

				}
			}
			if(foundMatchingCompHit) {
				float moduloStrip = thisRhPos - floor(thisRhPos);
				rhPositionDiff2D->Fill(minDiff, moduloStrip);


			}

		}
	}

	string folder = (USE_COMP_HITS ? "compHits" : "recHits");
	TFile * outF = new TFile(string("../data/" + folder + "/recVsComp-ANAL.root").c_str(),"RECREATE");

	rhPositionDiff2D->Draw("colz");
	rhPositionDiff2D->Write();

	outF->Close();

	return 0;
}
