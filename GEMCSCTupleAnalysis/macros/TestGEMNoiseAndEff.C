
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
  Analyze(std::string cscFileName, std::string gemFileName,const GEMConfigInfo* info) : AnalyzeBoth(cscFileName,gemFileName,info)
  {

    plotter.book2D("noisemap_denom",";strip number;#eta partition"  ,geo.getNStrips(), -0.5, geo.getNStrips() -0.5,geo.getNRows(), -0.5, geo.getNRows() -.5);
    plotter.book2D("noisemap_num"  ,";strip number;#eta partition"  ,geo.getNStrips(), -0.5, geo.getNStrips() -0.5,geo.getNRows(), -0.5, geo.getNRows() -.5);

    for(unsigned int iR = 0;iR <geo.getNRows(); ++iR ){
      TString name = TString::Format("noiseClusterCount_eta_%u",iR);
      plotter.book1D(name  ,";# of noise clusters per event;a.u."  ,15,-0.5,14.5);
    }


    plotter.book2D("effmap_denom",";vFat column;#eta partition"  ,3, -0.5, 2.5,geo.getNRows(), -0.5, geo.getNRows() -.5);
    plotter.book2D("effmap_num"  ,";vFat column;#eta partition"  ,3, -0.5, 2.5,geo.getNRows(), -0.5, geo.getNRows() -.5);


    plotter.book1D("effmap_denom_int",";#eta partition"  ,geo.getNRows(), -0.5, geo.getNRows() -.5);
    plotter.book1D("effmap_num_int"  ,";#eta partition"  ,geo.getNRows(), -0.5, geo.getNRows() -.5);

    plotter.book2D("occmap"  ,";strip number;#eta partition"  ,geo.getNStrips(), -0.5, geo.getNStrips() -0.5,geo.getNRows(), -0.5, geo.getNRows() -.5);

     }
  virtual  ~Analyze() {};

  void write(TString outFileName){ plotter.write(outFileName);}


  void fillEffMaps(const ErrorPoint2D& cscSeg  ){

    const double stretch = 0.9433;

    auto translateRowto0isNarrowFrom0isWide = [] (int row) -> int {
      row -= 3;
      row *= -1;
      row += 4;
      return row;
    };

    auto translateStripFrom383isLeftTo1isLeft = [] (int strip) -> int {
      strip -= 192;
      strip *= -1;
      strip += 192;
      return strip;
    };

    for(unsigned int iC = 0; iC < gem.gemInfo.clusters.size(); ++iC){
      for(int iStrip = gem.gemInfo.clusters[iC].getFirstStrip(); iStrip >= gem.gemInfo.clusters[iC].getLastStrip(); --iStrip){
        int plotStrip =  translateStripFrom383isLeftTo1isLeft(iStrip) -1;
        plotter.get2D("occmap")->Fill(plotStrip,gem.gemInfo.clusters[iC].nRow);
      }
    }


    int hitRow  = geo.findEtaPartition(cscSeg.y());
    int plotHitRow = translateRowto0isNarrowFrom0isWide(hitRow);
    double maxSigY = 1.6;
    double maxCMX = 2;

//    cout << "---------"<<endl;
//    cout << cscSeg.y() <<" "<<cscSeg.x() << " "<< hitRow <<" " << geo.getRowTop(hitRow)  <<" "<< geo.getRowBottom(hitRow)  << endl;

    //Make sure it is centered
    if(cscSeg.y() + maxSigY*cscSeg.error_y() > geo.getRowTop(hitRow) ) return;
    if(cscSeg.y() - maxSigY*cscSeg.error_y() <= geo.getRowBottom(hitRow) ) return;

    auto getStripCenterAtBottom = [&] (int row, int strip) -> double {
      float fStrip = static_cast<float>(strip) - 0.5;
      float nStrips = geo.getNStrips();
      return stretch*geo.getRowBottomEdge(row)*(fStrip/nStrips -0.5);
    };

    auto findColumn = [&] (int row, double xValue) -> int {
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,1) + maxCMX  ) return -1;
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,128) - maxCMX  ) return 0;
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,129) + maxCMX  ) return -1;
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,256) - maxCMX  ) return 1;
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,257) + maxCMX  ) return -1;
      if(cscSeg.x() <= getStripCenterAtBottom(hitRow,384) - maxCMX  ) return 2;
      return -1;
    };


    if(cscSeg.x() > getStripCenterAtBottom(hitRow,1) + 3 && cscSeg.x() <= getStripCenterAtBottom(hitRow,384) - 3    ){
      plotter.get1D("effmap_denom_int")->Fill(plotHitRow);
      bool found = false;
      for(unsigned int iC = 0; iC < gem.gemInfo.clusters.size(); ++iC){
        if(gem.gemInfo.clusters[iC].nRow != plotHitRow  ) {continue;}
        found = true;
        break;
      }
      if(found) plotter.get1D("effmap_num_int")->Fill(plotHitRow);
    }


    int columnNumber = findColumn(hitRow,cscSeg.x());
//    cout <<getStripCenterAtBottom(hitRow,1) <<" "<< getStripCenterAtBottom(hitRow,128)<<" "<< getStripCenterAtBottom(hitRow,256)<<" " <<columnNumber <<endl;
    if(columnNumber < 0) return;



    auto isInColumn = [] (int column, int strip) -> bool {
      if(column == 0){
        if(strip <= 128 ) return true;
        else return false;
      } else if(column == 1){
        if(strip <= 128 ) return false;
        if(strip <= 256 ) return true;
        else return false;
      } else if(column == 2){
        if(strip <= 256 ) return false;
        if(strip <= 384 ) return true;
        else return false;
      }
      return false;
    };




    plotter.get2D("effmap_denom")->Fill(columnNumber,plotHitRow);

    bool found = false;
    for(unsigned int iC = 0; iC < gem.gemInfo.clusters.size(); ++iC){
      if(gem.gemInfo.clusters[iC].nRow != plotHitRow  ) {continue;}

      for(int iStrip = gem.gemInfo.clusters[iC].getFirstStrip(); iStrip >= gem.gemInfo.clusters[iC].getLastStrip(); --iStrip){
        int transStrip =  translateStripFrom383isLeftTo1isLeft(iStrip);
        if(isInColumn(columnNumber,transStrip)){found = true; break;}
      }
    }
    if(found)plotter.get2D("effmap_num")->Fill(columnNumber,plotHitRow);




  }


  void fillNoiseMaps(const ErrorPoint2D& cscSeg  ){

    auto translateRowto0isNarrowFrom0isWide = [] (int row) -> int {
      row -= 3;
      row *= -1;
      row += 4;
      return row;
    };

    auto translateStripFrom383isLeftTo1isLeft = [] (int strip) -> int {
      strip -= 192;
      strip *= -1;
      strip += 192;
      return strip;
    };

    std::vector<int> noiseCounts(geo.getNRows(),-1);

    int hitRow  = geo.findEtaPartition(cscSeg.y());
    int plotHitRow = translateRowto0isNarrowFrom0isWide(hitRow);
    //fill in empties
    for(int iR = 0; iR < geo.getNRows(); ++iR ){
      if(TMath::Abs(iR - plotHitRow) <= 1  ) continue;
      noiseCounts[iR]++;
      for(int iStrip = 0; iStrip < geo.getNStrips(); ++iStrip){
        plotter.get2D("noisemap_denom")->Fill(iStrip,iR);
      }
    }

    //Now fill in hits
    for(unsigned int iC = 0; iC < gem.gemInfo.clusters.size(); ++iC){
      if(TMath::Abs(gem.gemInfo.clusters[iC].nRow - plotHitRow) <= 1  ) {continue;}
      noiseCounts[gem.gemInfo.clusters[iC].nRow]++;
      for(int iStrip = gem.gemInfo.clusters[iC].getFirstStrip(); iStrip >= gem.gemInfo.clusters[iC].getLastStrip(); --iStrip){
        int plotStrip =  translateStripFrom383isLeftTo1isLeft(iStrip) -1;
        plotter.get2D("noisemap_num")->Fill(plotStrip,gem.gemInfo.clusters[iC].nRow);
      }
    }

    for(unsigned int iR = 0;iR <geo.getNRows(); ++iR ){
      if(noiseCounts[iR] < 0) continue;
      TString name = TString::Format("noiseClusterCount_eta_%u",iR);
      plotter.get1D(name)->Fill(noiseCounts[iR]);

    }
  }

  virtual void runAEvent() {
    if(!pureSample(csc)) return;
    Segment seg;
    seg.setCoords(csc.segmentInfo.segment_pos_x->at(0),csc.segmentInfo.segment_pos_y->at(0),csc.segmentInfo.segment_dxdz->at(0),csc.segmentInfo.segment_dydz->at(0));
    seg.setCov(csc.segmentInfo.segment_cov_x->at(0),csc.segmentInfo.segment_cov_x_y->at(0),csc.segmentInfo.segment_cov_dxdz_x->at(0),csc.segmentInfo.segment_cov_dydz_x->at(0),
        csc.segmentInfo.segment_cov_y->at(0),csc.segmentInfo.segment_cov_dxdz_y->at(0),csc.segmentInfo.segment_cov_dydz_y->at(0),
        csc.segmentInfo.segment_cov_dxdz->at(0),csc.segmentInfo.segment_cov_dxdz_dydz->at(0),csc.segmentInfo.segment_cov_dydz->at(0));
    auto cscSeg = seg.project(-0.66767,1.1331,33.2863,-0.0274707);


    fillNoiseMaps(cscSeg);
    fillEffMaps(cscSeg);

  }

  HistGetter plotter;
  GEMGeometry geo;
};

#endif

void TestGEMNoiseAndEff(std::string cscfileName="csc_forsync.root",std::string gemfilename = "gem_forsync.root",std::string outFileName = "plotCSCAndGEM_out.root"){
  GEMConfigInfo info;
  info.geoName               = "gemGeo.txt";
  info.vFATChanMapName       = "slot_table_904_june09.csv";
//  info.vFATChanMapName       = "slot_table.csv";
  Analyze a(cscfileName,gemfilename,&info);
  a.analyze();
  a.write(outFileName);
//  a.draw();
}
