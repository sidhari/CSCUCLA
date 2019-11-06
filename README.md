# CSCUCLA

Last updated: **6 November 2019** 


## Installation Instructions
  * On either LPC or LXPLUS run
  
  ```bash
  source /cvmfs/cms.cern.ch/cmsset_default.sh 
  cmsrel CMSSW_10_3_0_pre5 
  cd CMSSW_10_3_0_pre5/src/
  cmsenv
  git cms-init
  git clone https://github.com/williamnash/CSCUCLA.git
  . setup.sh
  cd CSCDigiTuples
  scram b -j8 
  cd ..
  ```

## Package Overview

The CSCUCLA project extracts low level information from the CSC Muon System within CMS. Taking the data from `RAW` or `RAW-RECO` and simplfying it to be used for trigger studies among other things. 

  * **CSCDigiTuples**: Package used to extract data from CMSSW to be used in tuples
  * **CSCPatterns**: Package used to analyze trigger information obtained from tuples created by the `CSCDigiTuples` package
  * **GEMCSCTupleAnalysis**: Deprecated Package written by N. McColl
