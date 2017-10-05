#ifndef CSCUCLA_GEMCSCTUPLEANALYSIS_GEMGEOMETRY_H
#define CSCUCLA_GEMCSCTUPLEANALYSIS_GEMGEOMETRY_H
#include "GEMPartitionGeometry.h"
#include<vector>
#include<string>
namespace CSCGEMTuples {

class GEMGeometry {
public:
  GEMGeometry() : distBtwPartitions(0), nColumns(0) { build();};
  unsigned int getNRows() const {return partitions.size();}
  unsigned int getNColumns() const {return nColumns;}
  unsigned int getNStrips() const {return nStrips;}
  unsigned int getNStripsPerVFAT() const {return nStrips/nColumns;}

  const GEMPartitionGeometry& getPartition(int rowNumber) const {return partitions[rowNumber];}
  Point2D getPartitionCenter(int rowNumber) const {return partionCenters[rowNumber];}

  Point2D getClusterPosition(int rowNumber,int firstStrip, int nStrips) const;
  Error2D getClusterError(int rowNumber,int firstStrip, int nStrips) const;
  ErrorPoint2D getClusterInfo(int rowNumber,int firstStrip, int nStrips) const;

  //General information
  Point2D getStripCenter(int rowNumber, int stripNumber) const;
  float getStripAngle(int rowNumber, float strip) const;
  float getStripPitch(int rowNumber, float yHeight) const;
  float getRowHeight(int rowNumber) const;
  float getRowTop(int rowNumber) const;
  float getRowBottom(int rowNumber) const;
  float getRowBottomEdge(int rowNumber) const;
  float getRowTopEdge(int rowNumber) const;

  //Finders
  //return eta partion local y belongs to
  // -1 means that it is above the widest vfat
  // nRows means it is below the narrowest one
  // (I know....)
  int findEtaPartition(float yValue) const;







private:
  void build();
  std::vector<GEMPartitionGeometry> partitions;
  std::vector<Point2D> partionCenters;
  float distBtwPartitions;
  unsigned int nColumns;
  unsigned int nStrips;
};


};
#endif
