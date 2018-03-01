//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 21 19:22:35 2018 by ROOT version 6.06/01
// from TTree toyMCtree/Tree holding CSCDigis
// found on file: toyMCtree161031.root
//////////////////////////////////////////////////////////

#ifndef toyMCtree_h
#define toyMCtree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TMath.h>
#include <TRandom.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class toyMCtree {
public :
   TTree          *tree;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TRandom         rand;

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

   toyMCtree();
   virtual ~toyMCtree();
   int              chamberSerial(int ec, int st, int ri, int ch);
   virtual void     Init();
   virtual void     Loop(int Nev);
};

#endif

#ifdef toyMCtree_cxx
toyMCtree::toyMCtree() 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   rand.SetSeed(0);
   Init();
}

toyMCtree::~toyMCtree()
{
}

void toyMCtree::Init()
{

   // Set object pointers
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
   fCurrent = -1;
   tree = new TTree("CSCDigiTree","Toy Segment MC Tree");

   tree->Branch("Event_EventNumber", &Event_EventNumber, "Event_EventNumber/I");
   tree->Branch("Event_RunNumber", &Event_RunNumber, "Event_RunNumber/I");
   tree->Branch("Event_LumiSection", &Event_LumiSection, "Event_LumiSection/I");
   tree->Branch("Event_BXCrossing", &Event_BXCrossing, "Event_BXCrossing/I");
   tree->Branch("ss", &ss);
   tree->Branch("os", &os);
   tree->Branch("Pt", &Pt, "Pt/D");
   tree->Branch("eta", &eta, "eta/D");
   tree->Branch("phi", &phi, "phi/D");
   tree->Branch("q", &q, "q/D");
   tree->Branch("Nseg", &Nseg, "Nseg/I");
   tree->Branch("segEc", &segEc);
   tree->Branch("segSt", &segSt);
   tree->Branch("segRi", &segRi);
   tree->Branch("segCh", &segCh);
   tree->Branch("segX", &segX);
   tree->Branch("segY", &segY);
   tree->Branch("segdXdZ", &segdXdZ);
   tree->Branch("segdYdZ", &segdYdZ);
   tree->Branch("rhId", &rhId);
   tree->Branch("rhLay", &rhLay);
   tree->Branch("rhPos", &rhPos);
   tree->Branch("rhE", &rhE);
   tree->Branch("rhMax", &rhMax);
   tree->Branch("lctId", &lctId);
   tree->Branch("lctQ", &lctQ);
   tree->Branch("lctPat", &lctPat);
   tree->Branch("lctKWG", &lctKWG);
   tree->Branch("lctKHS", &lctKHS);
   tree->Branch("lctBend", &lctBend);
   tree->Branch("lctBX", &lctBX);
   tree->Branch("csctflctId", &csctflctId);
   tree->Branch("csctflctQ", &csctflctQ);
   tree->Branch("csctflctPat", &csctflctPat);
   tree->Branch("csctflctKWG", &csctflctKWG);
   tree->Branch("csctflctKHS", &csctflctKHS);
   tree->Branch("csctflctBend", &csctflctBend);
   tree->Branch("csctflctBX", &csctflctBX);
   tree->Branch("emtflctId", &emtflctId);
   tree->Branch("emtflctQ", &emtflctQ);
   tree->Branch("emtflctPat", &emtflctPat);
   tree->Branch("emtflctKWG", &emtflctKWG);
   tree->Branch("emtflctKHS", &emtflctKHS);
   tree->Branch("emtflctBend", &emtflctBend);
   tree->Branch("emtflctBX", &emtflctBX);
   tree->Branch("clctId", &clctId);
   tree->Branch("clctQ", &clctQ);
   tree->Branch("clctPat", &clctPat);
   tree->Branch("clctKHS", &clctKHS);
   tree->Branch("clctCFEB", &clctCFEB);
   tree->Branch("clctBend", &clctBend);
   tree->Branch("clctBX", &clctBX);
   tree->Branch("clctFBX", &clctFBX);
   tree->Branch("alctId", &alctId);
   tree->Branch("alctQ", &alctQ);
   tree->Branch("alctKWG", &alctKWG);
   tree->Branch("alctAc", &alctAc);
   tree->Branch("alctPB", &alctPB);
   tree->Branch("alctBX", &alctBX);
   tree->Branch("alctFBX", &alctFBX);
   tree->Branch("compId", &compId);
   tree->Branch("compLay", &compLay);
   tree->Branch("compStr", &compStr);
   tree->Branch("compHS", &compHS);
   tree->Branch("compTimeOn", &compTimeOn);
   tree->Branch("wireId", &wireId);
   tree->Branch("wireLay", &wireLay);
   tree->Branch("wireGrp", &wireGrp);
   tree->Branch("wireTimeOn", &wireTimeOn);
   tree->Branch("stripId", &stripId);
   tree->Branch("stripLay", &stripLay);
   tree->Branch("strip", &strip);
   tree->Branch("stripADCs", &stripADCs);
   tree->Branch("dduId", &dduId);
   tree->Branch("dduHeader", &dduHeader);
   tree->Branch("dduTrailer", &dduTrailer);
   tree->Branch("dmbId", &dmbId);
   tree->Branch("dmbHeader", &dmbHeader);
   tree->Branch("dmbTrailer", &dmbTrailer);
   tree->Branch("tmbId", &tmbId);
   tree->Branch("tmbHeader", &tmbHeader);
   tree->Branch("tmbTrailer", &tmbTrailer);
}

int toyMCtree::chamberSerial(int ec, int st, int ri, int ch) 
{
    int kSerial = ch;
    //cout<<"Endcap "<<ec<<" Station "<<st<<" Ring "<<ri<<" Chamber "<<ch<<" Layer "<<lay<<endl;
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
    return kSerial;
}

#endif // #ifdef toyMCtree_cxx
