#!/bin/bash

CMSVERS=CMSSW_8_0_31

#sets up environment so c++ executable knows where libraries are
echo "Starting job on " `date` #Date/time of start of job
echo "Running on: `uname -a`" #Condor job is running on this node
echo "System software: `cat /etc/redhat-release`" #Operating System on that node
source /cvmfs/cms.cern.ch/cmsset_default.sh  ## if a tcsh script, use .csh instead of .sh
export SCRAM_ARCH=slc6_amd64_gcc530
eval `scramv1 project CMSSW ${CMSVERS}`
cp {${1},${2}} $CMSVERS/src/
cd $CMSVERS/src/
eval `scramv1 runtime -sh` # cmsenv is an alias not on the workers

#executable, inputfile, outputfile
./${1} ${2} ${3}
mv ${3} ../../ #move out of CMSSW build so it transfers properly
