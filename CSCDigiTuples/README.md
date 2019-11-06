## CSCDigiTuples

This project extracts CSC information from `RAW` or `RAW-RECO` data and puts the information into a tuple containing all the information needed for trigger studies.

### Crab

The tuples are created using the scripts in this directory. Within the `crab/` directory running 

```bash
python makeCSCDigiTree_cfg.py
```

will run over a test sample, described within the file itself to verify the current code compiles and runs.

To run over large datasets, submit the script to crab via

```bash
python crabConfigZ18D.py
```

### Plugins

This directory contains the scripts used to generate the tuples. 

  * **CSCPatternsExtractor.cc**: This is the main plugin, generating all the trigger information we currently use to study
  * **CSCCMSSWTester.cc**: Intended to test edits to CMSSW within the framework
  * **CSCPatterns.cc**: A deprecated plugin kept for reference
  * **GetGEMGeo.cc**: Unused, written by N. McColl

### Source

Source files not run as a plugin are kept here.

  * **FillCSCInfo.cc**: Container classes use to take in flat ROOT trees and have easily accesible data when using the tuples.
  * **FilterGoodMuons.cc**: Deprecated
  * **hsData.cc**: Deprecated
  * **MuonQualityCuts.cc**: Deprecated
  * **patFilter.cc**: Deprecated

### Include

Include files associated with source and plugin files kept here.

### Macros

Currently deprecated code written by C. Bravo
