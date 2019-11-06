#Creates soft links used to carry classes between CMSSW code and simpler classes
# CSCDigiTuples -> CSCPatterns
base=$(pwd)
src1="/CSCDigiTuples/include/CSCHelper.h"
src2="/CSCDigiTuples/include/CSCInfo.h"
dest="/CSCPatterns/include"
srcpath1=$base$src1
srcpath2=$base$src2
destpath=$base$dest
ln -s $srcpath1 $destpath
ln -s $srcpath2 $destpath
