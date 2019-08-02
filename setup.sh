base=$(pwd)
subdir1="/CSCDigiTuples/include/CSCHelper.h"
subdir2="/CSCPatterns/include"
subdir3="/CSCDigiTuples/include/CSCInfo.h"
subdir4="/CSCPatterns/include"
newpath1=$base$subdir1
newpath2=$base$subdir2
newpath3=$base$subdir3
newpath4=$base$subdir4
ln -s $newpath1 $newpath2
ln -s $newpath3 $newpath4