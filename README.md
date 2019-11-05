# CSCUCLA

Last updated: **5 November 2019** 

## Installation Instructions

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
