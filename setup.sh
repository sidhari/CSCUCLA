base=$(pwd)
src1="/CSCDigiTuples/include/CSCHelper.h"
src2="/CSCDigiTuples/include/CSCInfo.h"
src3="/CSCDigiTuples/include/FillCSCInfo.h"
dest="/CSCPatterns/include"
srcpath1=$base$src1
srcpath2=$base$src2
srcpath3=$base$src3
destpath=$base$dest
ln -s $srcpath1 $destpath
ln -s $srcpath2 $destpath
ln -s $srcpath3 $destpath