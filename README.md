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


First run 

`$. setup.sh`

Then go to:  

https://github.com/CSCUCLA/CSCUCLA/wiki  

## Integration into CMSSW

Integration of software into CMSSW simulation must be done to test trigger performance officially. Enumerated steps towards integration are listed below
  * Pull the most recent version of [CMSSW](https://github.com/cms-sw/cmssw)
  * Edit code in
    * `Dataformats/CSCDigi/interface`
    * `L1trigger/CSCTriggerPrimitives`
  * Run tests ensuring backwards compatilbility of code (in correspondence with Tao Huang, Sven Dildick)
  * Make pull request to CMSSW who will designate commitee to review changes  
