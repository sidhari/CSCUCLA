#!/bin/bash
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_1.root 0 10000 & 
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_2.root 10000 20000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_3.root 20000 30000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_4.root 30000 40000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_5.root 40000 50000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_6.root 50000 60000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon.root dat/Trees/EmulationResults_7.root 60000 -1 & 
