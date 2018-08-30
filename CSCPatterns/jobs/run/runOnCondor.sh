#!/bin/bash

CMSVERS=CMSSW_8_0_8

#sets up environment so c++ executable knows where libraries are
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
echo "Copied Files:"
ls
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc6_amd64_gcc530
eval `scramv1 project CMSSW ${CMSVERS}`

#make a src and lib dir, in same format as where you built it
#mkdir $CMSVERS/src/src
#mkdir $CMSVERS/src/lib

mv *.tar $CMSVERS/src
cd $CMSVERS/src/
eval `scramv1 runtime -sh` # cmsenv is an alias not on the workers
tar xvf *.tar #untar the package with everything in it
make #make locally

#mv ../../${2} src
#mv *.so $CMSVERS/src/lib #move all the associated shared libraries
#mv *.cpp $CMSVERS/src/src #move all the src needed for the dictionary
#cd $CMSVERS/src/


#executable, inputfile, outputfile
./src/${1} ../../${2} ${3}
mv ${3} ../../ #move out of CMSSW build so it transfers properly
