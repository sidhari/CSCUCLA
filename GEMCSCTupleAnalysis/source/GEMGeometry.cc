
#include "../include/GEMGeometry.h"
#include <iostream>
#include <TString.h>
namespace CSCGEMTuples {

Point2D GEMGeometry::getClusterPosition(int rowNumber,int firstStrip, int nStrips) const {
  return partitions[rowNumber].clusterPosition(firstStrip,nStrips) + partionCenters[rowNumber];
}
Error2D GEMGeometry::getClusterError(int rowNumber,int firstStrip, int nStrips) const {
  return partitions[rowNumber].clusterError(firstStrip,nStrips);
}
ErrorPoint2D GEMGeometry::getClusterInfo(int rowNumber,int firstStrip, int nStrips) const {
  return ErrorPoint2D( partitions[rowNumber].clusterPosition(firstStrip,nStrips) + partionCenters[rowNumber],
                       partitions[rowNumber].clusterError(firstStrip,nStrips)
  );

}

Point2D GEMGeometry::getStripCenter(int rowNumber, int stripCenter) const {
  return partitions[rowNumber].centerOfStrip(stripCenter) +   partionCenters[rowNumber];
}

float GEMGeometry::getStripAngle(int rowNumber, float strip) const {
  return partitions[rowNumber].stripAngle(strip);
}
float GEMGeometry::getRowHeight(int rowNumber) const {
  return partitions[rowNumber].height();
}
float GEMGeometry::getRowTop(int rowNumber) const {
  return partionCenters[rowNumber].y() + partitions[rowNumber].height()/2;
}
float GEMGeometry::getRowBottom(int rowNumber) const {
  return partionCenters[rowNumber].y() - partitions[rowNumber].height()/2;
}
float GEMGeometry::getRowBottomEdge(int rowNumber) const {
  return partitions[rowNumber].bottomEdgeSize();
}
float GEMGeometry::getRowTopEdge(int rowNumber) const {
  return partitions[rowNumber].topEdgeSize();
}

int GEMGeometry::findEtaPartition(float yValue) const {
  if(yValue >=  getRowTop(0) ) return -1;
  for(unsigned int iR = 0; iR < getNRows(); ++iR){
    if(yValue >=  getRowBottom(iR) ) return iR;
  }
  return getNRows();
}


void GEMGeometry::build() {
  //Hard code paramters because generality is not needed!
  const int nLay = 8;
  distBtwPartitions = 0.05;
  nColumns = 3;
  nStrips = 384;
  std::vector<float> botEdge(nLay);
  std::vector<float> topEdge(nLay);
  std::vector<float> height(nLay);

  //Numbering in CMSSW goes from wide to narrow
  botEdge[0] = 20.5701; topEdge[0] = 22.2929; height[0] = 9.7025;
  botEdge[1] = 18.8429; topEdge[1] = 20.5657; height[1] = 9.7025;
  botEdge[2] = 17.4084; topEdge[2] = 18.8384; height[2] = 8.0535;
  botEdge[3] = 15.974 ; topEdge[3] = 17.404 ; height[3] = 8.0535;
  botEdge[4] = 14.7749; topEdge[4] = 15.9696; height[4] = 6.728 ;
  botEdge[5] = 13.5759; topEdge[5] = 14.7705; height[5] = 6.728 ;
  botEdge[6] = 12.5676; topEdge[6] = 13.5714; height[6] = 5.6535;
  botEdge[7] = 11.5593; topEdge[7] = 12.5631; height[7] = 5.6535;


//  botEdge[0] = 19.5003	; topEdge[0] = 20.97895; height[0] = 8.3275;
//  botEdge[1] = 18.0172	; topEdge[1] = 19.49585; height[1] = 8.3275;
//  botEdge[2] = 16.7648	; topEdge[2] = 18.01275; height[2] = 7.0285;
//  botEdge[3] = 15.51235 ; topEdge[3] = 16.76035; height[3] = 7.0285;
//  botEdge[4] = 14.44635 ; topEdge[4] = 15.5079;  height[4] = 5.9785;
//  botEdge[5] = 13.38035 ; topEdge[5] = 14.4419;  height[5] = 5.9785;
//  botEdge[6] = 12.4698	; topEdge[6] = 13.3759;  height[6] = 5.1030;
//  botEdge[7] = 11.5593	; topEdge[7] = 12.4654;  height[7] = 5.1030;

//
//  botEdge[0] = 19.5003  ; topEdge[0] = 20.97895; height[0] = 9.7025;
//  botEdge[1] = 18.0172  ; topEdge[1] = 19.49585; height[1] = 9.7025;
//  botEdge[2] = 16.7648  ; topEdge[2] = 18.01275; height[2] = 8.0535;
//  botEdge[3] = 15.51235 ; topEdge[3] = 16.76035; height[3] = 8.0535;
//  botEdge[4] = 14.44635 ; topEdge[4] = 15.5079;  height[4] = 6.728 ;
//  botEdge[5] = 13.38035 ; topEdge[5] = 14.4419;  height[5] = 6.728 ;
//  botEdge[6] = 12.4698  ; topEdge[6] = 13.3759;  height[6] = 5.6535;
//  botEdge[7] = 11.5593  ; topEdge[7] = 12.4654;  height[7] = 5.6535;

//
//  botEdge[0] = 18.8429; topEdge[0] = 20.5657; height[0] = 9.7025;
//  botEdge[1] = 17.4084; topEdge[1] = 18.8384; height[1] = 9.7025;
//  botEdge[2] = 15.974 ; topEdge[2] = 17.404 ; height[2] = 8.0535;
//  botEdge[3] = 14.7749; topEdge[3] = 15.9696; height[3] = 8.0535;
//  botEdge[4] = 13.5759; topEdge[4] = 14.7705; height[4] = 6.728 ;
//  botEdge[5] = 12.5676; topEdge[5] = 13.5714; height[5] = 6.728 ;
//  botEdge[6] = 11.5593; topEdge[6] = 12.5631; height[6] = 5.6535;
//  botEdge[7] = 11.5593; topEdge[7] = 12.5631; height[7] = 5.6535;

//  double alpha = .925;
//  for(unsigned int iE = 0; iE < 8; ++iE){
//    topEdge[iE] = topEdge[iE] - (1 - alpha)*botEdge[iE];
//    botEdge[iE] *= alpha;
//  }

  for(unsigned int iL = 0; iL < nLay; ++iL){
    partitions.emplace_back(botEdge[iL],topEdge[iL],height[iL],nStrips);
  }

  float totalHeight = 0;
  partionCenters.resize(nLay);
  //First build it bottom up!
  for(int iL = nLay -1; iL >= 0; --iL){
    partionCenters[iL].set(0,totalHeight + height[iL]);
    totalHeight += height[iL]*2;
    if(iL) totalHeight += distBtwPartitions;
  }

  //Print in tabel as shown in the twiki
//  for(int iL = nLay -1; iL >= 0; --iL){
//    std::cout << 10.0 *  partitions[iL].bottomEdgeSize() <<","<<10.0 *  partitions[iL].topEdgeSize() <<","
//        <<10.0 * partionCenters[iL].y() -   10.0 *partitions[iL].height()/2<<","<<10.0 * partionCenters[iL].y() +   10.0 *partitions[iL].height()/2<<std::endl;
//  }

  //Now adjust it so that it is centered at 0
  Point2D displacement(0.,totalHeight/2);
  for(unsigned int iL = 0; iL < nLay; ++iL){
    partionCenters[iL] -= displacement;
  }
//  std::cout << std::endl;
  //Print in tabel as we want it!
//  for(int iL = nLay -1; iL >= 0; --iL){
//    std::cout << 10.0 *  partitions[iL].bottomEdgeSize() <<","<<10.0 *  partitions[iL].topEdgeSize() <<","
//        <<10.0 * partionCenters[iL].y() -  10.0 * partitions[iL].height()/2<<","<<10.0 * partionCenters[iL].y() +   10.0 *partitions[iL].height()/2<<std::endl;
//  }


}



}
