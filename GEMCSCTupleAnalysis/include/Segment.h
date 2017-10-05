#ifndef CSCUCLA_GEMCSCTUPLEANALYSIS_SEGMENT_H
#define CSCUCLA_GEMCSCTUPLEANALYSIS_SEGMENT_H
#include "include/Points.h"
namespace CSCGEMTuples {
class Segment {
public:
  enum Coord {X,Y,DX,DY};
  enum Cov   {XX,XY,XDX,XDY,YY,YDX,YDY,DXDX,DXDY,DYDY};
  Segment() : _coords(new double[4]), _cov(new double[10]) {};
  Segment(const Segment& inseg);
  ~Segment() {delete[] _coords; delete[] _cov;}
  void setCoords(double x, double y, double dxdz, double dydz);
  void setCov(double xx, double xy, double xdxdz, double xdydz, double yy, double ydxdz, double ydydz, double dxdzdxdz, double dxdzdydz, double dydzdydz);

  Segment rotate(double phi) const;
  Segment rotateCosSin( double c, double s) const;

  const double * coords() const {return _coords;}
  const double * cov() const {return _cov;}

  double * coords() {return _coords;}
  double * cov() {return _cov;}

  ErrorPoint2D project(double projX, double projY, double projZ, double projPhi) const;
  ErrorPoint2D project2(double projX, double projY, double projZ, double projAngleX, double projAngleY, double projPhi) const;
private:
  double * _coords;
  double * _cov;
};


};
#endif
