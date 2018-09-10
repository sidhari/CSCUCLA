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


#include "TFile.h"
#include "TTree.h"
#include "TString.h"


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
  FillInfo(const TString prfx, TreeContainer& tree) :
	  prefix(prfx),
	  fTree(&tree)
{reset();};

  virtual ~FillInfo() {};
  virtual void reset() {};
protected:
  const TString prefix; //prefix before each tree branch associated with this object


  //Book single variable
  template<class T>
  void    book(const char *name, T& var, const char *type) { fTree->tree->Branch(
		  TString(prefix).Append('_').Append(name).Data(),
		  &var,
		  TString(name).Append("/").Append(type).Data()); }

  //Book vector
  template<class T>
  void    book(const char *name, std::vector<T>& varv)   { fTree->tree->Branch(
		  TString(prefix).Append('_').Append(name).Data(),
		  &varv); }

  TreeContainer * fTree;

};


class FillEventInfo : public FillInfo {
public:

  FillEventInfo(TreeContainer& tree) :
	  FillInfo("Event",tree)
{
    book("EventNumber",EventNumber,"l");
    book("RunNumber"  ,RunNumber  ,"l");
    book("LumiSection",LumiSection,"I");
    book("BXCrossing" ,BXCrossing ,"I");
  }
  virtual ~FillEventInfo() {};

  virtual void reset(){
    EventNumber  = -1;
    RunNumber    = -1;
    LumiSection  = -1;
    BXCrossing   = -1;
  }

private:
  unsigned long EventNumber;
  unsigned long RunNumber  ;
  int LumiSection;
  int BXCrossing ;



  public:

  void fill(const edm::Event& iEvent);

};

class FillMuonInfo : public FillInfo {
public:
	FillMuonInfo(TreeContainer& tree) :
		FillInfo("muon",tree)
{
		book("pt"		,pt			);
		book("eta"		,eta		);
		book("phi"		,phi		);
		book("q"		,q			);
		book("isGlobal"	,isGlobal	);
		book("isTracker",isTracker	);
}
	virtual ~FillMuonInfo() {};

	virtual void reset() {
		pt.clear();
		eta.clear();
		phi.clear();
		q.clear();
		isGlobal.clear();
		isTracker.clear();
	}
private:
	std::vector<float> pt;
	std::vector<float> eta;
	std::vector<float> phi;
	std::vector<int> 	q; //charge
	std::vector<bool>  isGlobal;
	std::vector<bool>  isTracker;

public:

	void fill(const reco::MuonCollection& m);
};


class FillSegmentInfo : public FillInfo {
public:

  FillSegmentInfo(TreeContainer& tree) :
	  FillInfo("segment",tree) {

    book("mu_id"       ,mu_id      );
    book("ch_id"       ,ch_id      );
    book("pos_x"       ,pos_x      );
    book("pos_y"       ,pos_y      );
    book("dxdz"        ,dxdz       );
    book("dydz"        ,dydz       );
    book("chisq"       ,chisq      );
    book("nHits"       ,nHits      );

  }
  virtual ~FillSegmentInfo() {};

  virtual void reset(){
    mu_id       .clear();
    ch_id       .clear();
    pos_x       .clear();
    pos_y       .clear();
    dxdz        .clear();
    dydz        .clear();
    chisq       .clear();
    nHits       .clear();
  }


private:
     std::vector<int>     mu_id          ; //id associated with the muon
     std::vector<int>     ch_id          ; //id associated with chamber
     std::vector<float>   pos_x          ; // [strips]
     std::vector<float>   pos_y          ;
     std::vector<float>   dxdz           ; // [strips / layer]
     std::vector<float>   dydz           ;
     std::vector<float>   chisq          ;
     std::vector<size8>   nHits          ;


  public:

  //void fill(const CSCSegmentCollection& segments, const CSCRecHit2DCollection * recHits = 0);
  void fill(std::vector<const CSCSegment*>& segments, const CSCGeometry* theCSC, int mu_index = -1);
  void fill(const CSCSegment& segment, const CSCGeometry* theCSC, int mu_index);
  size16 findRecHitIdx(const CSCRecHit2D& hit, const CSCRecHit2DCollection* allRecHits);

};

class FillRecHitInfo : public FillInfo {
public:

	FillRecHitInfo(TreeContainer& tree) :
			FillInfo("rh", tree) {
		book("mu_id", mu_id);
		book("ch_id", ch_id);
		book("lay", lay);
		book("pos_x", pos_x);
		book("pos_y", pos_y);
		book("e", e);
		book("max_adc", max_adc);

	}
	virtual ~FillRecHitInfo() {
	}
	;

private:
	std::vector<int> mu_id;
	std::vector<int> ch_id;
	std::vector<size8> lay;
	std::vector<float> pos_x;
	std::vector<float> pos_y;
	std::vector<float> e; //energy
	std::vector<float> max_adc; // time bin with the max ADCs

	virtual void reset() {
		mu_id.clear();
		ch_id.clear();
		lay.clear();
		pos_x.clear();
		pos_y.clear();
		e.clear();
		max_adc.clear();
	}

public:

  //void fill(const CSCRecHit2DCollection& recHits);
  void fill(const vector<CSCRecHit2D>& recHits, int mu_index);

};

/*
class FillLCTInfo : public FillInfo {
public:

  FillLCTInfo(TreeContainer& tree) :FillInfo(tree) {

    book("lct_id"          ,lct_id          );
    book("lct_quality"     ,lct_quality     );
    book("lct_pattern"     ,lct_pattern     );
    book("lct_bend"        ,lct_bend        );
    book("lct_keyWireGroup",lct_keyWireGroup);
    book("lct_keyHalfStrip",lct_keyHalfStrip);
    book("lct_nuchCross"   ,lct_bunchCross  );



  }
  virtual ~FillLCTInfo() {};

private:
   std::vector<size16> lct_id          ;
   std::vector<size8>  lct_quality     ;
   std::vector<size8>  lct_pattern     ;
   std::vector<size8>  lct_bend        ;
   std::vector<size8>  lct_keyWireGroup;
   std::vector<size8>  lct_keyHalfStrip;
   std::vector<size8>  lct_bunchCross;


  virtual void reset(){
    lct_id           .clear();
    lct_quality      .clear();
    lct_pattern      .clear();
    lct_bend         .clear();
    lct_keyWireGroup .clear();
    lct_keyHalfStrip .clear();
    lct_bunchCross   .clear();
  }

  public:

  void fill(const CSCCorrelatedLCTDigiCollection& lcts);

};



class FillCLCTInfo : public FillInfo {
public:

  FillCLCTInfo(TreeContainer& tree) :FillInfo(tree) {

    book("clct_id"         , clct_id        );
    book("clct_isvalid"    , clct_isvalid   );
    book("clct_quality"    , clct_quality   );
    book("clct_pattern"    , clct_pattern   );
    book("clct_stripType"  , clct_stripType );
    book("clct_bend"       , clct_bend      );
    book("clct_halfStrip"  , clct_halfStrip );
    book("clct_CFEB"       , clct_CFEB      );
    book("clct_BX"         , clct_BX        );
    book("clct_trkNumber"  , clct_trkNumber );
    book("clct_keyStrip"   , clct_keyStrip  );

  }
  virtual ~FillCLCTInfo() {};

private:
   std::vector<size16> clct_id          ;
   std::vector<size8>  clct_isvalid     ;
   std::vector<size16> clct_quality     ;
   std::vector<size8>  clct_pattern     ;
   std::vector<size8>  clct_stripType   ;
   std::vector<size8>  clct_bend        ;
   std::vector<size8>  clct_halfStrip   ;
   std::vector<size8>  clct_CFEB        ;
   std::vector<size8>  clct_BX          ;
   std::vector<size8>  clct_trkNumber   ;
   std::vector<size8>  clct_keyStrip    ;


  virtual void reset(){
    clct_id         .clear();
    clct_isvalid    .clear();
    clct_quality    .clear();
    clct_pattern    .clear();
    clct_stripType  .clear();
    clct_bend       .clear();
    clct_halfStrip  .clear();
    clct_CFEB       .clear();
    clct_BX         .clear();
    clct_trkNumber  .clear();
    clct_keyStrip   .clear();
  }

  public:

  void fill(const CSCCLCTDigiCollection& clcts);

};

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
