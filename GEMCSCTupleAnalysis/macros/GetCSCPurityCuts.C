
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "../include/BaseCSCAndGEMAnalyzer.h"
#include "include/HistGetter.h"
#include "TMath.h"

using namespace std;
using namespace CSCGEMTuples;

class Analyzer : public AnalyzeCSC{
public:
  Analyzer(std::string fileName, std::string treeName) : AnalyzeCSC(fileName,treeName){
    bookHistos();
  }



  void bookHistos() {
    plotter.book1D("nSegments",";# of segments",100,-0.5,99.5);

    //All after the nSeg = 1 cut
    plotter.book1D("nRecHitsInSeg",";# of rechits in segment",7,-0.5,6.5);
    plotter.book1D("nEXRecHits",";# of non-segment rechits",100,-0.5,99.5);
    plotter.book1D("projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("projError_y",";projected segment x-position error [cm]",100,0,20);

    plotter.book1D("nrh_eq6_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_projError_y",";projected segment y-position error [cm]",100,0,20);
    plotter.book1D("nrh_eq5_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq5_projError_y",";projected segment y-position error [cm]",100,0,20);
    plotter.book1D("nrh_eq4_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq4_projError_y",";projected segment y-position error [cm]",100,0,20);
    plotter.book1D("nrh_eq3_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq3_projError_y",";projected segment y-position error [cm]",100,0,20);

    plotter.book1D("nrh_eq6_proj_x",";projected segment x-position [cm]",100,-20,20);
    plotter.book1D("nrh_eq6_proj_y",";projected segment y-position [cm]",100,-80,80);
    plotter.book1D("nrh_eq5_proj_x",";projected segment x-position [cm]",100,-20,20);
    plotter.book1D("nrh_eq5_proj_y",";projected segment y-position [cm]",100,-80,80);
    plotter.book1D("nrh_eq4_proj_x",";projected segment x-position [cm]",100,-20,20);
    plotter.book1D("nrh_eq4_proj_y",";projected segment y-position [cm]",100,-80,80);
    plotter.book1D("nrh_eq3_proj_x",";projected segment x-position [cm]",100,-20,20);
    plotter.book1D("nrh_eq3_proj_y",";projected segment y-position [cm]",100,-80,80);

    plotter.book1D("nEXRecHits_eq0_nrh_eq6_proj_x",";projected segment x-position [cm]",100,-20,20);
    plotter.book1D("nEXRecHits_eq1_nrh_eq6_proj_y",";projected segment y-position [cm]",100,-80,80);

    plotter.book1D("nrh_eq6_nEXRecHits",";# of non-segment rechits",100,-0.5,99.5);

    plotter.book1D("nrh_eq6_nExH_eq0_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_eq0_projError_y",";projected segment y-position error [cm]",100,0,20);
    plotter.book1D("nrh_eq6_nExH_eq1_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_eq1_projError_y",";projected segment y-position error [cm]",100,0,20);


    plotter.book1D("chi2",";segment chi^2",100,0,100);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2",";segment chi^2",100,0,100);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob",";segment chi^2 probability",100,0,1);


    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_lt0p01_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_lt0p01_projError_y",";projected segment y-position error [cm]",100,0,20);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_y",";projected segment y-position error [cm]",100,0,20);

    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_x_0",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_y_0",";projected segment y-position error [cm]",100,0,20);
    plotter.book2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_SegmentMap"    ,"SegmentMap    ;x[cm];y[cm]",60,-30,30,90,-90,90);
    plotter.book2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_SegmentMap"    ,"SegmentMap    ;x[cm];y[cm]",60,-30,30,90,-90,90);
    plotter.book2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp"    ,";wireGrp;layer",50,-0.5,49.5,6,0.5,6.5);
    plotter.book2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp"    ,";wireGrp;layer",50,-0.5,49.5,6,0.5,6.5);


    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp_veto_projError_x",";projected segment x-position error [cm]",100,0,5);
    plotter.book1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp_veto_projError_y",";projected segment y-position error [cm]",100,0,20);


  }

  virtual void runAEvent() {
      plotter.get1D("nSegments")->Fill(segmentInfo.segment_id->size());

      if(segmentInfo.segment_id->size() != 1) return;
      plotter.get1D("nRecHitsInSeg")->Fill(segmentInfo.segment_nHits->at(0));
      plotter.get1D("nEXRecHits")->Fill(recHitInfo.rh_id->size() - segmentInfo.segment_nHits->at(0));
      plotter.get1D("chi2")->Fill(segmentInfo.segment_chisq->at(0));

      const double projZ = 34.3123;
      double projx,projy,projx_error,projy_error;
      projSement(0,projZ,projx,projy,projx_error,projy_error);
      projx -= segmentInfo.segment_pos_x->at(0);
      projy -= segmentInfo.segment_pos_y->at(0);

      plotter.get1D("projError_x")->Fill(projx_error);
      plotter.get1D("projError_y")->Fill(projy_error);

//      plotter.get1D("proj_x")->Fill(projx);
//      plotter.get1D("proj_y")->Fill(projy);

      TString ex = TString::Format("nrh_eq%u",size(segmentInfo.segment_nHits->at(0)));

      plotter.get1D(TString::Format("%s_projError_x",ex.Data()))->Fill(projx_error);
      plotter.get1D(TString::Format("%s_projError_y",ex.Data()))->Fill(projy_error);
      plotter.get1D(TString::Format("%s_proj_x",ex.Data()))->Fill(projx);
      plotter.get1D(TString::Format("%s_proj_y",ex.Data()))->Fill(projy);

      if(segmentInfo.segment_nHits->at(0) != 6) return;
      plotter.get1D(TString::Format("%s_nEXRecHits",ex.Data()))->Fill(recHitInfo.rh_id->size() - segmentInfo.segment_nHits->at(0));
      unsigned int nExHits = recHitInfo.rh_id->size() - segmentInfo.segment_nHits->at(0);
      if(nExHits > 1) return;
      TString ex2 = TString::Format("%s_nExH_eq%u",ex.Data(),nExHits);
      plotter.get1D(TString::Format("%s_projError_x",ex2.Data()))->Fill(projx_error);
      plotter.get1D(TString::Format("%s_projError_y",ex2.Data()))->Fill(projy_error);

      plotter.get1D("nrh_eq6_nExH_leq1_chi2")->Fill(segmentInfo.segment_chisq->at(0));
      double chisqProb = TMath::Prob(segmentInfo.segment_chisq->at(0),2*6-4);
      plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob")->Fill(chisqProb);

      if(chisqProb < 0.01){
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_lt0p01_projError_x")->Fill(projx_error);
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_lt0p01_projError_y")->Fill(projy_error);
      }
      else{
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_x")->Fill(projx_error);
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_y")->Fill(projy_error);
        double projx2,projy2,projx_error2,projy_error2;
        projSement(0,0,projx2,projy2,projx_error2,projy_error2);
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_x_0")->Fill(projx_error2);
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_y_0")->Fill(projy_error2);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_SegmentMap")->Fill(segmentInfo.segment_pos_x->at(0),segmentInfo.segment_pos_y->at(0));
        if(projy_error > 1) plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_SegmentMap")->Fill(segmentInfo.segment_pos_x->at(0),segmentInfo.segment_pos_y->at(0));
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_1->at(0)),1);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_2->at(0)),2);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_3->at(0)),3);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_4->at(0)),4);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_5->at(0)),5);
        plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_6->at(0)),6);
        if(projy_error > 1){
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_1->at(0)),1);
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_2->at(0)),2);
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_3->at(0)),3);
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_4->at(0)),4);
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_5->at(0)),5);
          plotter.get2D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_projError_geq1p0_wireGrp")->Fill(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_6->at(0)),6);
        }
      }

      if(chisqProb < 0.01) return;
      bool vetoGrp = false;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_1->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_1->at(0)) == 48) vetoGrp = true;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_2->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_2->at(0)) == 48) vetoGrp = true;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_3->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_3->at(0)) == 48) vetoGrp = true;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_4->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_4->at(0)) == 48) vetoGrp = true;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_5->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_5->at(0)) == 48) vetoGrp = true;
      if(recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_6->at(0)) == 1 || recHitInfo.rh_wireGrp->at(segmentInfo.segment_recHitIdx_6->at(0)) == 48) vetoGrp = true;
      if(!vetoGrp){
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp_veto_projError_x")->Fill(projx_error);
        plotter.get1D("nrh_eq6_nExH_leq1_chi2Prob_geq0p01_wireGrp_veto_projError_y")->Fill(projy_error);
      }



  }

  void write(TString fileName){ plotter.write(fileName);}
  HistGetter plotter;
};

#endif

void GetCSCPurityCuts(std::string fileName,std::string outFileName = "plots.root"){
  Analyzer a(fileName,"CSCDigiTree");
  a.analyze();
  a.write(outFileName);
}
