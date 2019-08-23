#!/bin/bash
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_1.root 0 100000 & 
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_2.root 100000 200000 &
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_3.root 200000 300000 &
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_4.root 300000 400000 &
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_5.root 400000 500000 &
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_6.root 500000 600000 &
./src/CLCTSelector dat/Datasets/CSCDigiTree_Run2018D_SingleMuon_EverythingEndingIn1.root dat/Output\ Root\ Files/Efficiencies/bayes_7.root 600000 -1 & 
