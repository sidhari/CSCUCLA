
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "../include/BaseCSCAndGEMAnalyzer.h"
#include "include/GEMPlottingInfo.h"
#include "../include/GEMGeometry.h"
#include "../include/Segment.h"

#include<iostream>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom2.h"
#include "TError.h"

using namespace std;
using namespace CSCGEMTuples;

int nPairs = 0;
int nMaxPairs =5000;
bool first = false;
double setZ = 50;
Segment * segments;
ErrorPoint2D * clusters;

double totalDist(const double *xx ){
  double dist = 0;
  for(unsigned int iP = 0; iP < nPairs; ++iP){
    ErrorPoint2D newClus = clusters[iP];
//    double stretch = 0.9433;
//    newClus.x() *= stretch;
//    newClus.cov_xx() *= stretch*stretch;
//    newClus.cov_xy() *= stretch;

    auto p = segments[iP].project(xx[0],xx[1],xx[2],xx[3]);
    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
    if(first) cout << clusters[iP].x() <<" "<<clusters[iP].y()<<" "<< clusters[iP].error_x() <<" "<<clusters[iP].error_y() <<" -> ";
//    p -= clusters[iP];
    p -= newClus;
    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
//    dist += p.x()*p.x()/p.cov_xx() + p.y()*p.y()/p.cov_yy() ;
    dist += p.x()*p.x() + p.y()*p.y() ;
    if(first) cout <<  p.x()*p.x()/p.cov_xx() <<" "<<p.y()*p.y()/p.cov_yy()<<" "<< dist <<endl;
  }
//  cout << xx[0]<<" "<<xx[1]<<" "<<xx[2]<<" "<< xx[3] << " " << dist<< endl;
  first = false;
  return dist;
}


double totalDist2(const double *xx ){
  double dist = 0;
  for(unsigned int iP = 0; iP < nPairs; ++iP){
    auto p = segments[iP].project2(xx[0],xx[1],xx[2],xx[3],xx[4],xx[5]);
    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
    if(first) cout << clusters[iP].x() <<" "<<clusters[iP].y()<<" "<< clusters[iP].error_x() <<" "<<clusters[iP].error_y() <<" -> ";
    p -= clusters[iP];
    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
//    dist += p.x()*p.x()/p.cov_xx() + p.y()*p.y()/p.cov_yy() ;
    dist += p.x()*p.x() + p.y()*p.y() ;
    if(first) cout <<  p.x()*p.x()/p.cov_xx() <<" "<<p.y()*p.y()/p.cov_yy()<<" "<< dist <<endl;
  }
//  cout << xx[0]<<" "<<xx[1]<<" "<<xx[2]<<" "<< xx[3] << " " << dist<< endl;
  first = false;
  return dist;
}




double totalDistWithoutZ(const double *xx ){
  double dist = 0;
  for(unsigned int iP = 0; iP < nPairs; ++iP){
    auto p = segments[iP].project(xx[0],xx[1],setZ,xx[2]);
    ErrorPoint2D newClus = clusters[iP];
    double stretch = 0.9433;
    newClus.x() *= stretch;
    newClus.cov_xx() *= stretch*stretch;
    newClus.cov_xy() *= stretch;

    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
    if(first) cout << clusters[iP].x() <<" "<<clusters[iP].y()<<" "<< clusters[iP].error_x() <<" "<<clusters[iP].error_y() <<" -> ";
    p -= newClus;
    if(first) cout << p.x() <<" "<<p.y()<<" "<< p.error_x() <<" "<<p.error_y() <<" -> ";
//    dist += p.x()*p.x()/p.cov_xx() + p.y()*p.y()/p.cov_yy() ;
//    dist += p.x()*p.x()/p.cov_xx() + p.y()*p.y()/p.cov_yy() ;
    dist += (p.x()*p.x() + p.y()*p.y() ) *(p.x()*p.x() + p.y()*p.y() ) /( p.x()*p.x()*p.cov_xx()  + p.y()*p.y()*p.cov_yy() + 2*p.x()*p.y()*p.cov_xy() )  ;
    if(first) cout <<  p.x()*p.x()/p.cov_xx() <<" "<<p.y()*p.y()/p.cov_yy()<<" "<< dist <<endl;
  }
//  cout << xx[0]<<" "<<xx[1]<<" "<<xx[2]<<" "<< xx[3] << " " << dist<< endl;
  first = false;
  return dist;
}



class Analyze : public AnalyzeBoth {
public:
  Analyze(std::string cscFileName, std::string gemFileName,const GEMConfigInfo* info) : AnalyzeBoth(cscFileName,gemFileName,info)
  {
    cout << nPairs <<" "<< nMaxPairs <<endl;
    segments= new Segment[nMaxPairs];
    clusters = new ErrorPoint2D[nMaxPairs];
  }
  virtual  ~Analyze() {delete[] segments; delete[] clusters;};

  virtual void runAEvent() {
    if(nPairs >= nMaxPairs) return;
    if(!pureSample(csc)) return;
    if(gem.gemInfo.clusters.size() != 1) return;
    Segment tempSeg;
    tempSeg.setCoords(csc.segmentInfo.segment_pos_x->at(0),csc.segmentInfo.segment_pos_y->at(0),csc.segmentInfo.segment_dxdz->at(0),csc.segmentInfo.segment_dydz->at(0));
    tempSeg.setCov(csc.segmentInfo.segment_cov_x->at(0),csc.segmentInfo.segment_cov_x_y->at(0),csc.segmentInfo.segment_cov_dxdz_x->at(0),csc.segmentInfo.segment_cov_dydz_x->at(0),
           csc.segmentInfo.segment_cov_y->at(0),csc.segmentInfo.segment_cov_dxdz_y->at(0),csc.segmentInfo.segment_cov_dydz_y->at(0),
           csc.segmentInfo.segment_cov_dxdz->at(0),csc.segmentInfo.segment_cov_dxdz_dydz->at(0),csc.segmentInfo.segment_cov_dydz->at(0));
    auto p = tempSeg.project(0,0,33,0);
    if(TMath::Abs(p.x()) > 25) return;
    if(TMath::Abs(p.y()) > 65) return;

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

    if(transRow == 3  && transStrip <= 20) return;
    if(transStrip >= 65 && transStrip <= 80) return;
    if(transStrip >= 112 && transStrip <= 128) return;

    clusters[nPairs] = geo.getClusterInfo(transRow,transStrip,gem.gemInfo.clusters[0].getNStrips());
    segments[nPairs].setCoords(csc.segmentInfo.segment_pos_x->at(0),csc.segmentInfo.segment_pos_y->at(0),csc.segmentInfo.segment_dxdz->at(0),csc.segmentInfo.segment_dydz->at(0));
    segments[nPairs].setCov(csc.segmentInfo.segment_cov_x->at(0),csc.segmentInfo.segment_cov_x_y->at(0),csc.segmentInfo.segment_cov_dxdz_x->at(0),csc.segmentInfo.segment_cov_dydz_x->at(0),
           csc.segmentInfo.segment_cov_y->at(0),csc.segmentInfo.segment_cov_dxdz_y->at(0),csc.segmentInfo.segment_cov_dydz_y->at(0),
           csc.segmentInfo.segment_cov_dxdz->at(0),csc.segmentInfo.segment_cov_dxdz_dydz->at(0),csc.segmentInfo.segment_cov_dydz->at(0));
    nPairs++;
  }

  void solve(TString outFileName){
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "MIGRAD");
    min->SetMaxFunctionCalls(1000000);
    min->SetTolerance(0.001);
    min->SetPrintLevel(10);

        ROOT::Math::Functor f(&totalDist,4);
    double step[4] = {0.01,0.01,.01,.01};
    double variable[4] = { 0,0,50,0};
    min->SetFunction(f);
    min->SetVariable(0,"x"  ,variable[0], step[0]);
    min->SetVariable(1,"y"  ,variable[1], step[1]);
    min->SetVariable(2,"z"  ,variable[2], step[2]);
    min->SetVariable(3,"phi",variable[3], step[3]);

       // do the minimization
       min->Minimize();

       const double *xs = min->X();
       std::cout << "Minimum: f(" << xs[0] << "," << xs[1]<< "," << xs[2]<< "," << xs[3] << "): "
                 << min->MinValue()  << std::endl;


           setZ=xs[2];

           ROOT::Math::Minimizer* min2 = ROOT::Math::Factory::CreateMinimizer("Minuit2", "MIGRAD");
           min2->SetMaxFunctionCalls(1000000);
           min2->SetTolerance(0.001);
           min2->SetPrintLevel(10);

               ROOT::Math::Functor f2(&totalDistWithoutZ,3);
           double step2[3] = {0.01,0.01,.01};
           double variable2[3] = { xs[0],xs[1],xs[3]};
           min2->SetFunction(f2);
           min2->SetVariable(0,"x"  ,variable2[0], step2[0]);
           min2->SetVariable(1,"y"  ,variable2[1], step2[1]);
           min2->SetVariable(2,"phi",variable2[2], step2[2]);

              // do the minimization
              min2->Minimize();

              const double *xs2 = min2->X();
              std::cout << "Minimum: f(" << xs2[0] << "," << xs2[1]<< "," << xs2[2] << "): "
                        << min2->MinValue()  << std::endl;



//    ROOT::Math::Functor f(&totalDist2,6);
//double step[6] = {0.01,0.01,.01,.01,.01,.01};
//double variable[6] = { 0,0,30,0,0,0};
//min->SetFunction(f);
//min->SetVariable(0,"x"   ,variable[0], step[0]);
//min->SetVariable(1,"y"   ,variable[1], step[1]);
//min->SetVariable(2,"z"   ,variable[2], step[2]);
//min->SetVariable(3,"angX",variable[3], step[3]);
//min->SetVariable(4,"angY",variable[4], step[4]);
//min->SetVariable(5,"phi" ,variable[5], step[5]);
//
//   // do the minimization
//   min->Minimize();
//
//   const double *xs = min->X();
//   std::cout << "Minimum: f(" << xs[0] << "," << xs[1]<< "," << xs[2]<< "," << xs[3]<< "," << xs[4]<< "," << xs[5] << "): "
//             << min->MinValue()  << std::endl;



//
//       setZ=xs[2];
//
//       ROOT::Math::Minimizer* min2 = ROOT::Math::Factory::CreateMinimizer("Minuit2", "MIGRAD");
//       min2->SetMaxFunctionCalls(1000000);
//       min2->SetTolerance(0.001);
//       min2->SetPrintLevel(10);
//
//           ROOT::Math::Functor f2(&totalDistWithoutZ,3);
//       double step2[3] = {0.01,0.01,.01};
//       double variable2[3] = { xs[0],xs[1],xs[3]};
//       min2->SetFunction(f2);
//       min2->SetVariable(0,"x"  ,variable2[0], step2[0]);
//       min2->SetVariable(1,"y"  ,variable2[1], step2[1]);
//       min2->SetVariable(2,"phi",variable2[2], step2[2]);
//
//          // do the minimization
//          min2->Minimize();
//
//          const double *xs2 = min2->X();
//          std::cout << "Minimum: f(" << xs2[0] << "," << xs2[1]<< "," << xs2[2] << "): "
//                    << min2->MinValue()  << std::endl;




//       // expected minimum is 0
//       if ( min->MinValue()  < 1.E-4  && f(xs) < 1.E-4)
//          std::cout << "   converged to the right minimum" << std::endl;
//       else {
//          std::cout << "   failed to converge !!!" << std::endl;
//          Error("NumericalMinimization","fail to converge");
//       }
  }

  GEMGeometry geo;

};

#endif

void GEMAlignment(std::string cscfileName="csc_forsync.root",std::string gemfilename = "gem_forsync.root",std::string outFileName = "plotCSCAndGEM_out.root"){
  GEMConfigInfo info;
  info.geoName               = "gemGeo.txt";
  info.vFATChanMapName       = "slot_table_904_june09.csv";
//  info.vFATChanMapName       = "slot_table.csv";
  Analyze a(cscfileName,gemfilename,&info);
  a.analyze();
  a.solve(outFileName);
//  a.draw();
}
