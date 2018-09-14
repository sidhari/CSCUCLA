#ifndef CSCUCLA_CSCDIGITUPLES_FILLCSCINFO_H
#define CSCUCLA_CSCDIGITUPLES_FILLCSCINFO_H


/* Original Author: Nick McColl
 *
 *   	    Author: William Nash
 *  Date Inherited: August 30th 2018
 *
 */

#include "FWCore/Framework/interface/Event.h"

#include <DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h>
#include <DataFormats/CSCDigi/interface/CSCStripDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCWireDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h>
#include <DataFormats/CSCRecHit/interface/CSCSegmentCollection.h>
#include <DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h>
#include "CSCUCLA/CSCDigiTuples/include/CSCHelper.h"

//muon
#include "DataFormats/MuonReco/interface/MuonFwd.h"

//segments
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"

#include "CSCInfo.h"


#include "TFile.h"
#include "TTree.h"
#include "TString.h"

/* TODO:
 * - make TreeWriter, TreeReader class (inherit from TreeContainer?)
 * - make each info class (change name?) take as a parameter TreeWriter / TreeReader
 * 		-set branch address, or makes new branch on instantiation
 * 		-
 *
 * - fill should be overloaded:
 * 		- takes CMSSW variables with treewriter
 * 		- takes no argument with treereader, eats branch
 *
 *
 */


typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

class TreeContainer {
public:
  TreeContainer(TString fileName, TString treeName, TString treeTitle){
	file = new TFile(fileName, "RECREATE");
	tree = new TTree(treeName,treeTitle);
  }
  void write() {
    file->cd();
    tree->Write();
    file->Close();
    delete file;
  }

  void fill() {tree->Fill();}
  TFile * file;
  TTree * tree;

};

class FillInfo {
public:
  FillInfo(const std::string& prfx, TreeContainer& tree) :
	  prefix(prfx),
	  fTree(&tree)
{reset();};

  virtual ~FillInfo() {};
  virtual void reset() {};
protected:
  //const TString prefix; //prefix before each tree branch associated with this object
  const std::string prefix; //prefix before each tree branch associated with this object


  //Book single variable
  template<class T>
  void    book(const char *name, T& var, const char *type) { fTree->tree->Branch(
		  //TString(prefix).Append('_').Append(name).Data(),
		  (prefix+string("_")+string(name)).c_str(),
		 // TString(prefix).Append('_').Append(name).Data(),
		  &var,
		  (string(name)+string("/")+string(type)).c_str());}
		  //TString(name).Append("/").Append(type).Data()); }

  //Book vector
  template<class T>
  void    book(const char *name, std::vector<T>& varv)   { fTree->tree->Branch(
		  //TString(prefix).Append('_').Append(name).Data(),
		  (prefix+string("_")+string(name)).c_str(),
		  &varv); }

  TreeContainer * fTree;

};


class FillEventInfo : public FillInfo {
public:

  FillEventInfo(TreeContainer& tree) :
	  FillInfo(e.name,tree)
	  //FillInfo("help",tree)
{
	  cout << e.name << endl;
    book("EventNumber",e.EventNumber,"l");
    book("RunNumber"  ,e.RunNumber  ,"l");
    book("LumiSection",e.LumiSection,"I");
    book("BXCrossing" ,e.BXCrossing ,"I");
  }
  virtual ~FillEventInfo() {};

  virtual void reset(){
    e.EventNumber  = 0;
    e.RunNumber    = 0;
    e.LumiSection  = -1;
    e.BXCrossing   = -1;
  }

private:
  CSCInfo::Event e;

  public:

  void fill(const edm::Event& iEvent);

};


class FillMuonInfo : public FillInfo {
public:
	FillMuonInfo(TreeContainer& tree) :
		FillInfo(m.name,tree)
{

		//might want to make a constructor for the object here...
		m.pt = new std::vector<float>();
		m.eta = new std::vector<float>();
		m.phi = new std::vector<float>();
		m.q = new std::vector<int>();
		m.isGlobal = new std::vector<bool>();
		m.isTracker = new std::vector<bool>();
		book("pt"		,*m.pt			);
		book("eta"		,*m.eta		);
		book("phi"		,*m.phi		);
		book("q"		,*m.q			);
		book("isGlobal"	,*m.isGlobal	);
		book("isTracker",*m.isTracker	);
}
	virtual ~FillMuonInfo() {
		delete m.pt;
		delete m.eta;
		delete m.phi;
		delete m.q;
		delete m.isGlobal;
		delete m.isTracker;
	};

	virtual void reset() {
		m.pt->clear();
		m.eta->clear();
		m.phi->clear();
		m.q->clear();
		m.isGlobal->clear();
		m.isTracker->clear();
	}
private:
	CSCInfo::Muons m;

public:

	void fill(const reco::MuonCollection& muons);
};


class FillSegmentInfo : public FillInfo {
public:

  FillSegmentInfo(TreeContainer& tree) :
	  FillInfo(s.name,tree) {
	s.mu_id = new vector<int>();
	s.ch_id = new vector<int>();
	s.pos_x = new vector<float>();
	s.pos_y = new vector<float>();
	s.dxdz = new vector <float>();
	s.dydz = new vector<float>();
	s.chisq = new vector<float>();
	s.nHits = new vector<size8>();
    book("mu_id"       ,*s.mu_id      );
    book("ch_id"       ,*s.ch_id      );
    book("pos_x"       ,*s.pos_x      );
    book("pos_y"       ,*s.pos_y      );
    book("dxdz"        ,*s.dxdz       );
    book("dydz"        ,*s.dydz       );
    book("chisq"       ,*s.chisq      );
    book("nHits"       ,*s.nHits      );

  }
  virtual ~FillSegmentInfo() {
	  delete s.mu_id;
	  delete s.ch_id;
	  delete s.pos_x;
	  delete s.pos_y;
	  delete s.dxdz;
	  delete s.dydz;
	  delete s.chisq;
	  delete s.nHits;
  };

  virtual void reset(){
    s.mu_id       ->clear();
    s.ch_id       ->clear();
    s.pos_x       ->clear();
    s.pos_y       ->clear();
    s.dxdz        ->clear();
    s.dydz        ->clear();
    s.chisq       ->clear();
    s.nHits       ->clear();
  }


private:
  	 CSCInfo::Segments s;


  public:

  //void fill(const CSCSegmentCollection& segments, const CSCRecHit2DCollection * recHits = 0);
  void fill(std::vector<const CSCSegment*>& segments, const CSCGeometry* theCSC, int mu_index = -1);
  void fill(const CSCSegment& segment, const CSCGeometry* theCSC, int mu_index);
  size16 findRecHitIdx(const CSCRecHit2D& hit, const CSCRecHit2DCollection* allRecHits);

};

class FillRecHitInfo : public FillInfo {
public:

	FillRecHitInfo(TreeContainer& tree) :
			FillInfo(r.name, tree) {
		r.mu_id = new vector<int>();
		r.ch_id = new vector<int>();
		r.lay = new vector<size8>();
		r.pos_x = new vector<float>();
		r.pos_y = new vector<float>();
		r.e = new vector<float>();
		r.max_adc = new vector<float>();
		book("mu_id", *r.mu_id);
		book("ch_id", *r.ch_id);
		book("lay", *r.lay);
		book("pos_x", *r.pos_x);
		book("pos_y", *r.pos_y);
		book("e", *r.e);
		book("max_adc", *r.max_adc);

	}
	virtual ~FillRecHitInfo() {
		delete r.mu_id;
		delete r.ch_id;
		delete r.lay;
		delete r.pos_x;
		delete r.pos_y;
		delete r.e;
		delete r.max_adc;
	}
	;

	virtual void reset() {
		r.mu_id->clear();
		r.ch_id->clear();
		r.lay->clear();
		r.pos_x->clear();
		r.pos_y->clear();
		r.e->clear();
		r.max_adc->clear();
	}

private:
	CSCInfo::RecHits r;


public:

  //void fill(const CSCRecHit2DCollection& recHits);
  void fill(const std::vector<CSCRecHit2D>& recHits, int mu_index);

};

class FillLCTInfo: public FillInfo {
public:

	FillLCTInfo(TreeContainer& tree) :
			FillInfo(l.name, tree) {
		l.ch_id = new vector<size16>();
		l.quality = new vector<size8>();
		l.pattern = new vector<size8>();
		l.bend = new vector<size8>();
		l.keyWireGroup = new vector<size8>();
		l.keyHalfStrip = new vector<size8>();
		l.bunchCross = new vector<size8>();
		book("ch_id", *l.ch_id);
		book("quality", *l.quality);
		book("pattern", *l.pattern);
		book("bend", *l.bend);
		book("keyWireGroup", *l.keyWireGroup);
		book("keyHalfStrip", *l.keyHalfStrip);
		book("bunchCross", *l.bunchCross);

	}
	virtual ~FillLCTInfo() {
		delete l.ch_id;
		delete l.quality;
		delete l.pattern;
		delete l.bend;
		delete l.keyWireGroup;
		delete l.keyHalfStrip;
		delete l.bunchCross;
	};

	virtual void reset() {
		l.ch_id->clear();
		l.quality->clear();
		l.pattern->clear();
		l.bend->clear();
		l.keyWireGroup->clear();
		l.keyHalfStrip->clear();
		l.bunchCross->clear();
	}

private:
	CSCInfo::LCTs l;

public:

	void fill(const CSCCorrelatedLCTDigiCollection& lcts);

};



class FillCLCTInfo: public FillInfo {
public:

	FillCLCTInfo(TreeContainer& tree) :
			FillInfo(c.name, tree) {
		c.ch_id = new vector<size16>();
		c.isvalid = new vector<size8>();
		c.quality = new vector<size16>();
		c.pattern = new vector<size8>();
		c.stripType = new vector<size8>();
		c.bend = new vector<size8>();
		c.halfStrip = new vector<size8>();
		c.CFEB = new vector<size8>();
		c.BX = new vector<size8>();
		c.trkNumber = new vector<size8>();
		c.keyStrip = new vector<size8>();
		book("ch_id", *c.ch_id);
		book("isValid", *c.isvalid);
		book("quality", *c.quality);
		book("pattern", *c.pattern);
		book("stripType", *c.stripType);
		book("bend", *c.bend);
		book("halfStrip", *c.halfStrip);
		book("CFEB", *c.CFEB);
		book("BX", *c.BX);
		book("trkNumber", *c.trkNumber);
		book("keyStrip", *c.keyStrip);

	}
	virtual ~FillCLCTInfo() {
		delete c.ch_id;
		delete c.isvalid;
		delete c.quality;
		delete c.pattern;
		delete c.stripType;
		delete c.bend;
		delete c.halfStrip;
		delete c.CFEB;
		delete c.BX;
		delete c.trkNumber;
		delete c.keyStrip;
	}
	;

	virtual void reset() {
		c.ch_id->clear();
		c.isvalid->clear();
		c.quality->clear();
		c.pattern->clear();
		c.stripType->clear();
		c.bend->clear();
		c.halfStrip->clear();
		c.CFEB->clear();
		c.BX->clear();
		c.trkNumber->clear();
		c.keyStrip->clear();
	}

private:
	CSCInfo::CLCTs c;

public:

	void fill(const CSCCLCTDigiCollection& clcts);

};


class FillCompInfo : public FillInfo {
public:

  FillCompInfo(TreeContainer& tree) :
	  FillInfo(c.name, tree) {
	  c.ch_id = new vector<int>();
	  c.lay = new vector<size8>();
	  c.strip = new vector<size8>();
	  c.halfStrip = new vector<size8>();
	  c.bestTime = new vector<size8>();
	  c.nTimeOn = new vector<size8>();

		book("ch_id", *c.ch_id);
		book("lay", *c.lay);
		book("strip", *c.strip);
		book("halfStrip", *c.halfStrip);
		book("bestTime", *c.bestTime);
		book("nTimeOn", *c.nTimeOn);



  }
  virtual ~FillCompInfo() {
	  delete c.ch_id;
	  delete c.lay;
	  delete c.strip;
	  delete c.halfStrip;
	  delete c.bestTime;
	  delete c.nTimeOn;
  };

  virtual void reset(){
	  c.ch_id->clear();
	  c.lay->clear();
	  c.strip->clear();
	  c.halfStrip->clear();
	  c.bestTime->clear();
	  c.nTimeOn->clear();
  }

private:
  CSCInfo::Comparators c;

  public:

  void fill(const CSCComparatorDigiCollection& strips);

};



/*
class FillStripInfo : public FillInfo {
public:

  FillStripInfo(TreeContainer& tree) :FillInfo(tree) {
    book("strip_id"           ,strip_id           );
    book("strip_lay"          ,strip_lay          );
    book("strip_number"       ,strip_number      );



  }
  virtual ~FillStripInfo() {};

private:
  std::vector<size16>   strip_id     ;
  std::vector<size8>    strip_lay    ;
  std::vector<size8>    strip_number ;


  virtual void reset(){
    strip_id     .clear();
    strip_lay    .clear();
    strip_number .clear();
  }

  public:

  void fill(const CSCStripDigiCollection& strips);

};

class FillCompInfo : public FillInfo {
public:

  FillCompInfo(TreeContainer& tree) :FillInfo(tree) {
    book("comp_id"      ,comp_id          );
    book("comp_lay"     ,comp_lay         );
    book("comp_strip"   ,comp_strip       );
    book("comp_comp"    ,comp_comp        );



  }
  virtual ~FillCompInfo() {};

private:
  std::vector<size16> comp_id    ;
  std::vector<size8>  comp_lay   ;
  std::vector<size8>  comp_strip ;
  std::vector<size8>  comp_comp  ;


  virtual void reset(){
    comp_id    .clear();
    comp_lay   .clear();
    comp_strip .clear();
    comp_comp  .clear();
  }

  public:

  void fill(const CSCComparatorDigiCollection& strips);

};


class FillWireInfo : public FillInfo {
public:

  FillWireInfo(TreeContainer& tree) :FillInfo(tree) {
    book("wire_id"     ,wire_id         );
    book("wire_lay"    ,wire_lay        );
    book("wire_grp"    ,wire_grp        );
    book("wire_time"   ,wire_time       );
    book("wire_bx"     ,wire_bx         );



  }
  virtual ~FillWireInfo() {};

private:
  std::vector<size16>     wire_id  ;
  std::vector<size8>      wire_lay ;
  std::vector<size8>      wire_grp ;
  std::vector<size8>      wire_time;
  std::vector<int>        wire_bx;


  virtual void reset(){
    wire_id   .clear();
    wire_lay  .clear();
    wire_grp  .clear();
    wire_time .clear();
    wire_bx   .clear();
  }

  public:

  void fill(const CSCWireDigiCollection& wires);

};





class FillALCTInfo : public FillInfo {
public:

  FillALCTInfo(TreeContainer& tree) :FillInfo(tree) {

    book("alct_id"         , alct_id        );
    book("alct_isvalid"    , alct_isvalid   );
    book("alct_quality"    , alct_quality   );
    book("alct_accel"      , alct_accel     );
    book("alct_collB"      , alct_collB     );
    book("alct_wireGroup"  , alct_wireGroup );
    book("alct_BX"         , alct_BX        );
    book("alct_trkNumber"  , alct_trkNumber );



  }
  virtual ~FillALCTInfo() {};

private:
   std::vector<size16> alct_id          ;
   std::vector<size8>  alct_isvalid     ;
   std::vector<size8>  alct_quality     ;
   std::vector<size8>  alct_accel       ;
   std::vector<size8>  alct_collB       ;
   std::vector<size8>  alct_wireGroup   ;
   std::vector<size8>  alct_BX          ;
   std::vector<size8>  alct_trkNumber   ;


  virtual void reset(){
    alct_id         .clear();
    alct_isvalid    .clear();
    alct_quality    .clear();
    alct_accel      .clear();
    alct_collB      .clear();
    alct_wireGroup  .clear();
    alct_BX         .clear();
    alct_trkNumber  .clear();
  }

  public:

  void fill(const CSCALCTDigiCollection& alcts);

};

*/

#endif /*CSCUCLA_CSCDIGITUPLES_FILLCSCINFO_H*/
