#!/bin/bash
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_11.root 0 100000 & 
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_12.root 100000 200000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_13.root 200000 300000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_14.root 300000 400000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_15.root 400000 500000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_16.root 500000 600000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_17.root 600000 -1 & 
