#!/bin/bash
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_1.root 0 100000 & 
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_2.root 100000 200000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_3.root 200000 300000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_4.root 300000 400000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_5.root 400000 500000 &
./src/EmulationTreeCreator dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn3.root dat/Trees/EmulationResults_EverythingEndingIn3_6.root 500000 -1 &
