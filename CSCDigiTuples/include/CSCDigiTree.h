//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Apr 20 16:30:14 2016 by ROOT version 6.02/05
// from TTree CSCDigiTree/Tree holding timing information
// found on file: CSCDigiTree.root
//////////////////////////////////////////////////////////

#ifndef CSCDigiTree_h
#define CSCDigiTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <TH2.h>
#include <TLine.h>

#include <iostream>
#include <string>

// Header file for the classes stored in the TTree if any.
#include "vector"


using namespace std;

class CSCDigiTree {
    public :
        TTree          *fChain;   //!pointer to the analyzed TTree or TChain
        Int_t           fCurrent; //!current Tree number in a TChain

        // Fixed size dimensions of array or collections stored in the TTree if any.

        // Declaration of leaf types
        Int_t           Event_EventNumber;
        Int_t           Event_RunNumber;
        Int_t           Event_LumiSection;
        Int_t           Event_BXCrossing;
        bool            os;
        bool            ss;
        Double_t        Pt;
        Double_t        eta;
        Double_t        phi;
        int             q;
        Int_t           Nseg;
        vector<int>     *segEc;
        vector<int>     *segSt;
        vector<int>     *segRi;
        vector<int>     *segCh;
        vector<float>     *segX;
        vector<float>     *segY;
        vector<float>     *segdXdZ;
        vector<float>     *segdYdZ;
        vector<int>     *rhId;
        vector<int>     *rhLay;
        vector<float>     *rhPos;
        vector<float>     *rhE;
        vector<float>     *rhMax;
        vector<int>     *lctId;
        vector<vector<int> > *lctQ;
        vector<vector<int> > *lctPat;
        vector<vector<int> > *lctKWG;
        vector<vector<int> > *lctKHS;
        vector<vector<int> > *lctBend;
        vector<int>     *tflctId;
        vector<vector<int> > *tflctQ;
        vector<vector<int> > *tflctPat;
        vector<vector<int> > *tflctKWG;
        vector<vector<int> > *tflctKHS;
        vector<vector<int> > *tflctBend;
        vector<int>     *clctId;
        vector<vector<int> > *clctQ;
        vector<vector<int> > *clctPat;
        vector<vector<int> > *clctKHS;
        vector<vector<int> > *clctCFEB;
        vector<vector<int> > *clctBend;
        vector<int>     *alctId;
        vector<vector<int> > *alctQ;
        vector<vector<int> > *alctAc;
        vector<vector<int> > *alctPB;
        vector<int>     *compId;
        vector<int>     *compLay;
        vector<vector<int> > *compStr;
        vector<vector<int> > *compHS;
        vector<vector<vector<int> > > *compTimeOn;
        vector<int>     *wireId;
        vector<int>     *wireLay;
        vector<vector<int> > *wireGrp;
        vector<vector<vector<int> > > *wireTimeOn;
        vector<int>     *stripId;
        vector<int>     *stripLay;
        vector<vector<int> > *strip;
        vector<vector<vector<int> > > *stripADCs;

        // List of branches
        TBranch        *b_Event_EventNumber;   //!
        TBranch        *b_Event_RunNumber;   //!
        TBranch        *b_Event_LumiSection;   //!
        TBranch        *b_Event_BXCrossing;   //!
        TBranch        *b_os;   //!
        TBranch        *b_ss;   //!
        TBranch        *b_Pt;   //!
        TBranch        *b_eta;   //!
        TBranch        *b_phi;   //!
        TBranch        *b_q;   //!
        TBranch        *b_Nseg;   //!
        TBranch        *b_segEc;   //!
        TBranch        *b_segSt;   //!
        TBranch        *b_segRi;   //!
        TBranch        *b_segCh;   //!
        TBranch        *b_segX;   //!
        TBranch        *b_segY;   //!
        TBranch        *b_segdXdZ;   //!
        TBranch        *b_segdYdZ;   //!
        TBranch        *b_rhId;   //!
        TBranch        *b_rhLay;   //!
        TBranch        *b_rhPos;   //!
        TBranch        *b_rhE;   //!
        TBranch        *b_rhMax;   //!
        TBranch        *b_lctId;   //!
        TBranch        *b_lctQ;   //!
        TBranch        *b_lctPat;   //!
        TBranch        *b_lctKWG;   //!
        TBranch        *b_lctKHS;   //!
        TBranch        *b_lctBend;   //!
        TBranch        *b_tflctId;   //!
        TBranch        *b_tflctQ;   //!
        TBranch        *b_tflctPat;   //!
        TBranch        *b_tflctKWG;   //!
        TBranch        *b_tflctKHS;   //!
        TBranch        *b_tflctBend;   //!
        TBranch        *b_clctId;   //!
        TBranch        *b_clctQ;   //!
        TBranch        *b_clctPat;   //!
        TBranch        *b_clctKHS;   //!
        TBranch        *b_clctCFEB;   //!
        TBranch        *b_clctBend;   //!
        TBranch        *b_alctId;   //!
        TBranch        *b_alctQ;   //!
        TBranch        *b_alctAc;   //!
        TBranch        *b_alctPB;   //!
        TBranch        *b_compId;   //!
        TBranch        *b_compLay;   //!
        TBranch        *b_compStr;   //!
        TBranch        *b_compHS;   //!
        TBranch        *b_compTimeOn;   //!
        TBranch        *b_wireId;   //!
        TBranch        *b_wireLay;   //!
        TBranch        *b_wireGrp;   //!
        TBranch        *b_wireTimeOn;   //!
        TBranch        *b_stripId;   //!
        TBranch        *b_stripLay;   //!
        TBranch        *b_strip;   //!
        TBranch        *b_stripADCs;   //!

        CSCDigiTree(TTree *tree=0);
        virtual ~CSCDigiTree();
        virtual Int_t    Cut(Long64_t entry);
        virtual Int_t    GetEntry(Long64_t entry);
        virtual Long64_t LoadTree(Long64_t entry);
        virtual void     Init(TTree *tree);
        virtual void     Loop(string sName);
        //virtual void     DrawBoxes(float x_pos, float y_pos);
        //virtual void     DrawPats(int pid);
        virtual Bool_t   Notify();
        virtual int      chamberSerial(int ec, int st, int ri, int ch );
        virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CSCDigiTree_cxx
CSCDigiTree::CSCDigiTree(TTree *tree) : fChain(0) 
{
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    if (tree == 0) {
        TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("CSCDigiTree.root");
        if (!f || !f->IsOpen()) {
            f = new TFile("CSCDigiTree.root");
        }
        f->GetObject("CSCDigiTree",tree);

    }
    Init(tree);
}

CSCDigiTree::~CSCDigiTree()
{
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t CSCDigiTree::GetEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
Long64_t CSCDigiTree::LoadTree(Long64_t entry)
{
    // Set the environment to read one entry
    if (!fChain) return -5;
    Long64_t centry = fChain->LoadTree(entry);
    if (centry < 0) return centry;
    if (fChain->GetTreeNumber() != fCurrent) {
        fCurrent = fChain->GetTreeNumber();
        Notify();
    }
    return centry;
}

void CSCDigiTree::Init(TTree *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    // Set object pointer
    segEc = 0;
    segSt = 0;
    segRi = 0;
    segCh = 0;
    segX = 0;
    segY = 0;
    segdXdZ = 0;
    segdYdZ = 0;
    rhId = 0;
    rhLay = 0;
    rhPos = 0;
    rhE = 0;
    rhMax = 0;
    lctId = 0;
    lctQ = 0;
    lctPat = 0;
    lctKWG = 0;
    lctKHS = 0;
    lctBend = 0;
    tflctId = 0;
    tflctQ = 0;
    tflctPat = 0;
    tflctKWG = 0;
    tflctKHS = 0;
    tflctBend = 0;
    clctId = 0;
    clctQ = 0;
    clctPat = 0;
    clctKHS = 0;
    clctCFEB = 0;
    clctBend = 0;
    alctId = 0;
    alctQ = 0;
    alctAc = 0;
    alctPB = 0;
    compId = 0;
    compLay = 0;
    compStr = 0;
    compHS = 0;
    compTimeOn = 0;
    wireId = 0;
    wireLay = 0;
    wireGrp = 0;
    wireTimeOn = 0;
    stripId = 0;
    stripLay = 0;
    strip = 0;
    stripADCs = 0;
    // Set branch addresses and branch pointers
    if (!tree) return;
    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);

    fChain->SetBranchAddress("Event_EventNumber", &Event_EventNumber, &b_Event_EventNumber);
    fChain->SetBranchAddress("Event_RunNumber", &Event_RunNumber, &b_Event_RunNumber);
    fChain->SetBranchAddress("Event_LumiSection", &Event_LumiSection, &b_Event_LumiSection);
    fChain->SetBranchAddress("Event_BXCrossing", &Event_BXCrossing, &b_Event_BXCrossing);
    fChain->SetBranchAddress("os", &os, &b_os);
    fChain->SetBranchAddress("ss", &ss, &b_ss);
    fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
    fChain->SetBranchAddress("eta", &eta, &b_eta);
    fChain->SetBranchAddress("phi", &phi, &b_phi);
    fChain->SetBranchAddress("q", &q, &b_q);
    fChain->SetBranchAddress("Nseg", &Nseg, &b_Nseg);
    fChain->SetBranchAddress("segEc", &segEc, &b_segEc);
    fChain->SetBranchAddress("segSt", &segSt, &b_segSt);
    fChain->SetBranchAddress("segRi", &segRi, &b_segRi);
    fChain->SetBranchAddress("segCh", &segCh, &b_segCh);
    fChain->SetBranchAddress("segX", &segX, &b_segX);
    fChain->SetBranchAddress("segY", &segY, &b_segY);
    fChain->SetBranchAddress("segdXdZ", &segdXdZ, &b_segdXdZ);
    fChain->SetBranchAddress("segdYdZ", &segdYdZ, &b_segdYdZ);
    fChain->SetBranchAddress("rhId", &rhId, &b_rhId);
    fChain->SetBranchAddress("rhLay", &rhLay, &b_rhLay);
    fChain->SetBranchAddress("rhPos", &rhPos, &b_rhPos);
    fChain->SetBranchAddress("rhE", &rhE, &b_rhE);
    fChain->SetBranchAddress("rhMax", &rhMax, &b_rhMax);
    fChain->SetBranchAddress("lctId", &lctId, &b_lctId);
    fChain->SetBranchAddress("lctQ", &lctQ, &b_lctQ);
    fChain->SetBranchAddress("lctPat", &lctPat, &b_lctPat);
    fChain->SetBranchAddress("lctKWG", &lctKWG, &b_lctKWG);
    fChain->SetBranchAddress("lctKHS", &lctKHS, &b_lctKHS);
    fChain->SetBranchAddress("lctBend", &lctBend, &b_lctBend);
    fChain->SetBranchAddress("tflctId", &tflctId, &b_tflctId);
    fChain->SetBranchAddress("tflctQ", &tflctQ, &b_tflctQ);
    fChain->SetBranchAddress("tflctPat", &tflctPat, &b_tflctPat);
    fChain->SetBranchAddress("tflctKWG", &tflctKWG, &b_tflctKWG);
    fChain->SetBranchAddress("tflctKHS", &tflctKHS, &b_tflctKHS);
    fChain->SetBranchAddress("tflctBend", &tflctBend, &b_tflctBend);
    fChain->SetBranchAddress("clctId", &clctId, &b_clctId);
    fChain->SetBranchAddress("clctQ", &clctQ, &b_clctQ);
    fChain->SetBranchAddress("clctPat", &clctPat, &b_clctPat);
    fChain->SetBranchAddress("clctKHS", &clctKHS, &b_clctKHS);
    fChain->SetBranchAddress("clctCFEB", &clctCFEB, &b_clctCFEB);
    fChain->SetBranchAddress("clctBend", &clctBend, &b_clctBend);
    fChain->SetBranchAddress("alctId", &alctId, &b_alctId);
    fChain->SetBranchAddress("alctQ", &alctQ, &b_alctQ);
    fChain->SetBranchAddress("alctAc", &alctAc, &b_alctAc);
    fChain->SetBranchAddress("alctPB", &alctPB, &b_alctPB);
    fChain->SetBranchAddress("compId", &compId, &b_compId);
    fChain->SetBranchAddress("compLay", &compLay, &b_compLay);
    fChain->SetBranchAddress("compStr", &compStr, &b_compStr);
    fChain->SetBranchAddress("compHS", &compHS, &b_compHS);
    fChain->SetBranchAddress("compTimeOn", &compTimeOn, &b_compTimeOn);
    fChain->SetBranchAddress("wireId", &wireId, &b_wireId);
    fChain->SetBranchAddress("wireLay", &wireLay, &b_wireLay);
    fChain->SetBranchAddress("wireGrp", &wireGrp, &b_wireGrp);
    fChain->SetBranchAddress("wireTimeOn", &wireTimeOn, &b_wireTimeOn);
    fChain->SetBranchAddress("stripId", &stripId, &b_stripId);
    fChain->SetBranchAddress("stripLay", &stripLay, &b_stripLay);
    fChain->SetBranchAddress("strip", &strip, &b_strip);
    fChain->SetBranchAddress("stripADCs", &stripADCs, &b_stripADCs);
    Notify();
}

Bool_t CSCDigiTree::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

int CSCDigiTree::chamberSerial( int ec, int st, int ri, int ch ) {

    int kSerial = ch;
    if (st == 1 && ri == 1) kSerial = ch;
    if (st == 1 && ri == 2) kSerial = ch + 36;
    if (st == 1 && ri == 3) kSerial = ch + 72;
    if (st == 1 && ri == 4) kSerial = ch;
    if (st == 2 && ri == 1) kSerial = ch + 108;
    if (st == 2 && ri == 2) kSerial = ch + 126;
    if (st == 3 && ri == 1) kSerial = ch + 162;
    if (st == 3 && ri == 2) kSerial = ch + 180;
    if (st == 4 && ri == 1) kSerial = ch + 216;
    if (st == 4 && ri == 2) kSerial = ch + 234;  // one day...
    if (ec == 2) kSerial = kSerial + 300;
    //cout << endl << endl << "Endcap: " << ec << "  Station: " << st << "  Ring: " << ri << "  Chamber: " << ch << "  serialID: " << kSerial << endl;
    return kSerial;
}


void CSCDigiTree::Show(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
    LoadTree(entry);

    TCanvas *cStr = new TCanvas("cStr", "", 0, 0, 1200, Nseg*273);
    cStr->Divide(1,Nseg);
    cStr->SetRightMargin(0.15);
    //cStr->SetBottomMargin(0.20);
    cStr->SetTopMargin(0.15);

    TCanvas *cComp = new TCanvas("cComp", "", 0, 0, 1200, Nseg*280);
    cComp->Divide(1,Nseg);
    cComp->SetRightMargin(0.15);
    cComp->SetBottomMargin(0.20);
    cComp->SetTopMargin(0.15);

    TCanvas *cWire = new TCanvas("cWire", "", 0, 0, 1200, Nseg*280);
    cWire->Divide(1,Nseg);
    cWire->SetRightMargin(0.15);
    cWire->SetBottomMargin(0.20);
    cWire->SetTopMargin(0.15);

    TH2F stripDis[10];
    TH2F compDis[10];
    TH2F wireDis[10];

    cout << "Pt: " << Pt << endl;

    cout << endl;
    cout << "This muon has " << Nseg << " segments." << endl;
    for(int iseg = 0; iseg < Nseg; iseg++)//Loop over segments
    {
        int EC = segEc->at(iseg);
        int ST = segSt->at(iseg);
        int RI = segRi->at(iseg);
        int CH = segCh->at(iseg);

        int chSid = chamberSerial(EC, ST, RI, CH);

        if(EC==1) 
        {
            stripDis[iseg] = TH2F(Form("stripDis%i",iseg), Form("Cathode Strip ADC Counts ME+%i/%i/%i",ST,RI,CH), 82, 0.5, 82.5, 6, 0.5, 6.5);
            compDis[iseg] = TH2F(Form("compDis%i",iseg), Form("Comparator Timing ME+%i/%i/%i",ST,RI,CH), 182, 0.5, 182.5, 6, 0.5, 6.5);
            wireDis[iseg] = TH2F(Form("wireDis%i",iseg), Form("Anode Wire Timing ME+%i/%i/%i",ST,RI,CH), 117, 0.5, 117.5, 6, 0.5, 6.5);
        }
        else
        {
            stripDis[iseg] = TH2F(Form("stripDis%i",iseg), Form("Cathode Strip ADC Counts ME-%i/%i/%i",ST,RI,CH), 82, 0.5, 82.5, 6, 0.5, 6.5);
            compDis[iseg] = TH2F(Form("compDis%i",iseg), Form("Comparator Timing ME-%i/%i/%i",ST,RI,CH), 182, 0.5, 182.5, 6, 0.5, 6.5);
            wireDis[iseg] = TH2F(Form("wireDis%i",iseg), Form("Anode Wire Timing ME-%i/%i/%i",ST,RI,CH), 117, 0.5, 117.5, 6, 0.5, 6.5);
        }

        stripDis[iseg].SetStats(0);
        stripDis[iseg].GetXaxis()->SetTitle("Strip");
        stripDis[iseg].GetYaxis()->SetTitle("Layer");
        stripDis[iseg].GetZaxis()->SetRangeUser(0,1024);

        compDis[iseg].SetStats(0);
        compDis[iseg].GetXaxis()->SetTitle("Half Strip");
        compDis[iseg].GetYaxis()->SetTitle("Layer");
        compDis[iseg].GetZaxis()->SetRangeUser(0,16);

        wireDis[iseg].SetStats(0);
        wireDis[iseg].GetXaxis()->SetTitle("Wire Group");
        wireDis[iseg].GetYaxis()->SetTitle("Layer");
        wireDis[iseg].GetZaxis()->SetRangeUser(0,20);


        //Read the ALCTs for this Segment
        cout << "ALCTs:" << endl;
        bool malct = false;
        for(int ialct = 0; ialct < (int) alctId->size(); ialct++)//Loop over alcts
        {
            if(chSid == alctId->at(ialct))
            {
                for(int i = 0; i < (int) alctQ->at(ialct).size(); i++)
                {
                    cout << "ALCT Found  Q = " << alctQ->at(ialct).at(i) << " Ac = " << alctAc->at(ialct).at(i) << " PatB = " << alctPB->at(ialct).at(i) << endl;
                }
                malct = true;
            }
        }
        if(!malct) cout << "NO ALCT Found for this Segment!" << endl;
        cout << endl;

        //Read the CLCTs for this Segment
        cout << "CLCTs:" << endl;
        bool mclct = false;
        for(int iclct = 0; iclct < (int) clctId->size(); iclct++)//Loop over clcts
        {
            if(chSid == clctId->at(iclct))
            {
                for(int i = 0; i < (int) clctQ->at(iclct).size(); i++)
                {
                    cout << "CLCT Found  Q = " << clctQ->at(iclct).at(i) << " Pat = " << clctPat->at(iclct).at(i) << " KHS = " << 32*clctCFEB->at(iclct).at(i)+clctKHS->at(iclct).at(i);
                    cout << " Bend = " << clctBend->at(iclct).at(i) << endl;
                }
                mclct = true;
            }
        }
        if(!mclct) cout << "NO CLCT Found for this Segment!" << endl;
        cout << endl;

        //Read the LCTs for this Segment
        cout << "LCTs:" << endl;
        bool mlct = false;
        for(int ilct = 0; ilct < (int) lctId->size(); ilct++)//Loop over lcts
        {
            if(chSid == lctId->at(ilct))
            {
                for(int i = 0; i < (int) lctQ->at(ilct).size(); i++)
                {
                    cout << "LCT Found  Q = " << lctQ->at(ilct).at(i) << " Pat = " << lctPat->at(ilct).at(i) << " KHS = " << lctKHS->at(ilct).at(i);
                    cout << " KWG = " << lctKWG->at(ilct).at(i) << " Bend = " << lctBend->at(ilct).at(i) << endl;
                }
                mlct = true;
            }
        }
        if(!mlct) cout << "NO LCT Found for this Segment!" << endl;
        cout << endl;

        //Read the Comps for this Segment
        cout << "Comparators:" << endl;
        bool mcomp = false;
        for(int icomp = 0; icomp < (int) compId->size(); icomp++)//Loop over comps
        {
            if(chSid == compId->at(icomp))
            {
                cout << "Comparator Found!" << endl;
                for(int i = 0; i < (int) compStr->at(icomp).size(); i++)
                {
                    int layN = compLay->at(icomp);
                    if(layN%2==1) cout << "Half Strip: " << 2*(compStr->at(icomp).at(i)-1) + compHS->at(icomp).at(i) << "  Layer: " << compLay->at(icomp) << endl;
                    else cout << "Half Strip: " << 2*(compStr->at(icomp).at(i)-1) + compHS->at(icomp).at(i) - 1 << "  Layer: " << compLay->at(icomp) << endl;
                    cout << "Time Bins On:";
                    for(int ii = 0; ii < (int) compTimeOn->at(icomp).at(i).size(); ii++)
                    {
                        cout << " " << compTimeOn->at(icomp).at(i).at(ii);
                    }
                    cout << endl << endl;
                }
                mcomp = true;
            }
        }
        if(!mcomp) cout << "NO Comparators Found for this Segment!" << endl;

        //Fill Strip histos
        cStr->cd(iseg+1)->SetGridy();
        for(int istr = 0; istr < (int) stripId->size(); istr++)
        {
            if(chSid == stripId->at(istr))
            {
                for(int i = 0; i < (int) strip->at(istr).size(); i++)
                {
                    double ADCmax = 0;
                    bool fire = false;
                    double ped = 0.5*(stripADCs->at(istr).at(i).at(0) + stripADCs->at(istr).at(i).at(1));
                    for(int ii = 0; ii < (int) stripADCs->at(istr).at(i).size(); ii++)
                    {
                        double ADCn = stripADCs->at(istr).at(i).at(ii) - ped;
                        if(ADCn > ADCmax) ADCmax = ADCn;
                        if(ii > 2 && ii < 7 && ADCn > 13.3) fire = true;
                    }
                    if(fire) stripDis[iseg].SetBinContent(strip->at(istr).at(i),stripLay->at(istr),ADCmax);
                }
            }
        }

        if(iseg == 0)stripDis[iseg].Draw("COLZ");
        else stripDis[iseg].Draw("COLZ same");

        //Fill Comparator histos
        cComp->cd(iseg+1)->SetGridy();
        for(int icomp = 0; icomp < (int) compId->size(); icomp++)
        {
            if(chSid == compId->at(icomp))
            {
                for(int i = 0; i < (int) compStr->at(icomp).size(); i++)
                {
                    int layN = compLay->at(icomp);
                    if(layN%2==1) compDis[iseg].SetBinContent(2*compStr->at(icomp).at(i)+compHS->at(icomp).at(i),compLay->at(icomp),compTimeOn->at(icomp).at(i).at(0));
                    else compDis[iseg].SetBinContent((2*compStr->at(icomp).at(i)+compHS->at(icomp).at(i))-1,compLay->at(icomp),compTimeOn->at(icomp).at(i).at(0));
                }
            }
        }

        compDis[iseg].Draw("COLZ");

        //Fill Wire histos
        cWire->cd(iseg+1)->SetGridy();
        for(int iwire = 0; iwire < (int) wireId->size(); iwire++)
        {
            if(chSid == wireId->at(iwire))
            {
                for(int i = 0; i < (int) wireGrp->at(iwire).size(); i++)
                {
                    wireDis[iseg].SetBinContent(wireGrp->at(iwire).at(i),wireLay->at(iwire),wireTimeOn->at(iwire).at(i).at(0));
                }
            }
        }

        wireDis[iseg].Draw("COLZ");

        /*    //wire display
              c1->cd(1)->SetGridy();

              TH2F* wireDis = new TH2F("wireDis", "", 112, 1, 113, 6, 1, 7);
              TH2F* wireDis_text = new TH2F("wireDis_text", "", 112, 1, 113, 6, 1, 7);
              TPaveText *pt1 = new TPaveText(0.4,.95,0.6,0.99, "NDC");

              WireDisplay(id, layer_wire, wire, wireDis, wireDis_text);
              SetTitle(pt1, "Anode Hit Timing");

              wireDis->Draw("COLZ");
              wireDis_text->Draw("text same");
              pt1->Draw();
        //legend
        SetEventDisplayLegend(legendName);

        //comparator display
        c1->cd(2)->SetGridy();
        gPad->SetBottomMargin(0.15);
        TH2F* comparatorDis = new TH2F("comparatorDis", "", 160, 1, 161, 6, 1, 7);
        TH2F* comparatorDis_text = new TH2F("comparatorDis_text", "", 160, 1, 161, 6, 1, 7);
        TPaveText *pt4 = new TPaveText(0.4,.95,0.6,0.99, "NDC");

        ComparatorDisplay(id, layer_comparator, comparator, comparatorDis, comparatorDis_text);

        comparatorDis->Draw("COLZ");
        comparatorDis_text->Draw("text same");
        SetTitle(pt4, "Comparator Hit Timing");
        pt4->Draw();
        //strip hit display
        c1->cd(4)->SetGridy();

        TGraph* stripHitDis;
        TGraph* comparatorHitNotReadOut;
        vector<vector<double> > sHit_cHit_layer = StripHitDisplay(id, layer_strip, layer_comparator, strip, comparator, cfeb);
        double* sHit = &(sHit_cHit_layer[0][0]);
        double* sHitLayer = &(sHit_cHit_layer[1][0]);
        double* cHit = &(sHit_cHit_layer[2][0]);
        double* cHitLayer = &(sHit_cHit_layer[3][0]);

        if (int(sHit_cHit_layer[0].size()) > 0){
        stripHitDis = new TGraph(int(sHit_cHit_layer[0].size()), sHit, sHitLayer);
        TPaveText *pt2 = new TPaveText(0.4,.95,0.6,0.99, "NDC");
        SetTitle(pt2, "Strip Hit");
        stripHitDis->Draw("ap");
        pt2->Draw();
        //          stripHitDis->GetXaxis()->SetLimits(1, 82);
        //          stripHitDis->GetYaxis()->SetRangeUser(1, 7);
        //          stripHitDis->SetMarkerStyle(20);
        SetPlotDetail_StripHit(stripHitDis);

        }

        if (int(sHit_cHit_layer[3].size() > 0)){
        comparatorHitNotReadOut = new TGraph(int(sHit_cHit_layer[2].size()), cHit, cHitLayer);
        comparatorHitNotReadOut->Draw("p same");
        comparatorHitNotReadOut->SetMarkerStyle(24);
        comparatorHitNotReadOut->SetMarkerColor(2);
        }
        //        stripHitDis->GetXaxis()->SetRangeUser(1, 82);
        //        stripHitDis->GetYaxis()->SetLimits(1, 7);

        //        stripHitDis->SetMarkerStyle(20);
        //        comparatorHitNotReadOut->Draw("p same");
        cfebNotReadOut->Draw("B same");


        c1->Update();*/

    }
    cStr->Update();
    cStr->SaveAs(Form("plots/strips_%i.png",(int)entry));
    cComp->Update();
    cComp->SaveAs(Form("plots/comp_%i.png",(int)entry));
    cWire->Update();
    cWire->SaveAs(Form("plots/wire_%i.png",(int)entry));
}

Int_t CSCDigiTree::Cut(Long64_t entry)
{
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}
#endif // #ifdef CSCDigiTree_cxx
