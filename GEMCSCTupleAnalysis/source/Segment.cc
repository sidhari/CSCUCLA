
#include "../include/Segment.h"
#include <cmath>

namespace CSCGEMTuples {
Segment::Segment(const Segment& inseg) : _coords(new double[4]), _cov(new double[10]) {
  _coords[X]  = inseg.coords()[X]  ;
  _coords[Y]  = inseg.coords()[Y]  ;
  _coords[DX] = inseg.coords()[DX] ;
  _coords[DY] = inseg.coords()[DY] ;

  _cov[XX]     = inseg.cov()[XX]   ;
  _cov[XY]     = inseg.cov()[XY]   ;
  _cov[XDX]    = inseg.cov()[XDX]  ;
  _cov[XDY]    = inseg.cov()[XDY]  ;
  _cov[YY]     = inseg.cov()[YY]   ;
  _cov[YDX]    = inseg.cov()[YDX]  ;
  _cov[YDY]    = inseg.cov()[YDY]  ;
  _cov[DXDX]   = inseg.cov()[DXDX] ;
  _cov[DXDY]   = inseg.cov()[DXDY] ;
  _cov[DYDY]   = inseg.cov()[DYDY] ;

}
void Segment::setCoords(double x, double y, double dxdz, double dydz) {
  _coords[X] = x; _coords[Y]=y; _coords[DX] = dxdz; _coords[DY] = dydz;
}
void Segment::setCov(double xx, double xy, double xdxdz, double xdydz, double yy, double ydxdz, double ydydz, double dxdzdxdz, double dxdzdydz, double dydzdydz) {
_cov[XX]   = xx;
_cov[XY]   = xy;
_cov[XDX]  = xdxdz;
_cov[XDY]  = xdydz;
_cov[YY]   = yy;
_cov[YDX]  = ydxdz;
_cov[YDY]  = ydydz;
_cov[DXDX] = dxdzdxdz;
_cov[DXDY] = dxdzdydz;
_cov[DYDY] = dydzdydz;
}

Segment Segment::rotate(double phi) const {
  return rotateCosSin( cos(phi), sin(phi));
}
Segment Segment::rotateCosSin( double c, double s) const {
  Segment newSeg(*this);
  newSeg.setCoords(c*_coords[X] -s*_coords[Y],c*_coords[Y] -s*_coords[X], c*_coords[DX] -s*_coords[DY], c*_coords[DY] -s*_coords[DX]);
  return newSeg;
//  return Segment(c*x() -s*y(), c*y() + s*x()  );
}


ErrorPoint2D Segment::project(double projX, double projY, double projZ, double projPhi) const {
  //First we project it to the correct Z
  double x = _coords[X] + projZ*_coords[DX];
  double y = _coords[Y] + projZ*_coords[DY];
  double xx = _cov[XX] +projZ*projZ*_cov[DXDX] +2*projZ*_cov[XDX];
  double xy = _cov[XY] +projZ*projZ*_cov[DXDY] +projZ*(_cov[XDY] + _cov[YDX]);
  double yy = _cov[YY] +projZ*projZ*_cov[DYDY] +2*projZ*_cov[YDY];
  //now move it
  ErrorPoint2D point(x+projX,y+projY,xx,xy,yy);
  //and rotate it
  if(projPhi) point = point.rotate(projPhi);
  return point;
}

ErrorPoint2D Segment::project2(double projX, double projY, double projZ, double projAngleX, double projAngleY, double projPhi) const  {

  Segment trans(*this);
  trans.setCoords(projX + _coords[X],projY + _coords[Y],_coords[DX],_coords[DY]);
    double * newCoords = trans.coords();
    double * newCov    = trans.cov();

    double cosAx = cos(projAngleX);
    double sinAx = sin(projAngleX);
    double cosAy = cos(projAngleY);
    double sinAy = sin(projAngleY);

    //determinate
    double det = cosAx*cosAy-cosAx*sinAy*newCoords[DY]-sinAx*cosAy*newCoords[DX];
    if (det == 0) det = 1; //for now;

    double x = cosAy*projZ*newCoords[DX] +cosAy*newCoords[X]+sinAy*newCoords[DX]*newCoords[Y] - sinAy*newCoords[DY]*newCoords[X];
    x/=det;

    double y = cosAx*projZ*newCoords[DY] +cosAx*newCoords[Y]+sinAx*newCoords[DY]*newCoords[X] - sinAx*newCoords[DX]*newCoords[Y];
    y/=det;

    //Errors not correct...
    double xx = newCov[XX] +projZ*projZ*newCov[DXDX] +2*projZ*newCov[XDX];
    double xy = newCov[XY] +projZ*projZ*newCov[DXDY] +projZ*(newCov[XDY] + newCov[YDX]);
    double yy = newCov[YY] +projZ*projZ*newCov[DYDY] +2*projZ*newCov[YDY];


  //now move it
  ErrorPoint2D point(x,y,xx,xy,yy);
  //and rotate it
  if(projPhi) point = point.rotate(projPhi);
  return point;
}

//ErrorPoint2D Segment::project2(double projX, double projY, double projZ, double projAngleX, double projAngleY, double projPhi) const  {
//  //rotate first so they are aligned
//  auto rot = rotate(projPhi);
//  double * newCoords = rot.coords();
//  double * newCov    = rot.cov();
//  //translate
//  newCoords[X] += projX;
//  newCoords[Y] += projY;
//
//  //Project
//  double cosAx = cos(projAngleX);
//  double sinAx = sin(projAngleX);
//  double cosAy = cos(projAngleY);
//  double sinAy = sin(projAngleY);
//
//  //determinate
//  double det = cosAx*cosAy-cosAx*sinAy*newCoords[DY]-sinAx*cosAy*newCoords[DX];
//  if (det == 0) det = 1; //for now;
//
//  double x = cosAy*projZ*newCoords[DX] +cosAy*newCoords[X]+sinAy*newCoords[DX]*newCoords[Y] - sinAy*newCoords[DY]*newCoords[X];
//  x/=det;
//
//  double y = cosAx*projZ*newCoords[DY] +cosAx*newCoords[Y]+sinAx*newCoords[DY]*newCoords[X] - sinAx*newCoords[DX]*newCoords[Y];
//  y/=det;
//
//  //Errors not correct...
//  double xx = newCov[XX] +projZ*projZ*newCov[DXDX] +2*projZ*newCov[XDX];
//  double xy = newCov[XY] +projZ*projZ*newCov[DXDY] +projZ*(newCov[XDY] + newCov[YDX]);
//  double yy = newCov[YY] +projZ*projZ*newCov[DYDY] +2*projZ*newCov[YDY];
//
//
//  //now move it
//  ErrorPoint2D point(x,y,xx,xy,yy);
//  //and rotate it
//  if(projPhi) point = point.rotate(projPhi);
//  return point;
//}

//ErrorPoint2D Segment::project2(double projX, double projY, double projZ, double projAngleX, double projAngleY, double projPhi) const  {
//  double cosAx = cos(projAngleX);
//  double sinAx = sin(projAngleX);
//  double cosAy = cos(projAngleY);
//  double sinAy = sin(projAngleY);
//
//  Segment trans(*this);
//  trans.setCoords(projX + _coords[X],projY + _coords[Y],_coords[DX],_coords[DY]);
//
//
//  //determinate
//  double det = cosAx*cosAy-cosAx*sinAy*_coords[DY]-sinAx*cosAy*_coords[DX];
//  if (det == 0) det = 1; //for now;
//
//  double x = projX + cosAy*projZ*_coords[DX] +cosAy*_coords[X]+sinAy*_coords[DX]*_coords[Y] - sinAy*_coords[DY]*_coords[X];
//  x/=det;
//
//  double y = projY + cosAx*projZ*_coords[DY] +cosAx*_coords[Y]+sinAx*_coords[DY]*_coords[X] - sinAx*_coords[DX]*_coords[Y];
//  y/=det;
//
//  //Errors not correct...
//  double xx = _cov[XX] +projZ*projZ*_cov[DXDX] +2*projZ*_cov[XDX];
//  double xy = _cov[XY] +projZ*projZ*_cov[DXDY] +projZ*(_cov[XDY] + _cov[YDX]);
//  double yy = _cov[YY] +projZ*projZ*_cov[DYDY] +2*projZ*_cov[YDY];
//
//
//  //now move it
//  ErrorPoint2D point(x,y,xx,xy,yy);
//  //and rotate it
//  if(projPhi) point = point.rotate(projPhi);
//  return point;
//}

};
