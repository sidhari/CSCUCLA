
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "../include/BaseCSCAndGEMAnalyzer.h"
#include "include/GEMPlottingInfo.h"
#include "../include/GEMGeometry.h"
#include "../include/Segment.h"

#include<iostream>

using namespace std;
using namespace CSCGEMTuples;

class Analyze : public AnalyzeBoth {
public:
  int nEvt = 0;
  Analyze(std::string cscFileName, std::string gemFileName,const GEMConfigInfo* info) : AnalyzeBoth(cscFileName,gemFileName,info)
  {
    plotter.book2D("noisemap_denom",";strip number;#eta partition"  ,geo.getNStrips(), -0.5, geo.getNStrips() -0.5,geo.getNRows(), -0.5, geo.getNRows() -.5);
    plotter.book2D("noisemap_byz",";strip number;#eta partition"  ,500, -25, 25,geo.getNRows(), -0.5, geo.getNRows() -.5);

    plotter.book2D("old_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("new_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("move_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("rot_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("tiltx_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("tilty_proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);
    plotter.book2D("proj_SegmentMap"            ,";x[cm];y[cm]"     ,90,-90,90,90,-90,90);

    plotter.book1D("new_proj_x"            ,";x [cm]"     ,100,-200,200);
    plotter.book1D("new_proj_y"            ,";y [cm]"     ,100,-200,200);
    plotter.book1D("new_proj_xError"            ,";error [cm]"     ,500,0,10);
    plotter.book1D("new_proj_yError"            ,";error [cm]"     ,500,0,10);
    plotter.book1D("rot_proj_xError"            ,";error [cm]"     ,500,0,10);
    plotter.book1D("rot_proj_yError"            ,";error [cm]"     ,500,0,10);

    plotter.book1D("clus_x"            ,";X [cm]"     ,100,-90,90);
    plotter.book1D("clus_xError"            ,";error [cm]"     ,500,0,10);
    plotter.book1D("clus_yError"            ,";error [cm]"     ,500,0,10);

    plotter.book1D("proj_xError"            ,";error [cm]"     ,500,0,10);
    plotter.book1D("proj_yError"            ,";error [cm]"     ,500,0,10);

    plotter.book1D("delta_xError"            ,";error [cm]"     ,500,-5,5);
    plotter.book1D("delta_yError"            ,";error [cm]"     ,500,-20,20);

    plotter.book1D("deltaX"            ,";#Delta X [cm]"     ,500,-90,90);
    plotter.book1D("deltaY"            ,";#Delta Y [cm]"     ,500,-90,90);
    plotter.book1D("deltaR"            ,";#Delta R [cm]"     ,500,-90,90);

    plotter.book1D("deltaXoSig"            ,";#Delta X / #sigma"     ,500,-10,10);
    plotter.book1D("deltaYoSig"            ,";#Delta Y / #sigma"     ,500,-10,10);
    plotter.book1D("deltaRoSig"            ,";#Delta R / #sigma"     ,500,-10,10);

    for(unsigned int iV =  0 ; iV < 24; ++iV){
      TString name = TString::Format("deltaXoSig_%u",iV);
      plotter.book1D(name            ,";#Delta X / #sigma"     ,1000,-10,10);
    }

    for(unsigned int iE =  0 ; iE < 8; ++iE){
      TString name1 = TString::Format("deltaYoSig_eta_%u",iE);
      TString name2 = TString::Format("deltaY_eta_%u",iE);
      plotter.book1D(name1              ,";(y_{g} - y_{p})/#sigma(y_{g} - y_{p});a.u."     ,250,-5,5);
      plotter.book1D(name2              ,";y_{g} - y_{p} [cm];a.u."     ,100,-30,30);
    }

    if (true){
      double xBins[] = {-22,-16.5,-11,-5.5,0,5.5,11,16.5,22};
      for(unsigned int iX =  0 ; iX < 8; ++iX){

        TString name1 = TString::Format("deltaXoSig_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name2 = TString::Format("deltaX_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name3 = TString::Format("deltaXoSig_const_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name4 = TString::Format("deltaX_const_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);

        TString name5 = TString::Format("deltaXoSig_const_oneStrip_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name6 = TString::Format("deltaX_const_oneStrip_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);

        plotter.book1D(name1              ,";(x_{g} - x_{p})/#sigma(x_{g} - x_{p});a.u."     ,100,-15,15);
        plotter.book1D(name2              ,";x_{g} - x_{p} [cm];a.u."     ,60,-5,5);
        plotter.book1D(name3              ,";(x_{g} - x_{p})/#sigma(x_{g} - x_{p});a.u."     ,100,-15,15);
        plotter.book1D(name4              ,";x_{g} - x_{p} [cm];a.u."     ,60,-5,5);
        plotter.book1D(name5              ,";(x_{g} - x_{p})/#sigma(x_{g} - x_{p});a.u."     ,100,-15,15);
        plotter.book1D(name6              ,";x_{g} - x_{p} [cm];a.u."     ,60,-5,5);
      }
    }

        plotter.book2D("proj_x_r0_1"            ,";x [cm];y [cm]"     ,90,-90,90,90,-90,90);
        plotter.book2D("proj_x_r0_384"            ,";x [cm];y [cm]"     ,90,-90,90,90,-90,90);
        plotter.book2D("proj_x_r3_20"            ,";x [cm];y [cm]"     ,90,-90,90,90,-90,90);






//    for(unsigned int iE =  0 ; iE < 8; ++iE){
//      for(unsigned int iX =  0 ; iX < 8; ++iX){
//        TString name = TString::Format("deltaXoSig_%u_%u",iE,iX);
//        plotter.book1D(name            ,";#Delta X / #sigma"     ,1000,-20,20);
//      }
//    }
//
//    for(unsigned int iE =  0 ; iE < 6; ++iE){
//      for(unsigned int iX =  0 ; iX < 8; ++iX){
//        TString name = TString::Format("deltaXoSig_a_%u_x_%u",iE,iX);
//        plotter.book1D(name            ,";#Delta X / #sigma"     ,1000,-20,20);
//      }
//    }


    for(unsigned int iC = 1; iC <= 384;iC += 10){
      cout <<geo.getStripCenter(0,iC).x() <<" ";
    }
    cout << endl;
    for(unsigned int iC = 1; iC <= 384; iC+=10){
      cout <<geo.getClusterInfo(0,iC,5).error_y() <<" ";
    }
  }
  virtual  ~Analyze() {};

  void write(TString outFileName){cout << nEvt << endl; plotter.write(outFileName);}

  virtual void runAEvent() {
    if(!pureSample(csc)) return;
    nEvt++;

    if(!gem.gemInfo.vFats.size()) return;
//    if(TMath::Abs(csc.segmentInfo.segment_cov_dxdz->at(0)) > 0.01) return;
    double sX = csc.segmentInfo.segment_pos_x->at(0);
    double sY = csc.segmentInfo.segment_pos_y->at(0);
    double spX,spY,projxe,projye;
    csc.projSement(0,gem.gemGeo.ZDIST,spX,spY,projxe,projye);
    plotter.get2D("old_proj_SegmentMap")->Fill(spX,spY);

    Segment seg;
    seg.setCoords(csc.segmentInfo.segment_pos_x->at(0),csc.segmentInfo.segment_pos_y->at(0),csc.segmentInfo.segment_dxdz->at(0),csc.segmentInfo.segment_dydz->at(0));
    seg.setCov(csc.segmentInfo.segment_cov_x->at(0),csc.segmentInfo.segment_cov_x_y->at(0),csc.segmentInfo.segment_cov_dxdz_x->at(0),csc.segmentInfo.segment_cov_dydz_x->at(0),
        csc.segmentInfo.segment_cov_y->at(0),csc.segmentInfo.segment_cov_dxdz_y->at(0),csc.segmentInfo.segment_cov_dydz_y->at(0),
        csc.segmentInfo.segment_cov_dxdz->at(0),csc.segmentInfo.segment_cov_dxdz_dydz->at(0),csc.segmentInfo.segment_cov_dydz->at(0));
    auto p = seg.project(0,0,gem.gemGeo.ZDIST,0);
    plotter.get1D("new_proj_x")->Fill(p.x());
    plotter.get1D("new_proj_y")->Fill(p.y());
    plotter.get1D("new_proj_xError")->Fill(p.error_x());
    plotter.get1D("new_proj_yError")->Fill(p.error_y());
    plotter.get2D("new_proj_SegmentMap")->Fill(p.x(),p.y());
    p = seg.project(10,10,gem.gemGeo.ZDIST,0);


    plotter.get2D("move_proj_SegmentMap")->Fill(p.x(),p.y());
    p = seg.project(0,0,gem.gemGeo.ZDIST,3.14/4.0);
    plotter.get2D("rot_proj_SegmentMap")->Fill(p.x(),p.y());
    plotter.get1D("rot_proj_xError")->Fill(p.error_x());
    plotter.get1D("rot_proj_yError")->Fill(p.error_y());


    p = seg.project2(0,0,gem.gemGeo.ZDIST,3.14/4.0,0,0);
    plotter.get2D("tiltx_proj_SegmentMap")->Fill(p.x(),p.y());

    p = seg.project2(0,0,gem.gemGeo.ZDIST,0,3.14/4.0,0);
    plotter.get2D("tilty_proj_SegmentMap")->Fill(p.x(),p.y());



    if(gem.gemInfo.clusters.size() != 1) return;

    auto translateRow = [] (int row) -> int {
      row -= 3;
      row *= -1;
      row += 4;
      return row;
    };

    auto translateStrip = [] (int strip) -> int {
      strip -= 192;
      strip *= -1;
      strip += 192;
      return strip;
    };


    int transRow = translateRow(gem.gemInfo.clusters[0].nRow);
    int transStrip = translateStrip(gem.gemInfo.clusters[0].getFirstStrip());
//    if(transRow == 3 && transStrip <= 129) return;

//    if(gem.gemInfo.vFats[0].idx != 0 && gem.gemInfo.vFats[0].idx != 8 && gem.gemInfo.vFats[0].idx != 16) return;
//    if(gem.gemInfo.clusters[0].nStrips != 2) return;
    auto gemClus = geo.getClusterInfo(transRow,transStrip,gem.gemInfo.clusters[0].getNStrips());
    const double clusX = gemClus.x();
    const double clusY = gemClus.y();

//    cout <<transRow <<" "<< gemClus.x() <<" "<<" "<<transStrip <<" "<< gem.gemInfo.clusters[0].getNStrips()<<" " << gemClus.y()<<  endl;

    plotter.get1D("clus_x")->Fill(gemClus.x());
    plotter.get1D("clus_xError")->Fill(gemClus.error_x());
    plotter.get1D("clus_yError")->Fill(gemClus.error_y());
//    auto cscSeg = seg.project(1.6058,0.995368,51.6499,0.0534664); //old config
//    auto cscSeg = seg.project(-1.00345,1.84613,34.1826,-0.0281307); //std
//    auto cscSeg = seg.project(-0.954373,1.86423,34.2593,-0.0265418);//stretch
//    auto cscSeg = seg.project(-0.801626,1.198,33.3594,-0.0292852);
//    -0.607096,1.16343,33.4283,-0.0285756
    auto cscSeg = seg.project(-0.66767,1.1331,33.2863,-0.0274707);
//    auto cscSeg = seg.project2(-0.807667,1.89984,34.1826,.387,0,-0.0251087); //22 degree



    double stretch = 0.9433;
    gemClus.x() *= stretch;
    gemClus.cov_xx() *= stretch*stretch;
    gemClus.cov_xy() *= stretch;

    double origX = gemClus.x();

//    auto cscSeg = seg.project2(-1.00345,1.84613,34.1826,.35,0,-0.0281838);
    plotter.get2D("proj_SegmentMap")->Fill(cscSeg.x(),cscSeg.y());

    plotter.get1D("proj_xError")->Fill(cscSeg.error_x());
    plotter.get1D("proj_yError")->Fill(cscSeg.error_y());

    gemClus -= cscSeg;

//    if(TMath::Abs(gemClus.x()) > 5) cout << transRow <<" "<< transStrip <<endl;

    if(TMath::Abs(gemClus.x()) > 4) plotter.get2D("noisemap_denom")->Fill(transStrip-1,transRow);
    if(TMath::Abs(gemClus.x()) > 4) plotter.get2D("noisemap_byz")->Fill(origX,transRow);


//    if(transRow == 3  && transStrip <= 20) return;
//    if(transStrip >= 65 && transStrip <= 80) return;
//    if(transStrip >= 112 && transStrip <= 128) return;

//    cout << cscSeg.x()<<" "<< gemClus.x() << endl;

    plotter.get1D("delta_xError")->Fill(gemClus.error_x());
    plotter.get1D("delta_yError")->Fill(gemClus.error_y());

    plotter.get1D("deltaX")->Fill(gemClus.x());
    plotter.get1D("deltaY")->Fill(gemClus.y());
    plotter.get1D("deltaR")->Fill(TMath::Sqrt(gemClus.y()*gemClus.y() + gemClus.x()*gemClus.y()));

    plotter.get1D("deltaXoSig")->Fill(gemClus.x()/gemClus.error_x());
    plotter.get1D("deltaYoSig")->Fill(gemClus.y()/gemClus.error_y());

    if( true) {
      unsigned int iE = transRow;
      TString name1 = TString::Format("deltaYoSig_eta_%u",iE);
      TString name2 = TString::Format("deltaY_eta_%u",iE);
      plotter.get1D(name2)->Fill(gemClus.y());
      plotter.get1D(name1)->Fill(gemClus.y()/gemClus.error_y());
    }



    if (true){
      double xBins[] = {-22,-16.5,-11,-5.5,0,5.5,11,16.5,22};
      for(unsigned int iX =  0 ; iX < 8; ++iX){
        if(clusX >= xBins[iX+1] ) continue;
        TString name1 = TString::Format("deltaXoSig_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name2 = TString::Format("deltaX_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name3 = TString::Format("deltaXoSig_const_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name4 = TString::Format("deltaX_const_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name5 = TString::Format("deltaXoSig_const_oneStrip_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);
        TString name6 = TString::Format("deltaX_const_oneStrip_x_%.0f_%.0f",xBins[iX]*10,xBins[iX+1]*10);

        plotter.get1D(name1)->Fill(gemClus.x()/gemClus.error_x());
        plotter.get1D(name2)->Fill(gemClus.x());

            if(TMath::Abs(csc.segmentInfo.segment_dxdz->at(0)) < 0.008777){
              plotter.get1D(name3)->Fill(gemClus.x()/gemClus.error_x());
              plotter.get1D(name4)->Fill(gemClus.x());
              if(gem.gemInfo.clusters[0].getNStrips() == 1){
                plotter.get1D(name5)->Fill(gemClus.x()/gemClus.error_x());
                plotter.get1D(name6)->Fill(gemClus.x());
              }
            }


        break;

      }
    }




      TString name = TString::Format("deltaXoSig_%u",gem.gemInfo.vFats[0].idx);
      plotter.get1D(name)->Fill(gemClus.x()/gemClus.error_x());



      if(transRow==0){
        if(transStrip==1) plotter.get2D("proj_x_r0_1")->Fill(cscSeg.x(),cscSeg.y());
        if(transStrip==384) plotter.get2D("proj_x_r0_384")->Fill(cscSeg.x(),cscSeg.y());
      }
      if(transRow==3){
        if(transStrip==20) plotter.get2D("proj_x_r3_20")->Fill(cscSeg.x(),cscSeg.y());
      }





//      if(true){
//        unsigned int xBin = 0;
////        if(cscSeg.x() < -40.0 +1*40/6)
//
//        if(cscSeg.x() < -20.0 + 1. * (40./8.) ) xBin = 0;
//        else if(cscSeg.x() < -20.0 + 2. * (40./8.) ) xBin = 1;
//        else if(cscSeg.x() < -20.0 + 3. * (40./8.) ) xBin = 2;
//        else if(cscSeg.x() < -20.0 + 4. * (40./8.) ) xBin = 3;
//        else if(cscSeg.x() < -20.0 + 5. * (40./8.) ) xBin = 4;
//        else if(cscSeg.x() < -20.0 + 6. * (40./8.) ) xBin = 5;
//        else if(cscSeg.x() < -20.0 + 7. * (40./8.) ) xBin = 6;
//        else xBin = 7;
//
//        TString name = TString::Format("deltaXoSig_%u_%u",transRow,xBin);
//        plotter.get1D(name)->Fill(gemClus.x());
//
//        unsigned int xBin2 = 0;
//        if(csc.segmentInfo.segment_dxdz->at(0)      < -.5 + 1. * (1./6.) ) xBin2 = 0;
//        else if(csc.segmentInfo.segment_dxdz->at(0) < -.5 + 2. * (1./6.) ) xBin2 = 1;
//        else if(csc.segmentInfo.segment_dxdz->at(0) < -.5 + 3. * (1./6.) ) xBin2 = 2;
//        else if(csc.segmentInfo.segment_dxdz->at(0) < -.5 + 4. * (1./6.) ) xBin2 = 3;
//        else if(csc.segmentInfo.segment_dxdz->at(0) < -.5 + 5. * (1./6.) ) xBin2 = 4;
//        else xBin2 = 5;
//
//        name = TString::Format("deltaXoSig_a_%u_x_%u",xBin2,xBin);
//        plotter.get1D(name)->Fill(gemClus.x());
//      }
//
//
//      if(transRow==0){
//        if(transStrip==1) plotter.get1D("proj_x_r0_1")->Fill(cscSeg.x());
//        if(transStrip==384) plotter.get1D("proj_x_r0_384")->Fill(cscSeg.x());
//      }
//      if(transRow==1){
//        if(transStrip==1) plotter.get1D("proj_x_r1_1")->Fill(cscSeg.x());
//        if(transStrip==384) plotter.get1D("proj_x_r1_384")->Fill(cscSeg.x());
//      }
//      if(transRow==2){
//        if(transStrip==1) plotter.get1D("proj_x_r2_1")->Fill(cscSeg.x());
//        if(transStrip==384) plotter.get1D("proj_x_r2_384")->Fill(cscSeg.x());
//      }



//      for(unsigned int iE =  0 ; iE < 6; ++iE){
//        for(unsigned int iX =  0 ; iX < 8; ++iX){
//          TString name = TString::Format("deltaXoSig_a_%u_x_%u",iE,iX);
//          plotter.book1D(name            ,";#Delta X / #sigma"     ,1000,-20,20);
//        }
//      }


//
//    plotter.book1D("deltaX"            ,";#Delta X [cm]"     ,500,-10,10);
//    plotter.book1D("deltaY"            ,";#Delta Y [cm]"     ,500,-10,10);
//    plotter.book1D("deltaR"            ,";#Delta R [cm]"     ,500,-10,10);
//
//    plotter.book1D("deltaXoSig"            ,";#Delta X / #sigma"     ,500,-5,5);
//    plotter.book1D("deltaYoSig"            ,";#Delta Y / #sigma"     ,500,-5,5);
//    plotter.book1D("deltaRoSig"            ,";#Delta R / #sigma"     ,500,-5,5);





  }

  HistGetter plotter;
  GEMGeometry geo;
};

#endif

void TestGEMGeo(std::string cscfileName="csc_forsync.root",std::string gemfilename = "gem_forsync.root",std::string outFileName = "plotCSCAndGEM_out.root"){
  GEMConfigInfo info;
  info.geoName               = "gemGeo.txt";
  info.vFATChanMapName       = "slot_table_904_june09.csv";
//  info.vFATChanMapName       = "slot_table.csv";
  Analyze a(cscfileName,gemfilename,&info);
  a.analyze();

  a.write(outFileName);
//  a.draw();
}
