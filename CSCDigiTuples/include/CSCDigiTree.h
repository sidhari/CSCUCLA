//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 21 19:22:35 2018 by ROOT version 6.06/01
// from TTree CSCDigiTree/Tree holding CSCDigis
// found on file: CSCDigiTree161031.root
//////////////////////////////////////////////////////////

#ifndef CSCDigiTree_h
#define CSCDigiTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

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
   Bool_t          ss;
   Bool_t          os;
   Double_t        Pt;
   Double_t        eta;
   Double_t        phi;
   Int_t           q;
   Int_t           Nseg;
   vector<int>     *segEc;
   vector<int>     *segSt;
   vector<int>     *segRi;
   vector<int>     *segCh;
   vector<float>   *segX;
   vector<float>   *segY;
   vector<float>   *segdXdZ;
   vector<float>   *segdYdZ;
   vector<int>     *rhId;
   vector<int>     *rhLay;
   vector<float>   *rhPos;
   vector<float>   *rhE;
   vector<float>   *rhMax;
   vector<int>     *lctId;
   vector<vector<int> > *lctQ;
   vector<vector<int> > *lctPat;
   vector<vector<int> > *lctKWG;
   vector<vector<int> > *lctKHS;
   vector<vector<int> > *lctBend;
   vector<vector<int> > *lctBX;
   vector<int>     *csctflctId;
   vector<vector<int> > *csctflctQ;
   vector<vector<int> > *csctflctPat;
   vector<vector<int> > *csctflctKWG;
   vector<vector<int> > *csctflctKHS;
   vector<vector<int> > *csctflctBend;
   vector<vector<int> > *csctflctBX;
   vector<int>     *emtflctId;
   vector<vector<int> > *emtflctQ;
   vector<vector<int> > *emtflctPat;
   vector<vector<int> > *emtflctKWG;
   vector<vector<int> > *emtflctKHS;
   vector<vector<int> > *emtflctBend;
   vector<vector<int> > *emtflctBX;
   vector<int>     *clctId;
   vector<vector<int> > *clctQ;
   vector<vector<int> > *clctPat;
   vector<vector<int> > *clctKHS;
   vector<vector<int> > *clctCFEB;
   vector<vector<int> > *clctBend;
   vector<vector<int> > *clctBX;
   vector<vector<int> > *clctFBX;
   vector<int>     *alctId;
   vector<vector<int> > *alctQ;
   vector<vector<int> > *alctKWG;
   vector<vector<int> > *alctAc;
   vector<vector<int> > *alctPB;
   vector<vector<int> > *alctBX;
   vector<vector<int> > *alctFBX;
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
   vector<int>     *dduId;
   vector<vector<int> > *dduHeader;
   vector<vector<int> > *dduTrailer;
   vector<int>     *dmbId;
   vector<vector<int> > *dmbHeader;
   vector<vector<int> > *dmbTrailer;
   vector<int>     *tmbId;
   vector<vector<int> > *tmbHeader;
   vector<vector<int> > *tmbTrailer;

   // List of branches
   TBranch        *b_Event_EventNumber;   //!
   TBranch        *b_Event_RunNumber;   //!
   TBranch        *b_Event_LumiSection;   //!
   TBranch        *b_Event_BXCrossing;   //!
   TBranch        *b_ss;   //!
   TBranch        *b_os;   //!
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
   TBranch        *b_lctBX;   //!
   TBranch        *b_csctflctId;   //!
   TBranch        *b_csctflctQ;   //!
   TBranch        *b_csctflctPat;   //!
   TBranch        *b_csctflctKWG;   //!
   TBranch        *b_csctflctKHS;   //!
   TBranch        *b_csctflctBend;   //!
   TBranch        *b_csctflctBX;   //!
   TBranch        *b_emtflctId;   //!
   TBranch        *b_emtflctQ;   //!
   TBranch        *b_emtflctPat;   //!
   TBranch        *b_emtflctKWG;   //!
   TBranch        *b_emtflctKHS;   //!
   TBranch        *b_emtflctBend;   //!
   TBranch        *b_emtflctBX;   //!
   TBranch        *b_clctId;   //!
   TBranch        *b_clctQ;   //!
   TBranch        *b_clctPat;   //!
   TBranch        *b_clctKHS;   //!
   TBranch        *b_clctCFEB;   //!
   TBranch        *b_clctBend;   //!
   TBranch        *b_clctBX;   //!
   TBranch        *b_clctFBX;   //!
   TBranch        *b_alctId;   //!
   TBranch        *b_alctQ;   //!
   TBranch        *b_alctKWG;   //!
   TBranch        *b_alctAc;   //!
   TBranch        *b_alctPB;   //!
   TBranch        *b_alctBX;   //!
   TBranch        *b_alctFBX;   //!
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
   TBranch        *b_dduId;   //!
   TBranch        *b_dduHeader;   //!
   TBranch        *b_dduTrailer;   //!
   TBranch        *b_dmbId;   //!
   TBranch        *b_dmbHeader;   //!
   TBranch        *b_dmbTrailer;   //!
   TBranch        *b_tmbId;   //!
   TBranch        *b_tmbHeader;   //!
   TBranch        *b_tmbTrailer;   //!

   CSCDigiTree(TTree *tree=0);
   virtual ~CSCDigiTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CSCDigiTree_cxx
CSCDigiTree::CSCDigiTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("CSCDigiTree161031.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("CSCDigiTree161031.root");
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
   lctBX = 0;
   csctflctId = 0;
   csctflctQ = 0;
   csctflctPat = 0;
   csctflctKWG = 0;
   csctflctKHS = 0;
   csctflctBend = 0;
   csctflctBX = 0;
   emtflctId = 0;
   emtflctQ = 0;
   emtflctPat = 0;
   emtflctKWG = 0;
   emtflctKHS = 0;
   emtflctBend = 0;
   emtflctBX = 0;
   clctId = 0;
   clctQ = 0;
   clctPat = 0;
   clctKHS = 0;
   clctCFEB = 0;
   clctBend = 0;
   clctBX = 0;
   clctFBX = 0;
   alctId = 0;
   alctQ = 0;
   alctKWG = 0;
   alctAc = 0;
   alctPB = 0;
   alctBX = 0;
   alctFBX = 0;
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
   dduId = 0;
   dduHeader = 0;
   dduTrailer = 0;
   dmbId = 0;
   dmbHeader = 0;
   dmbTrailer = 0;
   tmbId = 0;
   tmbHeader = 0;
   tmbTrailer = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Event_EventNumber", &Event_EventNumber, &b_Event_EventNumber);
   fChain->SetBranchAddress("Event_RunNumber", &Event_RunNumber, &b_Event_RunNumber);
   fChain->SetBranchAddress("Event_LumiSection", &Event_LumiSection, &b_Event_LumiSection);
   fChain->SetBranchAddress("Event_BXCrossing", &Event_BXCrossing, &b_Event_BXCrossing);
   fChain->SetBranchAddress("ss", &ss, &b_ss);
   fChain->SetBranchAddress("os", &os, &b_os);
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
   fChain->SetBranchAddress("lctBX", &lctBX, &b_lctBX);
   fChain->SetBranchAddress("csctflctId", &csctflctId, &b_csctflctId);
   fChain->SetBranchAddress("csctflctQ", &csctflctQ, &b_csctflctQ);
   fChain->SetBranchAddress("csctflctPat", &csctflctPat, &b_csctflctPat);
   fChain->SetBranchAddress("csctflctKWG", &csctflctKWG, &b_csctflctKWG);
   fChain->SetBranchAddress("csctflctKHS", &csctflctKHS, &b_csctflctKHS);
   fChain->SetBranchAddress("csctflctBend", &csctflctBend, &b_csctflctBend);
   fChain->SetBranchAddress("csctflctBX", &csctflctBX, &b_csctflctBX);
   fChain->SetBranchAddress("emtflctId", &emtflctId, &b_emtflctId);
   fChain->SetBranchAddress("emtflctQ", &emtflctQ, &b_emtflctQ);
   fChain->SetBranchAddress("emtflctPat", &emtflctPat, &b_emtflctPat);
   fChain->SetBranchAddress("emtflctKWG", &emtflctKWG, &b_emtflctKWG);
   fChain->SetBranchAddress("emtflctKHS", &emtflctKHS, &b_emtflctKHS);
   fChain->SetBranchAddress("emtflctBend", &emtflctBend, &b_emtflctBend);
   fChain->SetBranchAddress("emtflctBX", &emtflctBX, &b_emtflctBX);
   fChain->SetBranchAddress("clctId", &clctId, &b_clctId);
   fChain->SetBranchAddress("clctQ", &clctQ, &b_clctQ);
   fChain->SetBranchAddress("clctPat", &clctPat, &b_clctPat);
   fChain->SetBranchAddress("clctKHS", &clctKHS, &b_clctKHS);
   fChain->SetBranchAddress("clctCFEB", &clctCFEB, &b_clctCFEB);
   fChain->SetBranchAddress("clctBend", &clctBend, &b_clctBend);
   fChain->SetBranchAddress("clctBX", &clctBX, &b_clctBX);
   fChain->SetBranchAddress("clctFBX", &clctFBX, &b_clctFBX);
   fChain->SetBranchAddress("alctId", &alctId, &b_alctId);
   fChain->SetBranchAddress("alctQ", &alctQ, &b_alctQ);
   fChain->SetBranchAddress("alctKWG", &alctKWG, &b_alctKWG);
   fChain->SetBranchAddress("alctAc", &alctAc, &b_alctAc);
   fChain->SetBranchAddress("alctPB", &alctPB, &b_alctPB);
   fChain->SetBranchAddress("alctBX", &alctBX, &b_alctBX);
   fChain->SetBranchAddress("alctFBX", &alctFBX, &b_alctFBX);
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
   fChain->SetBranchAddress("dduId", &dduId, &b_dduId);
   fChain->SetBranchAddress("dduHeader", &dduHeader, &b_dduHeader);
   fChain->SetBranchAddress("dduTrailer", &dduTrailer, &b_dduTrailer);
   fChain->SetBranchAddress("dmbId", &dmbId, &b_dmbId);
   fChain->SetBranchAddress("dmbHeader", &dmbHeader, &b_dmbHeader);
   fChain->SetBranchAddress("dmbTrailer", &dmbTrailer, &b_dmbTrailer);
   fChain->SetBranchAddress("tmbId", &tmbId, &b_tmbId);
   fChain->SetBranchAddress("tmbHeader", &tmbHeader, &b_tmbHeader);
   fChain->SetBranchAddress("tmbTrailer", &tmbTrailer, &b_tmbTrailer);
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

void CSCDigiTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CSCDigiTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CSCDigiTree_cxx
