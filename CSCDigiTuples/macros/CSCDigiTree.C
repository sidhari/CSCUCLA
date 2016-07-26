#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <math.h>

using namespace std;

void CSCDigiTree::Loop(string sName)
{
    //   In a ROOT session, you can do:
    //      root> .L CSCDigiTree.C
    //      root> CSCDigiTree t
    //      root> t.GetEntry(12); // Fill t data members with entry number 12
    //      root> t.Show();       // Show values of entry 12
    //      root> t.Show(16);     // Read and show values of entry 16
    //      root> t.Loop();       // Loop on all entries
    //

    //     This is the loop skeleton where:
    //    jentry is the global entry number in the chain
    //    ientry is the entry number in the current Tree
    //  Note that the argument to GetEntry must be:
    //    jentry for TChain::GetEntry
    //    ientry for TTree::GetEntry and TBranch::GetEntry
    //
    //       To read only selected branches, Insert statements like:
    // METHOD1:
    //    fChain->SetBranchStatus("*",0);  // disable all branches
    //    fChain->SetBranchStatus("branchname",1);  // activate branchname
    // METHOD2: replace line
    //    fChain->GetEntry(jentry);       //read all branches
    //by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) return;

    bool op = false;

    TFile *myF = new TFile(Form("%sCSCDigiTreeAna.root",sName.c_str()),"RECREATE");

    //TCanvas *c1 = new TCanvas("c1","c1",1750,400);
    //TCanvas *c2 = new TCanvas("c2","c2",850,700);

    TH1D *segLay3_h = new TH1D("segLay3_h","RecHit position of layer 3 in Segments;Position [strips]",220,-10,100);
    TH1D *seg_clctDis_h = new TH1D("seg_clctDis_h","Distance between Layer 3 RecHit and closest CLCT;Distance [strips]",100,0.0,10.0);

    TH2D *nSeg_nLCT_h = new TH2D("nSeg_nLCT_h",";Number of Segments;Number of LCTs",10,-0.5,9.5,10,-0.5,9.5);
    TH2D *nLCT_ntfLCT_h = new TH2D("nLCT_ntfLCT_h",";Number of LCTs;Number of tfLCTs",10,-0.5,9.5,10,-0.5,9.5);
    TH1D *nSegMinusNLCT_h = new TH1D("nSegMinusNLCT_h",";Number of Segments - Number of LCTs",21,-10.5,10.5);
    //TH1D *nLCTMinusNtfLCT_h = new TH2D("nLCTMinusNtfLCT_h",";Number of Segments;Number of LCTs",21,-10.5,10.5);
    TH1D *nRHpSeg_h = new TH1D("nRHpSeg_h",";Number of Segments;Number of LCTs",7,-0.5,6.5);
    TH1D *nLCTpSeg_h = new TH1D("nLCTpSeg_h",";Number of Segments;Number of LCTs",7,-0.5,6.5);

    TH1D *pt_h = new TH1D("pt_h","p_{T} of Muons for all LCTs",200,0.0,100.0);
    TH1D *pt_pid10_h = new TH1D("pt_pid10_h","p_{T} of Muons for LCTs with pid = 10",200,0.0,100.0);
    TH1D *pt_pid9_h = new TH1D("pt_pid9_h","p_{T} of Muons for LCTs with pid = 9",200,0.0,100.0);
    TH1D *pt_pid8_h = new TH1D("pt_pid8_h","p_{T} of Muons for LCTs with pid = 8",200,0.0,100.0);
    TH1D *pt_pid7_h = new TH1D("pt_pid7_h","p_{T} of Muons for LCTs with pid = 7",200,0.0,100.0);
    TH1D *pt_pid6_h = new TH1D("pt_pid6_h","p_{T} of Muons for LCTs with pid = 6",200,0.0,100.0);
    TH1D *pt_pid5_h = new TH1D("pt_pid5_h","p_{T} of Muons for LCTs with pid = 5",200,0.0,100.0);
    TH1D *pt_pid4_h = new TH1D("pt_pid4_h","p_{T} of Muons for LCTs with pid = 4",200,0.0,100.0);
    TH1D *pt_pid3_h = new TH1D("pt_pid3_h","p_{T} of Muons for LCTs with pid = 3",200,0.0,100.0);
    TH1D *pt_pid2_h = new TH1D("pt_pid2_h","p_{T} of Muons for LCTs with pid = 2",200,0.0,100.0);

    TH1D *pt_pmu_h = new TH1D("pt_pmu_h","p_{T} of pMuons for all LCTs",200,0.0,100.0);
    TH1D *pt_pmu_pid10_h = new TH1D("pt_pmu_pid10_h","p_{T} of pMuons for LCTs with pid = 10",200,0.0,100.0);
    TH1D *pt_pmu_pid9_h = new TH1D("pt_pmu_pid9_h","p_{T} of pMuons for LCTs with pid = 9,8",200,0.0,100.0);
    TH1D *pt_pmu_pid8_h = new TH1D("pt_pmu_pid8_h","p_{T} of pMuons for LCTs with pid = 7,6",200,0.0,100.0);
    TH1D *pt_pmu_pid7_h = new TH1D("pt_pmu_pid7_h","p_{T} of pMuons for LCTs with pid = 5,4",200,0.0,100.0);
    TH1D *pt_pmu_pid6_h = new TH1D("pt_pmu_pid6_h","p_{T} of pMuons for LCTs with pid = 3,2,1",200,0.0,100.0);
    TH1D *pt_pmu_pid5_h = new TH1D("pt_pmu_pid5_h","p_{T} of pMuons for LCTs with pid = 9,8",200,0.0,100.0);
    TH1D *pt_pmu_pid4_h = new TH1D("pt_pmu_pid4_h","p_{T} of pMuons for LCTs with pid = 7,6",200,0.0,100.0);
    TH1D *pt_pmu_pid3_h = new TH1D("pt_pmu_pid3_h","p_{T} of pMuons for LCTs with pid = 5,4",200,0.0,100.0);
    TH1D *pt_pmu_pid2_h = new TH1D("pt_pmu_pid2_h","p_{T} of pMuons for LCTs with pid = 3,2,1",200,0.0,100.0);

    TH1D *pt_mmu_h = new TH1D("pt_mmu_h","p-_{T} of Muons for all LCTs",200,0.0,100.0);
    TH1D *pt_mmu_pid10_h = new TH1D("pt_mmu_pid10_h","p_{T} of mMuons for LCTs with pid = 10",200,0.0,100.0);
    TH1D *pt_mmu_pid9_h = new TH1D("pt_mmu_pid9_h","p_{T} of mMuons for LCTs with pid = 9",200,0.0,100.0);
    TH1D *pt_mmu_pid8_h = new TH1D("pt_mmu_pid8_h","p_{T} of mMuons for LCTs with pid = 8",200,0.0,100.0);
    TH1D *pt_mmu_pid7_h = new TH1D("pt_mmu_pid7_h","p_{T} of mMuons for LCTs with pid = 7",200,0.0,100.0);
    TH1D *pt_mmu_pid6_h = new TH1D("pt_mmu_pid6_h","p_{T} of mMuons for LCTs with pid = 6",200,0.0,100.0);
    TH1D *pt_mmu_pid5_h = new TH1D("pt_mmu_pid5_h","p_{T} of mMuons for LCTs with pid = 5",200,0.0,100.0);
    TH1D *pt_mmu_pid4_h = new TH1D("pt_mmu_pid4_h","p_{T} of mMuons for LCTs with pid = 4",200,0.0,100.0);
    TH1D *pt_mmu_pid3_h = new TH1D("pt_mmu_pid3_h","p_{T} of mMuons for LCTs with pid = 3",200,0.0,100.0);
    TH1D *pt_mmu_pid2_h = new TH1D("pt_mmu_pid2_h","p_{T} of mMuons for LCTs with pid = 2",200,0.0,100.0);

    TH1D *pt_seg_h = new TH1D("pt_seg_h","p_{T} of Muons for all segments",100,0.0,100.0);
    TH1D *pt_0clct_h = new TH1D("pt_0clct_h","p_{T} of Muons for all segments with 0 clcts",100,0.0,100.0);

    TH1D *pt_me11_h = new TH1D("pt_me11_h","p_{T} of Muons for all LCTs in ME1/1",200,0.0,100.0);
    TH1D *pt_me11_pid10_h = new TH1D("pt_me11_pid10_h","p_{T} of Muons for LCTs with pid = 10_me11",200,0.0,100.0);
    TH1D *pt_me11_pid9_h = new TH1D("pt_me11_pid9_h","p_{T} of Muons for LCTs with pid = 9_me11",200,0.0,100.0);
    TH1D *pt_me11_pid8_h = new TH1D("pt_me11_pid8_h","p_{T} of Muons for LCTs with pid = 8_me11",200,0.0,100.0);
    TH1D *pt_me11_pid7_h = new TH1D("pt_me11_pid7_h","p_{T} of Muons for LCTs with pid = 7_me11",200,0.0,100.0);
    TH1D *pt_me11_pid6_h = new TH1D("pt_me11_pid6_h","p_{T} of Muons for LCTs with pid = 6_me11",200,0.0,100.0);
    TH1D *pt_me11_pid5_h = new TH1D("pt_me11_pid5_h","p_{T} of Muons for LCTs with pid = 5_me11",200,0.0,100.0);
    TH1D *pt_me11_pid4_h = new TH1D("pt_me11_pid4_h","p_{T} of Muons for LCTs with pid = 4_me11",200,0.0,100.0);
    TH1D *pt_me11_pid3_h = new TH1D("pt_me11_pid3_h","p_{T} of Muons for LCTs with pid = 3_me11",200,0.0,100.0);
    TH1D *pt_me11_pid2_h = new TH1D("pt_me11_pid2_h","p_{T} of Muons for LCTs with pid = 2_me11",200,0.0,100.0);
    TH1D *pt_me12_h = new TH1D("pt_me12_h","p_{T} of Muons for all LCTs in ME12",200,0.0,100.0);
    TH1D *pt_me12_pid10_h = new TH1D("pt_me12_pid10_h","p_{T} of Muons for LCTs with pid = 10_me12",200,0.0,100.0);
    TH1D *pt_me12_pid9_h = new TH1D("pt_me12_pid9_h","p_{T} of Muons for LCTs with pid = 9_me12",200,0.0,100.0);
    TH1D *pt_me12_pid8_h = new TH1D("pt_me12_pid8_h","p_{T} of Muons for LCTs with pid = 8_me12",200,0.0,100.0);
    TH1D *pt_me12_pid7_h = new TH1D("pt_me12_pid7_h","p_{T} of Muons for LCTs with pid = 7_me12",200,0.0,100.0);
    TH1D *pt_me12_pid6_h = new TH1D("pt_me12_pid6_h","p_{T} of Muons for LCTs with pid = 6_me12",200,0.0,100.0);
    TH1D *pt_me12_pid5_h = new TH1D("pt_me12_pid5_h","p_{T} of Muons for LCTs with pid = 5_me12",200,0.0,100.0);
    TH1D *pt_me12_pid4_h = new TH1D("pt_me12_pid4_h","p_{T} of Muons for LCTs with pid = 4_me12",200,0.0,100.0);
    TH1D *pt_me12_pid3_h = new TH1D("pt_me12_pid3_h","p_{T} of Muons for LCTs with pid = 3_me12",200,0.0,100.0);
    TH1D *pt_me12_pid2_h = new TH1D("pt_me12_pid2_h","p_{T} of Muons for LCTs with pid = 2_me12",200,0.0,100.0);
    TH1D *pt_me13_h = new TH1D("pt_me13_h","p_{T} of Muons for all LCTs in ME13",200,0.0,100.0);
    TH1D *pt_me13_pid10_h = new TH1D("pt_me13_pid10_h","p_{T} of Muons for LCTs with pid = 10_me13",200,0.0,100.0);
    TH1D *pt_me13_pid9_h = new TH1D("pt_me13_pid9_h","p_{T} of Muons for LCTs with pid = 9_me13",200,0.0,100.0);
    TH1D *pt_me13_pid8_h = new TH1D("pt_me13_pid8_h","p_{T} of Muons for LCTs with pid = 8_me13",200,0.0,100.0);
    TH1D *pt_me13_pid7_h = new TH1D("pt_me13_pid7_h","p_{T} of Muons for LCTs with pid = 7_me13",200,0.0,100.0);
    TH1D *pt_me13_pid6_h = new TH1D("pt_me13_pid6_h","p_{T} of Muons for LCTs with pid = 6_me13",200,0.0,100.0);
    TH1D *pt_me13_pid5_h = new TH1D("pt_me13_pid5_h","p_{T} of Muons for LCTs with pid = 5_me13",200,0.0,100.0);
    TH1D *pt_me13_pid4_h = new TH1D("pt_me13_pid4_h","p_{T} of Muons for LCTs with pid = 4_me13",200,0.0,100.0);
    TH1D *pt_me13_pid3_h = new TH1D("pt_me13_pid3_h","p_{T} of Muons for LCTs with pid = 3_me13",200,0.0,100.0);
    TH1D *pt_me13_pid2_h = new TH1D("pt_me13_pid2_h","p_{T} of Muons for LCTs with pid = 2_me13",200,0.0,100.0);
    TH1D *pt_me21_h = new TH1D("pt_me21_h","p_{T} of Muons for all LCTs in ME21",200,0.0,100.0);
    TH1D *pt_me21_pid10_h = new TH1D("pt_me21_pid10_h","p_{T} of Muons for LCTs with pid = 10_me21",200,0.0,100.0);
    TH1D *pt_me21_pid9_h = new TH1D("pt_me21_pid9_h","p_{T} of Muons for LCTs with pid = 9_me21",200,0.0,100.0);
    TH1D *pt_me21_pid8_h = new TH1D("pt_me21_pid8_h","p_{T} of Muons for LCTs with pid = 8_me21",200,0.0,100.0);
    TH1D *pt_me21_pid7_h = new TH1D("pt_me21_pid7_h","p_{T} of Muons for LCTs with pid = 7_me21",200,0.0,100.0);
    TH1D *pt_me21_pid6_h = new TH1D("pt_me21_pid6_h","p_{T} of Muons for LCTs with pid = 6_me21",200,0.0,100.0);
    TH1D *pt_me21_pid5_h = new TH1D("pt_me21_pid5_h","p_{T} of Muons for LCTs with pid = 5_me21",200,0.0,100.0);
    TH1D *pt_me21_pid4_h = new TH1D("pt_me21_pid4_h","p_{T} of Muons for LCTs with pid = 4_me21",200,0.0,100.0);
    TH1D *pt_me21_pid3_h = new TH1D("pt_me21_pid3_h","p_{T} of Muons for LCTs with pid = 3_me21",200,0.0,100.0);
    TH1D *pt_me21_pid2_h = new TH1D("pt_me21_pid2_h","p_{T} of Muons for LCTs with pid = 2_me21",200,0.0,100.0);
    TH1D *pt_me22_h = new TH1D("pt_me22_h","p_{T} of Muons for all LCTs in ME22",200,0.0,100.0);
    TH1D *pt_me22_pid10_h = new TH1D("pt_me22_pid10_h","p_{T} of Muons for LCTs with pid = 10_me22",200,0.0,100.0);
    TH1D *pt_me22_pid9_h = new TH1D("pt_me22_pid9_h","p_{T} of Muons for LCTs with pid = 9_me22",200,0.0,100.0);
    TH1D *pt_me22_pid8_h = new TH1D("pt_me22_pid8_h","p_{T} of Muons for LCTs with pid = 8_me22",200,0.0,100.0);
    TH1D *pt_me22_pid7_h = new TH1D("pt_me22_pid7_h","p_{T} of Muons for LCTs with pid = 7_me22",200,0.0,100.0);
    TH1D *pt_me22_pid6_h = new TH1D("pt_me22_pid6_h","p_{T} of Muons for LCTs with pid = 6_me22",200,0.0,100.0);
    TH1D *pt_me22_pid5_h = new TH1D("pt_me22_pid5_h","p_{T} of Muons for LCTs with pid = 5_me22",200,0.0,100.0);
    TH1D *pt_me22_pid4_h = new TH1D("pt_me22_pid4_h","p_{T} of Muons for LCTs with pid = 4_me22",200,0.0,100.0);
    TH1D *pt_me22_pid3_h = new TH1D("pt_me22_pid3_h","p_{T} of Muons for LCTs with pid = 3_me22",200,0.0,100.0);
    TH1D *pt_me22_pid2_h = new TH1D("pt_me22_pid2_h","p_{T} of Muons for LCTs with pid = 2_me22",200,0.0,100.0);
    TH1D *pt_me31_h = new TH1D("pt_me31_h","p_{T} of Muons for all LCTs in ME31",200,0.0,100.0);
    TH1D *pt_me31_pid10_h = new TH1D("pt_me31_pid10_h","p_{T} of Muons for LCTs with pid = 10_me31",200,0.0,100.0);
    TH1D *pt_me31_pid9_h = new TH1D("pt_me31_pid9_h","p_{T} of Muons for LCTs with pid = 9_me31",200,0.0,100.0);
    TH1D *pt_me31_pid8_h = new TH1D("pt_me31_pid8_h","p_{T} of Muons for LCTs with pid = 8_me31",200,0.0,100.0);
    TH1D *pt_me31_pid7_h = new TH1D("pt_me31_pid7_h","p_{T} of Muons for LCTs with pid = 7_me31",200,0.0,100.0);
    TH1D *pt_me31_pid6_h = new TH1D("pt_me31_pid6_h","p_{T} of Muons for LCTs with pid = 6_me31",200,0.0,100.0);
    TH1D *pt_me31_pid5_h = new TH1D("pt_me31_pid5_h","p_{T} of Muons for LCTs with pid = 5_me31",200,0.0,100.0);
    TH1D *pt_me31_pid4_h = new TH1D("pt_me31_pid4_h","p_{T} of Muons for LCTs with pid = 4_me31",200,0.0,100.0);
    TH1D *pt_me31_pid3_h = new TH1D("pt_me31_pid3_h","p_{T} of Muons for LCTs with pid = 3_me31",200,0.0,100.0);
    TH1D *pt_me31_pid2_h = new TH1D("pt_me31_pid2_h","p_{T} of Muons for LCTs with pid = 2_me31",200,0.0,100.0);
    TH1D *pt_me32_h = new TH1D("pt_me32_h","p_{T} of Muons for all LCTs in ME32",200,0.0,100.0);
    TH1D *pt_me32_pid10_h = new TH1D("pt_me32_pid10_h","p_{T} of Muons for LCTs with pid = 10_me32",200,0.0,100.0);
    TH1D *pt_me32_pid9_h = new TH1D("pt_me32_pid9_h","p_{T} of Muons for LCTs with pid = 9_me32",200,0.0,100.0);
    TH1D *pt_me32_pid8_h = new TH1D("pt_me32_pid8_h","p_{T} of Muons for LCTs with pid = 8_me32",200,0.0,100.0);
    TH1D *pt_me32_pid7_h = new TH1D("pt_me32_pid7_h","p_{T} of Muons for LCTs with pid = 7_me32",200,0.0,100.0);
    TH1D *pt_me32_pid6_h = new TH1D("pt_me32_pid6_h","p_{T} of Muons for LCTs with pid = 6_me32",200,0.0,100.0);
    TH1D *pt_me32_pid5_h = new TH1D("pt_me32_pid5_h","p_{T} of Muons for LCTs with pid = 5_me32",200,0.0,100.0);
    TH1D *pt_me32_pid4_h = new TH1D("pt_me32_pid4_h","p_{T} of Muons for LCTs with pid = 4_me32",200,0.0,100.0);
    TH1D *pt_me32_pid3_h = new TH1D("pt_me32_pid3_h","p_{T} of Muons for LCTs with pid = 3_me32",200,0.0,100.0);
    TH1D *pt_me32_pid2_h = new TH1D("pt_me32_pid2_h","p_{T} of Muons for LCTs with pid = 2_me32",200,0.0,100.0);
    TH1D *pt_me41_h = new TH1D("pt_me41_h","p_{T} of Muons for all LCTs in ME41",200,0.0,100.0);
    TH1D *pt_me41_pid10_h = new TH1D("pt_me41_pid10_h","p_{T} of Muons for LCTs with pid = 10_me41",200,0.0,100.0);
    TH1D *pt_me41_pid9_h = new TH1D("pt_me41_pid9_h","p_{T} of Muons for LCTs with pid = 9_me41",200,0.0,100.0);
    TH1D *pt_me41_pid8_h = new TH1D("pt_me41_pid8_h","p_{T} of Muons for LCTs with pid = 8_me41",200,0.0,100.0);
    TH1D *pt_me41_pid7_h = new TH1D("pt_me41_pid7_h","p_{T} of Muons for LCTs with pid = 7_me41",200,0.0,100.0);
    TH1D *pt_me41_pid6_h = new TH1D("pt_me41_pid6_h","p_{T} of Muons for LCTs with pid = 6_me41",200,0.0,100.0);
    TH1D *pt_me41_pid5_h = new TH1D("pt_me41_pid5_h","p_{T} of Muons for LCTs with pid = 5_me41",200,0.0,100.0);
    TH1D *pt_me41_pid4_h = new TH1D("pt_me41_pid4_h","p_{T} of Muons for LCTs with pid = 4_me41",200,0.0,100.0);
    TH1D *pt_me41_pid3_h = new TH1D("pt_me41_pid3_h","p_{T} of Muons for LCTs with pid = 3_me41",200,0.0,100.0);
    TH1D *pt_me41_pid2_h = new TH1D("pt_me41_pid2_h","p_{T} of Muons for LCTs with pid = 2_me41",200,0.0,100.0);
    TH1D *pt_me42_h = new TH1D("pt_me42_h","p_{T} of Muons for all LCTs in ME42",200,0.0,100.0);
    TH1D *pt_me42_pid10_h = new TH1D("pt_me42_pid10_h","p_{T} of Muons for LCTs with pid = 10_me42",200,0.0,100.0);
    TH1D *pt_me42_pid9_h = new TH1D("pt_me42_pid9_h","p_{T} of Muons for LCTs with pid = 9_me42",200,0.0,100.0);
    TH1D *pt_me42_pid8_h = new TH1D("pt_me42_pid8_h","p_{T} of Muons for LCTs with pid = 8_me42",200,0.0,100.0);
    TH1D *pt_me42_pid7_h = new TH1D("pt_me42_pid7_h","p_{T} of Muons for LCTs with pid = 7_me42",200,0.0,100.0);
    TH1D *pt_me42_pid6_h = new TH1D("pt_me42_pid6_h","p_{T} of Muons for LCTs with pid = 6_me42",200,0.0,100.0);
    TH1D *pt_me42_pid5_h = new TH1D("pt_me42_pid5_h","p_{T} of Muons for LCTs with pid = 5_me42",200,0.0,100.0);
    TH1D *pt_me42_pid4_h = new TH1D("pt_me42_pid4_h","p_{T} of Muons for LCTs with pid = 4_me42",200,0.0,100.0);
    TH1D *pt_me42_pid3_h = new TH1D("pt_me42_pid3_h","p_{T} of Muons for LCTs with pid = 3_me42",200,0.0,100.0);
    TH1D *pt_me42_pid2_h = new TH1D("pt_me42_pid2_h","p_{T} of Muons for LCTs with pid = 2_me42",200,0.0,100.0);

    TH1D *pid_h = new TH1D("pid_h","CLCT Pattern ID Occupancy",9,1.5,10.5);
    TH1D *pid13_h = new TH1D("pid13_h","CLCT Pattern ID Occupancy",9,1.5,10.5);
    TH1D *ED_h = new TH1D("ED_h","RecHit Energy Deposit",150,0.0,1500.0);
    TH1D *ED_cF0_h = new TH1D("ED_cF0_h","RecHit Energy Deposit",150,0.0,1500.0);
    TH1D *ED_cF1_h = new TH1D("ED_cF1_h","RecHit Energy Deposit",150,0.0,1500.0);
    TH1D *MD_cF0_h = new TH1D("MD_cF0_h","RecHit Max ADC",400,0.0,1000.0);
    TH1D *MD_cF1_h = new TH1D("MD_cF1_h","RecHit Max ADC",400,0.0,1000.0);
    TH1D *recM50_phi_h = new TH1D("recM50_phi_h","RecHit Max > 50 muon #phi",128,-3.2,3.2);
    TH1D *recM50_eta_h = new TH1D("recM50_eta_h","RecHit Max > 50 muon #eta",128,-3.2,3.2);
    TH1D *misslcts_phi_h = new TH1D("misslcts_phi_h","Missing LCT muon #phi",128,-3.2,3.2);
    TH1D *misslcts_eta_h = new TH1D("misslcts_eta_h","Missing LCT muon #eta",128,-3.2,3.2);

    TH2D *occu_h = new TH2D("occu_h","Occupancy of Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    TH2D *occuE_h = new TH2D("occuE_h","Occupancy of Even Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    TH2D *occuO_h = new TH2D("occuO_h","Occupancy of Odd Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    TH2D *NoccuE_h = new TH2D("NoccuE_h","Occupancy of Even Half Strips Shifted by CLCT Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_h = new TH2D("NoccuO_h","Occupancy of Odd Half Strips Shifted by CLCT Key Half Strip",22,-10.5,11.5,7,0.5,7.5);

    TH2D *NoccuE_pat10_h = new TH2D("NoccuE_pat10_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 10) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat10_h = new TH2D("NoccuO_pat10_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 10) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat9_h = new TH2D("NoccuE_pat9_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 9) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat9_h = new TH2D("NoccuO_pat9_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 9) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat8_h = new TH2D("NoccuE_pat8_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 8) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat8_h = new TH2D("NoccuO_pat8_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 8) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat7_h = new TH2D("NoccuE_pat7_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 7) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat7_h = new TH2D("NoccuO_pat7_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 7) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat6_h = new TH2D("NoccuE_pat6_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 6) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat6_h = new TH2D("NoccuO_pat6_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 6) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat5_h = new TH2D("NoccuE_pat5_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 5) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat5_h = new TH2D("NoccuO_pat5_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 5) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat4_h = new TH2D("NoccuE_pat4_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 4) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat4_h = new TH2D("NoccuO_pat4_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 4) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat3_h = new TH2D("NoccuE_pat3_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 3) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat3_h = new TH2D("NoccuO_pat3_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 3) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuE_pat2_h = new TH2D("NoccuE_pat2_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 2) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    TH2D *NoccuO_pat2_h = new TH2D("NoccuO_pat2_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 2) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);

    TH2D *rhHS_h = new TH2D("rhHS_h","Trigger Position and RecHit Position",93*2,-0.5,92.5,82*10,0.0,82.0);
    TH2D *rhHS_rh_h = new TH2D("rhHS_rh_h","Trigger Position and RecHit Position",93*2,-0.5,92.5,82*10,0.0,82.0);
    TH2D *rhmHSchID_h = new TH2D("rhmHSchID_h","Trigger Position minus RecHit Position in Chambers",24,-3,3,600,0.5,600.5);
    TH1D *rhmHS_h = new TH1D("rhmHS_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me11_h = new TH1D("rhmHS_me11_h","Trigger Position minus RecHit Position",243,-60.25,60.25);
    TH1D *rhmHS_rh_h = new TH1D("rhmHS_rh_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_pme1aE_h = new TH1D("rhmHS_pme1aE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_pme1aO_h = new TH1D("rhmHS_pme1aO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_pme1bE_h = new TH1D("rhmHS_pme1bE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_pme1bO_h = new TH1D("rhmHS_pme1bO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_mme1aE_h = new TH1D("rhmHS_mme1aE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_mme1aO_h = new TH1D("rhmHS_mme1aO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_mme1bE_h = new TH1D("rhmHS_mme1bE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_mme1bO_h = new TH1D("rhmHS_mme1bO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me12_h = new TH1D("rhmHS_me12_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me13_h = new TH1D("rhmHS_me13_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me21_h = new TH1D("rhmHS_me21_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me22_h = new TH1D("rhmHS_me22_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me31_h = new TH1D("rhmHS_me31_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me32_h = new TH1D("rhmHS_me32_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me41_h = new TH1D("rhmHS_me41_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_me42_h = new TH1D("rhmHS_me42_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    TH1D *rhmHS_lQ_h = new TH1D("rhmHS_lQ_h","Trigger Position minus RecHit Position for Low Energy",200,-1.0,1.0);
    TH1D *rhmHS_mQ_h = new TH1D("rhmHS_mQ_h","Trigger Position minus RecHit Position for Medium Energy",200,-1.0,1.0);
    TH1D *rhmHS_hQ_h = new TH1D("rhmHS_hQ_h","Trigger Position minus RecHit Position for High Energy",200,-1.0,1.0);
    TH1D *rhmHSb_h = new TH1D("rhmHSb_h","Chamber Number of Bad Matches",600,0.5,600.5);
    TH1D *misslctid_h = new TH1D("misslctid_h","Chambers missing LCTs",600,0.5,600.5);
    TH1D *misstflctid_h = new TH1D("misstflctid_h","Chambers missing LCTs",600,0.5,600.5);
    TH1D *miss1lct_phi_h = new TH1D("miss1lct_phi_h","Missing LCT muon #phi",128,-3.2,3.2);
    TH1D *miss1lct_eta_h = new TH1D("miss1lct_eta_h","Missing LCT muon #eta",128,-3.2,3.2);
    TH1D *miss1tflct_phi_h = new TH1D("miss1tflct_phi_h","Missing tfLCT muon #phi",128,-3.2,3.2);
    TH1D *miss1tflct_eta_h = new TH1D("miss1tflct_eta_h","Missing tfLCT muon #eta",128,-3.2,3.2);

    //Format histos to look nice when I draw them
    pid_h->SetStats(0);
    pid13_h->SetStats(0);
    ED_h->SetStats(0);
    occu_h->SetStats(0);
    NoccuE_h->SetStats(0);
    NoccuO_h->SetStats(0);
    NoccuE_pat10_h->SetStats(0);
    NoccuO_pat10_h->SetStats(0);
    NoccuE_pat9_h->SetStats(0);
    NoccuO_pat9_h->SetStats(0);
    NoccuE_pat8_h->SetStats(0);
    NoccuO_pat8_h->SetStats(0);
    NoccuE_pat7_h->SetStats(0);
    NoccuO_pat7_h->SetStats(0);
    NoccuE_pat6_h->SetStats(0);
    NoccuO_pat6_h->SetStats(0);
    NoccuE_pat5_h->SetStats(0);
    NoccuO_pat5_h->SetStats(0);
    NoccuE_pat4_h->SetStats(0);
    NoccuO_pat4_h->SetStats(0);
    NoccuE_pat3_h->SetStats(0);
    NoccuO_pat3_h->SetStats(0);
    NoccuE_pat2_h->SetStats(0);
    NoccuO_pat2_h->SetStats(0);
    occuE_h->SetStats(0);
    occuO_h->SetStats(0);
    rhHS_h->SetStats(0);
    rhmHSchID_h->SetStats(0);

    pid_h->SetLineWidth(2);
    pid13_h->SetLineWidth(2);
    ED_h->SetLineWidth(2);
    rhmHS_h->SetLineWidth(2);
    rhmHS_lQ_h->SetLineWidth(2);
    rhmHS_mQ_h->SetLineWidth(2);
    rhmHS_hQ_h->SetLineWidth(2);

    occu_h->GetXaxis()->SetTitle("Pattern ID");
    occu_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat10_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat10_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat9_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat9_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat8_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat8_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat7_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat7_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat6_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat6_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat5_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat5_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat4_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat4_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat3_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat3_h->GetXaxis()->SetTitle("Half Strip");
    NoccuE_pat2_h->GetXaxis()->SetTitle("Half Strip");
    NoccuO_pat2_h->GetXaxis()->SetTitle("Half Strip");
    occuE_h->GetXaxis()->SetTitle("Half Strip");
    occuO_h->GetXaxis()->SetTitle("Half Strip");
    rhHS_h->GetXaxis()->SetTitle("Comparator Position / 0.5 Strips");
    rhmHS_h->GetXaxis()->SetTitle("Difference [Strips]");
    rhmHS_lQ_h->GetXaxis()->SetTitle("Difference [Strips]");
    rhmHS_mQ_h->GetXaxis()->SetTitle("Difference [Strips]");
    rhmHS_hQ_h->GetXaxis()->SetTitle("Difference [Strips]");
    rhmHSb_h->GetXaxis()->SetTitle("Chamber Number");
    rhmHSchID_h->GetXaxis()->SetTitle("Difference [Strips]");

    float titO = 1.5;
    occu_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat10_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat10_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat9_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat9_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat8_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat8_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat7_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat7_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat6_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat6_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat5_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat5_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat4_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat4_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat3_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat3_h->GetXaxis()->SetTitleOffset(titO);
    NoccuE_pat2_h->GetXaxis()->SetTitleOffset(titO);
    NoccuO_pat2_h->GetXaxis()->SetTitleOffset(titO);
    occuE_h->GetXaxis()->SetTitleOffset(titO);
    occuO_h->GetXaxis()->SetTitleOffset(titO);

    rhmHS_h->GetYaxis()->SetTitleOffset(1.45);
    rhmHS_lQ_h->GetYaxis()->SetTitleOffset(1.45);
    rhmHS_mQ_h->GetYaxis()->SetTitleOffset(1.45);
    rhmHS_hQ_h->GetYaxis()->SetTitleOffset(1.45);
    rhmHSchID_h->GetYaxis()->SetTitleOffset(1.4);

    pid_h->GetYaxis()->SetTitle("Entries");
    pid13_h->GetYaxis()->SetTitle("Entries");
    ED_h->GetYaxis()->SetTitle("Entries");
    occu_h->GetYaxis()->SetTitle("Layer");
    NoccuE_h->GetYaxis()->SetTitle("Layer");
    NoccuO_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat10_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat10_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat9_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat9_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat8_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat8_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat7_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat7_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat6_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat6_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat5_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat5_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat4_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat4_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat3_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat3_h->GetYaxis()->SetTitle("Layer");
    NoccuE_pat2_h->GetYaxis()->SetTitle("Layer");
    NoccuO_pat2_h->GetYaxis()->SetTitle("Layer");
    occuE_h->GetYaxis()->SetTitle("Layer");
    occuO_h->GetYaxis()->SetTitle("Layer");
    rhHS_h->GetYaxis()->SetTitle("RecHit Position / 0.1 Strips");
    rhmHS_h->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    rhmHS_lQ_h->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    rhmHS_mQ_h->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    rhmHS_hQ_h->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    rhmHSb_h->GetYaxis()->SetTitle("Entries");
    rhmHSchID_h->GetYaxis()->SetTitle("Chamber Number");

    pid_h->GetXaxis()->CenterTitle();
    pid13_h->GetXaxis()->CenterTitle();
    ED_h->GetXaxis()->CenterTitle();
    rhHS_h->GetXaxis()->CenterTitle();
    occu_h->GetXaxis()->CenterTitle();
    NoccuE_h->GetXaxis()->CenterTitle();
    NoccuO_h->GetXaxis()->CenterTitle();
    NoccuE_pat10_h->GetXaxis()->CenterTitle();
    NoccuO_pat10_h->GetXaxis()->CenterTitle();
    NoccuE_pat9_h->GetXaxis()->CenterTitle();
    NoccuO_pat9_h->GetXaxis()->CenterTitle();
    NoccuE_pat8_h->GetXaxis()->CenterTitle();
    NoccuO_pat8_h->GetXaxis()->CenterTitle();
    NoccuE_pat7_h->GetXaxis()->CenterTitle();
    NoccuO_pat7_h->GetXaxis()->CenterTitle();
    NoccuE_pat6_h->GetXaxis()->CenterTitle();
    NoccuO_pat6_h->GetXaxis()->CenterTitle();
    NoccuE_pat5_h->GetXaxis()->CenterTitle();
    NoccuO_pat5_h->GetXaxis()->CenterTitle();
    NoccuE_pat4_h->GetXaxis()->CenterTitle();
    NoccuO_pat4_h->GetXaxis()->CenterTitle();
    NoccuE_pat3_h->GetXaxis()->CenterTitle();
    NoccuO_pat3_h->GetXaxis()->CenterTitle();
    NoccuE_pat2_h->GetXaxis()->CenterTitle();
    NoccuO_pat2_h->GetXaxis()->CenterTitle();
    occuE_h->GetXaxis()->CenterTitle();
    occuO_h->GetXaxis()->CenterTitle();
    rhHS_h->GetXaxis()->CenterTitle();
    rhmHS_h->GetXaxis()->CenterTitle();
    rhmHS_lQ_h->GetXaxis()->CenterTitle();
    rhmHS_mQ_h->GetXaxis()->CenterTitle();
    rhmHS_hQ_h->GetXaxis()->CenterTitle();
    rhmHSb_h->GetXaxis()->CenterTitle();
    rhmHSchID_h->GetXaxis()->CenterTitle();

    pid_h->GetYaxis()->CenterTitle();
    pid13_h->GetYaxis()->CenterTitle();
    ED_h->GetYaxis()->CenterTitle();
    rhHS_h->GetYaxis()->CenterTitle();
    occu_h->GetYaxis()->CenterTitle();
    NoccuE_h->GetYaxis()->CenterTitle();
    NoccuO_h->GetYaxis()->CenterTitle();
    NoccuE_pat10_h->GetYaxis()->CenterTitle();
    NoccuO_pat10_h->GetYaxis()->CenterTitle();
    NoccuE_pat9_h->GetYaxis()->CenterTitle();
    NoccuO_pat9_h->GetYaxis()->CenterTitle();
    NoccuE_pat8_h->GetYaxis()->CenterTitle();
    NoccuO_pat8_h->GetYaxis()->CenterTitle();
    NoccuE_pat7_h->GetYaxis()->CenterTitle();
    NoccuO_pat7_h->GetYaxis()->CenterTitle();
    NoccuE_pat6_h->GetYaxis()->CenterTitle();
    NoccuO_pat6_h->GetYaxis()->CenterTitle();
    NoccuE_pat5_h->GetYaxis()->CenterTitle();
    NoccuO_pat5_h->GetYaxis()->CenterTitle();
    NoccuE_pat4_h->GetYaxis()->CenterTitle();
    NoccuO_pat4_h->GetYaxis()->CenterTitle();
    NoccuE_pat3_h->GetYaxis()->CenterTitle();
    NoccuO_pat3_h->GetYaxis()->CenterTitle();
    NoccuE_pat2_h->GetYaxis()->CenterTitle();
    NoccuO_pat2_h->GetYaxis()->CenterTitle();
    occuE_h->GetYaxis()->CenterTitle();
    occuO_h->GetYaxis()->CenterTitle();
    rhHS_h->GetYaxis()->CenterTitle();
    rhmHS_h->GetYaxis()->CenterTitle();
    rhmHS_lQ_h->GetYaxis()->CenterTitle();
    rhmHS_mQ_h->GetYaxis()->CenterTitle();
    rhmHS_hQ_h->GetYaxis()->CenterTitle();
    rhmHSb_h->GetYaxis()->CenterTitle();
    rhmHSchID_h->GetYaxis()->CenterTitle();

    float labelS = 0.08;
    occu_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat10_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat10_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat9_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat9_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat8_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat8_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat7_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat7_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat6_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat6_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat5_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat5_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat4_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat4_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat3_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat3_h->GetXaxis()->SetLabelSize(labelS);
    NoccuE_pat2_h->GetXaxis()->SetLabelSize(labelS);
    NoccuO_pat2_h->GetXaxis()->SetLabelSize(labelS);
    occuE_h->GetXaxis()->SetLabelSize(labelS);
    occuO_h->GetXaxis()->SetLabelSize(labelS);

    labelS = 0.11;
    occu_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat10_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat10_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat9_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat9_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat8_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat8_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat7_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat7_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat6_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat6_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat5_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat5_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat4_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat4_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat3_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat3_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat2_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat2_h->GetYaxis()->SetLabelSize(labelS);
    occuE_h->GetYaxis()->SetLabelSize(labelS);
    occuO_h->GetYaxis()->SetLabelSize(labelS);
    rhmHS_h->GetYaxis()->SetLabelSize(0.03);
    rhmHS_lQ_h->GetYaxis()->SetLabelSize(0.03);
    rhmHS_mQ_h->GetYaxis()->SetLabelSize(0.03);
    rhmHS_hQ_h->GetYaxis()->SetLabelSize(0.03);

    labelS = 0.08;
    occu_h->GetZaxis()->SetLabelSize(labelS);
    NoccuE_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat10_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat10_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat9_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat9_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat8_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat8_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat7_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat7_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat6_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat6_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat5_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat5_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat4_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat4_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat3_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat3_h->GetYaxis()->SetLabelSize(labelS);
    NoccuE_pat2_h->GetYaxis()->SetLabelSize(labelS);
    NoccuO_pat2_h->GetYaxis()->SetLabelSize(labelS);
    occuE_h->GetZaxis()->SetLabelSize(labelS);
    occuO_h->GetZaxis()->SetLabelSize(labelS);

    int mCol = 2;
    occu_h->SetMarkerColor(mCol);
    NoccuE_h->SetMarkerColor(mCol);
    NoccuO_h->SetMarkerColor(mCol);
    NoccuE_pat10_h->SetMarkerColor(mCol);
    NoccuO_pat10_h->SetMarkerColor(mCol);
    NoccuE_pat9_h->SetMarkerColor(mCol);
    NoccuO_pat9_h->SetMarkerColor(mCol);
    NoccuE_pat8_h->SetMarkerColor(mCol);
    NoccuO_pat8_h->SetMarkerColor(mCol);
    NoccuE_pat7_h->SetMarkerColor(mCol);
    NoccuO_pat7_h->SetMarkerColor(mCol);
    NoccuE_pat6_h->SetMarkerColor(mCol);
    NoccuO_pat6_h->SetMarkerColor(mCol);
    NoccuE_pat5_h->SetMarkerColor(mCol);
    NoccuO_pat5_h->SetMarkerColor(mCol);
    NoccuE_pat4_h->SetMarkerColor(mCol);
    NoccuO_pat4_h->SetMarkerColor(mCol);
    NoccuE_pat3_h->SetMarkerColor(mCol);
    NoccuO_pat3_h->SetMarkerColor(mCol);
    NoccuE_pat2_h->SetMarkerColor(mCol);
    NoccuO_pat2_h->SetMarkerColor(mCol);
    occuE_h->SetMarkerColor(mCol);
    occuO_h->SetMarkerColor(mCol);

    float mS = 1.8;
    occu_h->SetMarkerSize(mS);
    NoccuE_h->SetMarkerSize(mS);
    NoccuO_h->SetMarkerSize(mS);
    NoccuE_pat10_h->SetMarkerSize(mS);
    NoccuO_pat10_h->SetMarkerSize(mS);
    NoccuE_pat9_h->SetMarkerSize(mS);
    NoccuO_pat9_h->SetMarkerSize(mS);
    NoccuE_pat8_h->SetMarkerSize(mS);
    NoccuO_pat8_h->SetMarkerSize(mS);
    NoccuE_pat7_h->SetMarkerSize(mS);
    NoccuO_pat7_h->SetMarkerSize(mS);
    NoccuE_pat6_h->SetMarkerSize(mS);
    NoccuO_pat6_h->SetMarkerSize(mS);
    NoccuE_pat5_h->SetMarkerSize(mS);
    NoccuO_pat5_h->SetMarkerSize(mS);
    NoccuE_pat4_h->SetMarkerSize(mS);
    NoccuO_pat4_h->SetMarkerSize(mS);
    NoccuE_pat3_h->SetMarkerSize(mS);
    NoccuO_pat3_h->SetMarkerSize(mS);
    NoccuE_pat2_h->SetMarkerSize(mS);
    NoccuO_pat2_h->SetMarkerSize(mS);
    occuE_h->SetMarkerSize(mS);
    occuO_h->SetMarkerSize(mS);

    NoccuE_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat10_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat10_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat10_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat10_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat9_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat9_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat9_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat9_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat8_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat8_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat8_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat8_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat7_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat7_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat7_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat7_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat6_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat6_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat6_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat6_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat5_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat5_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat5_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat5_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat4_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat4_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat4_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat4_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat3_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat3_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat3_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat3_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuE_pat2_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuE_pat2_h->GetYaxis()->SetBinLabel(7,"#Sigma");
    NoccuO_pat2_h->GetXaxis()->SetBinLabel(22,"#Sigma");
    NoccuO_pat2_h->GetYaxis()->SetBinLabel(7,"#Sigma");

    int Nrh = 0;
    int Ncomp = 0;
    int NcompM = 0;
    int NcompM_lE = 0;

    for(int iy = 1; iy < 7; iy++)
    {
        occu_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat10_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat10_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat9_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat9_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat8_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat8_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat7_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat7_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat6_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat6_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat5_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat5_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat4_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat4_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat3_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat3_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuE_pat2_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        NoccuO_pat2_h->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
    }

    for(int ix = 1; ix < 23; ix+=5)
    {
        occu_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat10_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat10_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat9_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat9_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat8_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat8_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat7_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat7_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat6_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat6_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat5_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat5_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat4_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat4_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat3_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat3_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuE_pat2_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        NoccuO_pat2_h->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
    }
    TH1D *testH = new TH1D("testH","testH",11,-0.5,10.5);

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries/1;jentry++) 
    {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        if(!os) continue;

        if(op)
        {
            if((int)segEc->size()!=(int)segSt->size()||(int)segSt->size()!=(int)segRi->size()||(int)segRi->size()!=Nseg)
            {
                cout << "Hmmmm: " << jentry << endl;
            }

            if(Nseg!=(int)lctId->size())
            {
                cout << "LCT: " << jentry << endl;
            }

            if(Nseg!=(int)clctId->size())
            {
                cout << "CLCT: " << jentry << endl;
            }

            if(Nseg!=(int)alctId->size())
            {
                cout << "ALCT: " << jentry << endl;
            }

            testH->Fill(compLay->size());

            if(Nseg == 7) cout << "Nseg==7 at " << jentry << endl;
        }
        else
        {

            int Nlct = 0;
            int Ntflct = 0;
            float missID = -999.9;
            //Loop over segments
            for(int iseg = 0; iseg < Nseg; iseg++)
            {
                int EC = segEc->at(iseg);
                int ST = segSt->at(iseg);
                int RI = segRi->at(iseg);
                int CH = segCh->at(iseg);
                int chSid = chamberSerial(EC, ST, RI, CH);
                bool me11 = (ST==1 && (RI==1 || RI==4));
                bool me1a = (ST==1 && RI==4);
                bool me1b = (ST==1 && RI==1);
                float segLay3 = -9;
                int Nrh_seg = 0;

                //Loop over rechits
                for(int irh = 0; irh < int(rhId->size()); irh++)
                {
                    int iID = rhId->at(irh);
                    int iLay = rhLay->at(irh);
                    float recE = rhE->at(irh);
                    float recM = rhMax->at(irh);
                    if(iID != chSid) continue;
                    Nrh_seg++;
                    Nrh++;
                    float recPos = rhPos->at(irh);
                    if(iLay == 3) segLay3 = rhPos->at(irh);
                    float minD = 999.0;
                    float fCompP = 999.0;
                    bool compFound = false;
                    for(int icomp = 0; icomp < int(compId->size()); icomp++)
                    {
                        if(iID != compId->at(icomp) || iLay != compLay->at(icomp)) continue;
                        if(compStr->at(icomp).size() == 0) continue;
                        for(int istr = 0; istr < compStr->at(icomp).size(); istr++)
                        {
                            float cPos = 2*(compStr->at(icomp).at(istr)-1) + compHS->at(icomp).at(istr);
                            cPos = cPos/2.0 + 0.75;
                            if(me1a) cPos = cPos - 64;
                            float diffD = cPos - recPos;
                            if(fabs(diffD) < fabs(minD))
                            {
                                minD = diffD;
                                fCompP = cPos;
                            }
                        }
                        compFound = true;
                    }
                    if(!compFound)
                    {
                        ED_cF0_h->Fill(recE);
                        MD_cF0_h->Fill(recM);
                        NcompM++; 
                        if(recE < 200.0)NcompM_lE++; 
                        if(recM > 50.0)
                        {
                            recM50_phi_h->Fill(phi);
                            recM50_eta_h->Fill(eta);
                            //cout << "Missing Comparator above threshold: " << jentry << endl;
                            //cout << "Run Number: " << Event_RunNumber << endl;
                            //cout << "Event Number: " << Event_EventNumber << endl;
                            //cout << "Lumi Section: " << Event_LumiSection << endl;
                            //cout << "Bunch Crossing: " << Event_BXCrossing << endl << endl;
                        }
                        continue;
                    }

                    if(EC==1 && ST==1 && RI==4 && CH%2==0) rhmHS_pme1aE_h->Fill(minD);
                    if(EC==1 && ST==1 && RI==4 && CH%2==1) rhmHS_pme1aO_h->Fill(minD);
                    if(EC==1 && ST==1 && RI==1 && CH%2==0) rhmHS_pme1bE_h->Fill(minD);
                    if(EC==1 && ST==1 && RI==1 && CH%2==1) rhmHS_pme1bO_h->Fill(minD);
                    if(EC==2 && ST==1 && RI==4 && CH%2==0) rhmHS_mme1aE_h->Fill(minD);
                    if(EC==2 && ST==1 && RI==4 && CH%2==1) rhmHS_mme1aO_h->Fill(minD);
                    if(EC==2 && ST==1 && RI==1 && CH%2==0) rhmHS_mme1bE_h->Fill(minD);
                    if(EC==2 && ST==1 && RI==1 && CH%2==1) rhmHS_mme1bO_h->Fill(minD);
                    if(ST==1 && RI==2)                     rhmHS_me12_h->Fill(minD);
                    if(ST==1 && RI==3)                     rhmHS_me13_h->Fill(minD);
                    if(ST==2 && RI==1)                     rhmHS_me21_h->Fill(minD);
                    if(ST==2 && RI==2)                     rhmHS_me22_h->Fill(minD);
                    if(ST==3 && RI==1)                     rhmHS_me31_h->Fill(minD);
                    if(ST==3 && RI==2)                     rhmHS_me32_h->Fill(minD);
                    if(ST==4 && RI==1)                     rhmHS_me41_h->Fill(minD);
                    if(ST==4 && RI==2)                     rhmHS_me42_h->Fill(minD);

                    Ncomp++;
                    ED_cF1_h->Fill(rhE->at(irh));
                    MD_cF1_h->Fill(rhMax->at(irh));
                    rhHS_rh_h->Fill(fCompP,recPos);
                    if((EC==1 && me1a)||(EC==2 && me1b)) rhmHS_rh_h->Fill(-1.0*minD);
                    else rhmHS_rh_h->Fill(minD);
                }
                segLay3_h->Fill(segLay3);

                //if (me11) continue;

                int Nclct = 0;
                int pid = -9;
                int Nlct_seg = 0;
                float seg_clctDis = 999.9;
                pt_seg_h->Fill(Pt);

                //Count LCTs
                for(int ilct = 0; ilct < (int) lctId->size(); ilct++)
                {
                    if(!(chSid == lctId->at(ilct))) continue;
                    for(int jlct = 0; jlct < lctQ->at(ilct).size(); jlct++)
                    {
                        Nlct++;
                        Nlct_seg++;
                    }
                }

                int Ntflct_seg = 0;
                //Count tfLCTs
                for(int itflct = 0; itflct < (int) tflctId->size(); itflct++)
                {
                    if(!(chSid == tflctId->at(itflct))) continue;
                    for(int jtflct = 0; jtflct < tflctQ->at(itflct).size(); jtflct++)
                    {
                        Ntflct++;
                        Ntflct_seg++;
                    }
                }


                //Count tfLCTs
                //for(int itflct = 0; itflct < (int) tflctId->size(); itflct++)
                //{
                //    if(!(chSid == tflctId->at(itflct))) continue;
                //    for(int jtflct = 0; jtflct < tflctQ->at(itflct).size(); jtflct++)
                //    {
                //        Ntflct++;
                //    }
                //}

                //Look for a CLCT
                for(int iclct = 0; iclct < (int) clctId->size(); iclct++)
                {
                    if(!(chSid == clctId->at(iclct))) continue;
                    Nclct++;
                    for(int jclct = 0; jclct < clctQ->at(iclct).size(); jclct++)
                    {
                        int KHS = 32*clctCFEB->at(iclct).at(jclct)+clctKHS->at(iclct).at(jclct);
                        pid_h->Fill(clctPat->at(iclct).at(jclct));
                        if(fabs( ( (KHS/2.0) + 0.75 ) - segLay3 ) < seg_clctDis )
                        {
                            seg_clctDis = fabs( ( (KHS/2.0) + 0.75 ) - segLay3 );
                            pid = clctPat->at(iclct).at(jclct);
                        }

                        if(ST==1 && RI==3 && Pt < 5.0) pid13_h->Fill(clctPat->at(iclct).at(jclct));
                        //Find Comparators for this clct
                        for(int icomp = 0; icomp < compId->size(); icomp++)
                        {
                            if(!(chSid == compId->at(icomp))) continue;
                            int minD = 999;
                            int layN = compLay->at(icomp);
                            for(int jcomp = 0; jcomp < (int) compStr->at(icomp).size(); jcomp++)
                            {
                                int diffN = 0;
                                if(me11)
                                {
                                    diffN = 2*(compStr->at(icomp).at(jcomp)-1) + compHS->at(icomp).at(jcomp) - KHS;
                                }
                                else
                                {
                                    if(layN%2==1) diffN = 2*(compStr->at(icomp).at(jcomp)-1) + compHS->at(icomp).at(jcomp) - KHS;
                                    else diffN = 2*(compStr->at(icomp).at(jcomp) - 1) + compHS->at(icomp).at(jcomp) - 1 - KHS;
                                    //else diffN = 2*(compStr->at(icomp).at(jcomp)-1) + compHS->at(icomp).at(jcomp) - KHS;
                                }
                                if(fabs(diffN) < fabs(minD)) minD = diffN;
                            }//jcomp
                            occu_h->Fill(minD,layN);
                            float recPos = -99.9;
                            float recE = -999.0;
                            for(int irh = 0; irh < rhId->size(); irh++)
                            {
                                if(chSid != rhId->at(irh)) continue;
                                if(layN == rhLay->at(irh)) 
                                {
                                    recPos = rhPos->at(irh);
                                    recE = rhE->at(irh);
                                }
                            }
                            if(recPos < -2.0) continue;

                            float compPos = float (minD + KHS)/2.0 + 0.75;
                            if(!me11 && layN%2==0) compPos = (minD + 1 + KHS)/2.0 + 0.75;
                            if(me1a) compPos = compPos - 64;
                            rhHS_h->Fill(compPos,recPos);
                            if(recE > 0.0) rhmHS_h->Fill(compPos - recPos);
                            if(recE > 0.0 && me11) rhmHS_me11_h->Fill(compPos - recPos);
                            if(recE > 0.0 && recE < 200.0) rhmHS_lQ_h->Fill(compPos - recPos);
                            if(recE > 200.0 && recE < 500.0) rhmHS_mQ_h->Fill(compPos - recPos);
                            if(recE > 500.0) rhmHS_hQ_h->Fill(compPos - recPos);
                            if(fabs(compPos - recPos) > 0.5) rhmHSb_h->Fill(chSid);
                            rhmHSchID_h->Fill(compPos - recPos,chSid);
                            if(recE) ED_h->Fill(recE);

                            if(minD > 10 || minD < -10) continue;
                            if(KHS%2==1)
                            {
                                occuO_h->Fill(minD,layN);
                                NoccuO_h->Fill(minD,layN);
                                NoccuO_h->Fill(minD,7);
                                NoccuO_h->Fill(11,layN);
                                if(clctPat->at(iclct).at(jclct) == 10)
                                {
                                    NoccuO_pat10_h->Fill(minD,layN);
                                    NoccuO_pat10_h->Fill(minD,7);
                                    NoccuO_pat10_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 9)
                                {
                                    NoccuO_pat9_h->Fill(minD,layN);
                                    NoccuO_pat9_h->Fill(minD,7);
                                    NoccuO_pat9_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 8)
                                {
                                    NoccuO_pat8_h->Fill(minD,layN);
                                    NoccuO_pat8_h->Fill(minD,7);
                                    NoccuO_pat8_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 7)
                                {
                                    NoccuO_pat7_h->Fill(minD,layN);
                                    NoccuO_pat7_h->Fill(minD,7);
                                    NoccuO_pat7_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 6)
                                {
                                    NoccuO_pat6_h->Fill(minD,layN);
                                    NoccuO_pat6_h->Fill(minD,7);
                                    NoccuO_pat6_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 5)
                                {
                                    NoccuO_pat5_h->Fill(minD,layN);
                                    NoccuO_pat5_h->Fill(minD,7);
                                    NoccuO_pat5_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 4)
                                {
                                    NoccuO_pat4_h->Fill(minD,layN);
                                    NoccuO_pat4_h->Fill(minD,7);
                                    NoccuO_pat4_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 3)
                                {
                                    NoccuO_pat3_h->Fill(minD,layN);
                                    NoccuO_pat3_h->Fill(minD,7);
                                    NoccuO_pat3_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 2)
                                {
                                    NoccuO_pat2_h->Fill(minD,layN);
                                    NoccuO_pat2_h->Fill(minD,7);
                                    NoccuO_pat2_h->Fill(11,layN);
                                }
                            }
                            else 
                            {
                                occuE_h->Fill(minD,layN);
                                NoccuE_h->Fill(minD,layN);
                                NoccuE_h->Fill(minD,7);
                                NoccuE_h->Fill(11,layN);
                                if(clctPat->at(iclct).at(jclct) == 10)
                                {
                                    NoccuE_pat10_h->Fill(minD,layN);
                                    NoccuE_pat10_h->Fill(minD,7);
                                    NoccuE_pat10_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 9)
                                {
                                    NoccuE_pat9_h->Fill(minD,layN);
                                    NoccuE_pat9_h->Fill(minD,7);
                                    NoccuE_pat9_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 8)
                                {
                                    NoccuE_pat8_h->Fill(minD,layN);
                                    NoccuE_pat8_h->Fill(minD,7);
                                    NoccuE_pat8_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 7)
                                {
                                    NoccuE_pat7_h->Fill(minD,layN);
                                    NoccuE_pat7_h->Fill(minD,7);
                                    NoccuE_pat7_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 6)
                                {
                                    NoccuE_pat6_h->Fill(minD,layN);
                                    NoccuE_pat6_h->Fill(minD,7);
                                    NoccuE_pat6_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 5)
                                {
                                    NoccuE_pat5_h->Fill(minD,layN);
                                    NoccuE_pat5_h->Fill(minD,7);
                                    NoccuE_pat5_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 4)
                                {
                                    NoccuE_pat4_h->Fill(minD,layN);
                                    NoccuE_pat4_h->Fill(minD,7);
                                    NoccuE_pat4_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 3)
                                {
                                    NoccuE_pat3_h->Fill(minD,layN);
                                    NoccuE_pat3_h->Fill(minD,7);
                                    NoccuE_pat3_h->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 2)
                                {
                                    NoccuE_pat2_h->Fill(minD,layN);
                                    NoccuE_pat2_h->Fill(minD,7);
                                    NoccuE_pat2_h->Fill(11,layN);
                                }
                            }
                        }//icomp
                    }
                }//iclct
                if(Nrh_seg == 4 && Nlct_seg == 0) cout << "Show event " << jentry << endl;
                nRHpSeg_h->Fill(Nrh_seg);
                nLCTpSeg_h->Fill(Nlct_seg);
                if(Nlct_seg == 0 || Ntflct_seg == 0) missID = chSid;
                if(segLay3 == -9) continue;
                //if(Nrh_seg != 4) continue;
                seg_clctDis_h->Fill(seg_clctDis);
                pt_h->Fill(Pt);
                if(q == 1) pt_pmu_h->Fill(Pt);
                if(q == -1) pt_pmu_h->Fill(Pt);
                if(ST==1 && (RI==1 || RI == 4)) pt_me11_h->Fill(Pt);
                if(ST==1 && RI==2) pt_me12_h->Fill(Pt);
                if(ST==1 && RI==3) pt_me13_h->Fill(Pt);
                if(ST==2 && RI==1) pt_me21_h->Fill(Pt);
                if(ST==2 && RI==2) pt_me22_h->Fill(Pt);
                if(ST==3 && RI==1) pt_me31_h->Fill(Pt);
                if(ST==3 && RI==2) pt_me32_h->Fill(Pt);
                if(ST==4 && RI==1) pt_me41_h->Fill(Pt);
                if(ST==4 && RI==2) pt_me42_h->Fill(Pt);
                if(Nclct == 0) pt_0clct_h->Fill(Pt);

                if(pid == -9) continue;

                if(pid == 10 && q==1) pt_pmu_pid10_h->Fill(Pt);
                if(pid==9 && q==1) pt_pmu_pid9_h->Fill(Pt);
                if(pid==8 && q==1) pt_pmu_pid8_h->Fill(Pt);
                if(pid==7 && q==1) pt_pmu_pid7_h->Fill(Pt);
                if(pid==6 && q==1) pt_pmu_pid6_h->Fill(Pt);
                if(pid==5 && q==1) pt_pmu_pid5_h->Fill(Pt);
                if(pid==4 && q==1) pt_pmu_pid4_h->Fill(Pt);
                if(pid==3 && q==1) pt_pmu_pid3_h->Fill(Pt);
                if(pid==2 && q==1) pt_pmu_pid2_h->Fill(Pt);
                if(pid == 10 && q==-1) pt_mmu_pid10_h->Fill(Pt);
                if(pid==9 && q==-1) pt_mmu_pid9_h->Fill(Pt);
                if(pid==8 && q==-1) pt_mmu_pid8_h->Fill(Pt);
                if(pid==7 && q==-1) pt_mmu_pid7_h->Fill(Pt);
                if(pid==6 && q==-1) pt_mmu_pid6_h->Fill(Pt);
                if(pid==5 && q==-1) pt_mmu_pid5_h->Fill(Pt);
                if(pid==4 && q==-1) pt_mmu_pid4_h->Fill(Pt);
                if(pid==3 && q==-1) pt_mmu_pid3_h->Fill(Pt);
                if(pid==2 && q==-1) pt_mmu_pid2_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid ==10) pt_me11_pid10_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==9) pt_me11_pid9_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==8) pt_me11_pid8_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==7) pt_me11_pid7_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==6) pt_me11_pid6_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==5) pt_me11_pid5_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==4) pt_me11_pid4_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==3) pt_me11_pid3_h->Fill(Pt);
                if((ST==1 && (RI==1 || RI == 4)) && pid==2) pt_me11_pid2_h->Fill(Pt);
                if((ST==1 && RI==2) && pid ==10) pt_me12_pid10_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==9) pt_me12_pid9_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==8) pt_me12_pid8_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==7) pt_me12_pid7_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==6) pt_me12_pid6_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==5) pt_me12_pid5_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==4) pt_me12_pid4_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==3) pt_me12_pid3_h->Fill(Pt);
                if((ST==1 && RI==2) && pid==2) pt_me12_pid2_h->Fill(Pt);
                if((ST==1 && RI==3) && pid ==10) pt_me13_pid10_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==9) pt_me13_pid9_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==8) pt_me13_pid8_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==7) pt_me13_pid7_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==6) pt_me13_pid6_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==5) pt_me13_pid5_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==4) pt_me13_pid4_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==3) pt_me13_pid3_h->Fill(Pt);
                if((ST==1 && RI==3) && pid==2) pt_me13_pid2_h->Fill(Pt);
                if((ST==2 && RI==1) && pid ==10) pt_me21_pid10_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==9) pt_me21_pid9_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==8) pt_me21_pid8_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==7) pt_me21_pid7_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==6) pt_me21_pid6_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==5) pt_me21_pid5_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==4) pt_me21_pid4_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==3) pt_me21_pid3_h->Fill(Pt);
                if((ST==2 && RI==1) && pid==2) pt_me21_pid2_h->Fill(Pt);
                if((ST==2 && RI==2) && pid ==10) pt_me22_pid10_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==9) pt_me22_pid9_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==8) pt_me22_pid8_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==7) pt_me22_pid7_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==6) pt_me22_pid6_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==5) pt_me22_pid5_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==4) pt_me22_pid4_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==3) pt_me22_pid3_h->Fill(Pt);
                if((ST==2 && RI==2) && pid==2) pt_me22_pid2_h->Fill(Pt);
                if((ST==3 && RI==1) && pid ==10) pt_me31_pid10_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==9) pt_me31_pid9_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==8) pt_me31_pid8_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==7) pt_me31_pid7_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==6) pt_me31_pid6_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==5) pt_me31_pid5_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==4) pt_me31_pid4_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==3) pt_me31_pid3_h->Fill(Pt);
                if((ST==3 && RI==1) && pid==2) pt_me31_pid2_h->Fill(Pt);
                if((ST==3 && RI==2) && pid ==10) pt_me32_pid10_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==9) pt_me32_pid9_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==8) pt_me32_pid8_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==7) pt_me32_pid7_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==6) pt_me32_pid6_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==5) pt_me32_pid5_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==4) pt_me32_pid4_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==3) pt_me32_pid3_h->Fill(Pt);
                if((ST==3 && RI==2) && pid==2) pt_me32_pid2_h->Fill(Pt);
                if((ST==4 && RI==1) && pid ==10) pt_me41_pid10_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==9) pt_me41_pid9_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==8) pt_me41_pid8_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==7) pt_me41_pid7_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==6) pt_me41_pid6_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==5) pt_me41_pid5_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==4) pt_me41_pid4_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==3) pt_me41_pid3_h->Fill(Pt);
                if((ST==4 && RI==1) && pid==2) pt_me41_pid2_h->Fill(Pt);
                if((ST==4 && RI==2) && pid ==10) pt_me42_pid10_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==9) pt_me42_pid9_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==8) pt_me42_pid8_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==7) pt_me42_pid7_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==6) pt_me42_pid6_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==5) pt_me42_pid5_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==4) pt_me42_pid4_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==3) pt_me42_pid3_h->Fill(Pt);
                if((ST==4 && RI==2) && pid==2) pt_me42_pid2_h->Fill(Pt);

                if(pid == 10) pt_pid10_h->Fill(Pt);
                if(pid==9) pt_pid9_h->Fill(Pt);
                if(pid==8) pt_pid8_h->Fill(Pt);
                if(pid==7) pt_pid7_h->Fill(Pt);
                if(pid==6) pt_pid6_h->Fill(Pt);
                if(pid==5) pt_pid5_h->Fill(Pt);
                if(pid==4) pt_pid4_h->Fill(Pt);
                if(pid==3) pt_pid3_h->Fill(Pt);
                if(pid==2) pt_pid2_h->Fill(Pt);
            }//iseg
            nLCT_ntfLCT_h->Fill(Nlct,Ntflct);
            if(Nlct != 0 && Nlct - Ntflct == 1) { misstflctid_h->Fill(missID); miss1tflct_phi_h->Fill(phi); miss1tflct_eta_h->Fill(eta); }
            if(Nlct != 0 && Nlct - Ntflct == -1) { misslctid_h->Fill(missID); miss1lct_phi_h->Fill(phi); miss1lct_eta_h->Fill(eta); }
            if(Nlct == 0 && Ntflct > 0) { misslcts_phi_h->Fill(phi); misslcts_eta_h->Fill(eta); }
            nSeg_nLCT_h->Fill(Nseg,Nlct);
            nSegMinusNLCT_h->Fill(Nseg-Nlct);
            //nLCTMinusNtfLCT_h->Fill(Nlct-Ntflct);
        }//option if
    }

    for(int ix = 1; ix < 22; ix++)
    {
        for(int iy = 1; iy < 7; iy++)
        {
            NoccuO_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_h->GetBinContent(ix,iy)/NoccuO_h->GetBinContent(22,iy)))/10.0);
            NoccuE_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_h->GetBinContent(ix,iy)/NoccuE_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat10_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat10_h->GetBinContent(ix,iy)/NoccuO_pat10_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat10_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat10_h->GetBinContent(ix,iy)/NoccuE_pat10_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat9_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat9_h->GetBinContent(ix,iy)/NoccuO_pat9_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat9_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat9_h->GetBinContent(ix,iy)/NoccuE_pat9_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat8_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat8_h->GetBinContent(ix,iy)/NoccuO_pat8_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat8_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat8_h->GetBinContent(ix,iy)/NoccuE_pat8_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat7_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat7_h->GetBinContent(ix,iy)/NoccuO_pat7_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat7_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat7_h->GetBinContent(ix,iy)/NoccuE_pat7_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat6_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat6_h->GetBinContent(ix,iy)/NoccuO_pat6_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat6_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat6_h->GetBinContent(ix,iy)/NoccuE_pat6_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat5_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat5_h->GetBinContent(ix,iy)/NoccuO_pat5_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat5_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat5_h->GetBinContent(ix,iy)/NoccuE_pat5_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat4_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat4_h->GetBinContent(ix,iy)/NoccuO_pat4_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat4_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat4_h->GetBinContent(ix,iy)/NoccuE_pat4_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat3_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat3_h->GetBinContent(ix,iy)/NoccuO_pat3_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat3_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat3_h->GetBinContent(ix,iy)/NoccuE_pat3_h->GetBinContent(22,iy)))/10.0);
            NoccuO_pat2_h->SetBinContent(ix,iy,float(int(1000.0*NoccuO_pat2_h->GetBinContent(ix,iy)/NoccuO_pat2_h->GetBinContent(22,iy)))/10.0);
            NoccuE_pat2_h->SetBinContent(ix,iy,float(int(1000.0*NoccuE_pat2_h->GetBinContent(ix,iy)/NoccuE_pat2_h->GetBinContent(22,iy)))/10.0);
        }
    }



    NoccuO_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat10_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat10_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat9_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat9_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat8_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat8_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat7_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat7_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat6_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat6_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat5_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat5_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat4_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat4_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat3_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat3_h->SetAxisRange(0.0,100.0,"Z");
    NoccuO_pat2_h->SetAxisRange(0.0,100.0,"Z");
    NoccuE_pat2_h->SetAxisRange(0.0,100.0,"Z");

    TLine *l1 = new TLine(10.5,0.5,10.5,6.5);
    l1->SetLineWidth(3);
    TLine *l2 = new TLine(-10.5,6.5,10.5,6.5);
    l2->SetLineWidth(3);

    if(!op)
    {
        /*c1->cd();
        occu_h->Draw("colz text");
        c1->SaveAs(Form("../%s/plots/occu.png",sName.c_str()));

        occuE_h->Draw("colz text");
        c1->SaveAs(Form("../%s/plots/occuEven.png",sName.c_str()));

        occuO_h->Draw("colz text");
        c1->SaveAs(Form("../%s/plots/occuOdd.png",sName.c_str()));

        NoccuE_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        c1->SaveAs(Form("../%s/plots/NoccuEven.png",sName.c_str()));

        NoccuO_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        c1->SaveAs(Form("../%s/plots/NoccuOdd.png",sName.c_str()));

        NoccuE_pat10_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(10);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat10.png",sName.c_str()));
        NoccuO_pat10_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(10);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat10.png",sName.c_str()));
        NoccuE_pat9_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(9);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat9.png",sName.c_str()));
        NoccuO_pat9_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(9);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat9.png",sName.c_str()));
        NoccuE_pat8_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(8);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat8.png",sName.c_str()));
        NoccuO_pat8_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(8);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat8.png",sName.c_str()));
        NoccuE_pat7_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(7);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat7.png",sName.c_str()));
        NoccuO_pat7_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(7);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat7.png",sName.c_str()));
        NoccuE_pat6_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(6);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat6.png",sName.c_str()));
        NoccuO_pat6_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(6);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat6.png",sName.c_str()));
        NoccuE_pat5_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(5);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat5.png",sName.c_str()));
        NoccuO_pat5_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(5);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat5.png",sName.c_str()));
        NoccuE_pat4_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(4);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat4.png",sName.c_str()));
        NoccuO_pat4_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(4);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat4.png",sName.c_str()));
        NoccuE_pat3_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(3);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat3.png",sName.c_str()));
        NoccuO_pat3_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(3);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat3.png",sName.c_str()));
        NoccuE_pat2_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(2);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat2.png",sName.c_str()));
        NoccuO_pat2_h->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(2);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat2.png",sName.c_str()));

        c2->cd();

        rhmHSchID_h->Draw("colz");
        c2->SaveAs(Form("../%s/plots/rhmHSchID.png",sName.c_str()));

        rhmHSb_h->Draw();
        c2->SaveAs(Form("../%s/plots/rhHSb.png",sName.c_str()));

        rhHS_h->Draw("colz");
        c2->SaveAs(Form("../%s/plots/rhHS.png",sName.c_str()));

        rhmHS_h->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS.png",sName.c_str()));

        rhmHS_lQ_h->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_lQ.png",sName.c_str()));

        rhmHS_mQ_h->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_mQ.png",sName.c_str()));

        rhmHS_hQ_h->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_hQ.png",sName.c_str()));*/

        segLay3_h->Write();
        seg_clctDis_h->Write();
        nSeg_nLCT_h->Write();
        nLCT_ntfLCT_h->Write();
        misslctid_h->Write();
        misslcts_phi_h->Write();
        misslcts_eta_h->Write();
        misstflctid_h->Write();
        miss1tflct_phi_h->Write();
        miss1tflct_eta_h->Write();
        miss1lct_phi_h->Write();
        miss1lct_eta_h->Write();
        nSegMinusNLCT_h->Write();
        //nLCTMinusNtfLCT_h->Write();
        nRHpSeg_h->Write();
        nLCTpSeg_h->Write();
        pt_h->Write();
        pt_seg_h->Write();
        pt_0clct_h->Write();
        pt_pid10_h->Write();
        pt_pid9_h->Write();
        pt_pid8_h->Write();
        pt_pid7_h->Write();
        pt_pid6_h->Write();
        pt_pid5_h->Write();
        pt_pid4_h->Write();
        pt_pid3_h->Write();
        pt_pid2_h->Write();
        pt_pmu_h->Write();
        pt_pmu_pid10_h->Write();
        pt_pmu_pid9_h->Write();
        pt_pmu_pid8_h->Write();
        pt_pmu_pid7_h->Write();
        pt_pmu_pid6_h->Write();
        pt_pmu_pid5_h->Write();
        pt_pmu_pid4_h->Write();
        pt_pmu_pid3_h->Write();
        pt_pmu_pid2_h->Write();
        pt_mmu_h->Write();
        pt_mmu_pid10_h->Write();
        pt_mmu_pid9_h->Write();
        pt_mmu_pid8_h->Write();
        pt_mmu_pid7_h->Write();
        pt_mmu_pid6_h->Write();
        pt_mmu_pid5_h->Write();
        pt_mmu_pid4_h->Write();
        pt_mmu_pid3_h->Write();
        pt_mmu_pid2_h->Write();
        pt_me11_h->Write();
        pt_me11_pid10_h->Write();
        pt_me11_pid9_h->Write();
        pt_me11_pid8_h->Write();
        pt_me11_pid7_h->Write();
        pt_me11_pid6_h->Write();
        pt_me11_pid5_h->Write();
        pt_me11_pid4_h->Write();
        pt_me11_pid3_h->Write();
        pt_me11_pid2_h->Write();
        pt_me11_h->Write();
        pt_me12_pid10_h->Write();
        pt_me12_pid9_h->Write();
        pt_me12_pid8_h->Write();
        pt_me12_pid7_h->Write();
        pt_me12_pid6_h->Write();
        pt_me12_pid5_h->Write();
        pt_me12_pid4_h->Write();
        pt_me12_pid3_h->Write();
        pt_me12_pid2_h->Write();
        pt_me13_h->Write();
        pt_me13_pid10_h->Write();
        pt_me13_pid9_h->Write();
        pt_me13_pid8_h->Write();
        pt_me13_pid7_h->Write();
        pt_me13_pid6_h->Write();
        pt_me13_pid5_h->Write();
        pt_me13_pid4_h->Write();
        pt_me13_pid3_h->Write();
        pt_me13_pid2_h->Write();
        pt_me21_h->Write();
        pt_me21_pid10_h->Write();
        pt_me21_pid9_h->Write();
        pt_me21_pid8_h->Write();
        pt_me21_pid7_h->Write();
        pt_me21_pid6_h->Write();
        pt_me21_pid5_h->Write();
        pt_me21_pid4_h->Write();
        pt_me21_pid3_h->Write();
        pt_me21_pid2_h->Write();
        pt_me22_h->Write();
        pt_me22_pid10_h->Write();
        pt_me22_pid9_h->Write();
        pt_me22_pid8_h->Write();
        pt_me22_pid7_h->Write();
        pt_me22_pid6_h->Write();
        pt_me22_pid5_h->Write();
        pt_me22_pid4_h->Write();
        pt_me22_pid3_h->Write();
        pt_me22_pid2_h->Write();
        pt_me31_h->Write();
        pt_me31_pid10_h->Write();
        pt_me31_pid9_h->Write();
        pt_me31_pid8_h->Write();
        pt_me31_pid7_h->Write();
        pt_me31_pid6_h->Write();
        pt_me31_pid5_h->Write();
        pt_me31_pid4_h->Write();
        pt_me31_pid3_h->Write();
        pt_me31_pid2_h->Write();
        pt_me32_h->Write();
        pt_me32_pid10_h->Write();
        pt_me32_pid9_h->Write();
        pt_me32_pid8_h->Write();
        pt_me32_pid7_h->Write();
        pt_me32_pid6_h->Write();
        pt_me32_pid5_h->Write();
        pt_me32_pid4_h->Write();
        pt_me32_pid3_h->Write();
        pt_me32_pid2_h->Write();
        pt_me41_h->Write();
        pt_me41_pid10_h->Write();
        pt_me41_pid9_h->Write();
        pt_me41_pid8_h->Write();
        pt_me41_pid7_h->Write();
        pt_me41_pid6_h->Write();
        pt_me41_pid5_h->Write();
        pt_me41_pid4_h->Write();
        pt_me41_pid3_h->Write();
        pt_me41_pid2_h->Write();
        pt_me42_h->Write();
        pt_me42_pid10_h->Write();
        pt_me42_pid9_h->Write();
        pt_me42_pid8_h->Write();
        pt_me42_pid7_h->Write();
        pt_me42_pid6_h->Write();
        pt_me42_pid5_h->Write();
        pt_me42_pid4_h->Write();
        pt_me42_pid3_h->Write();
        pt_me42_pid2_h->Write();
        pid_h->Write();
        pid13_h->Write();
        ED_h->Write();
        ED_cF0_h->Write();
        ED_cF1_h->Write();
        MD_cF0_h->Write();
        MD_cF1_h->Write();
        recM50_phi_h->Write();
        recM50_eta_h->Write();
        occu_h->Write();
        occuE_h->Write();
        occuO_h->Write();
        NoccuE_h->Write();
        NoccuO_h->Write();
        NoccuE_pat10_h->Write();
        NoccuO_pat10_h->Write();
        NoccuE_pat9_h->Write();
        NoccuO_pat9_h->Write();
        NoccuE_pat8_h->Write();
        NoccuO_pat8_h->Write();
        NoccuE_pat7_h->Write();
        NoccuO_pat7_h->Write();
        NoccuE_pat6_h->Write();
        NoccuO_pat6_h->Write();
        NoccuE_pat5_h->Write();
        NoccuO_pat5_h->Write();
        NoccuE_pat4_h->Write();
        NoccuO_pat4_h->Write();
        NoccuE_pat3_h->Write();
        NoccuO_pat3_h->Write();
        NoccuE_pat2_h->Write();
        NoccuO_pat2_h->Write();
        rhmHS_pme1aE_h->Write();
        rhmHS_pme1aO_h->Write();
        rhmHS_pme1bE_h->Write();
        rhmHS_pme1bO_h->Write();
        rhmHS_mme1aE_h->Write();
        rhmHS_mme1aO_h->Write();
        rhmHS_mme1bE_h->Write();
        rhmHS_mme1bO_h->Write();
        rhmHS_me12_h->Write();
        rhmHS_me13_h->Write();
        rhmHS_me21_h->Write();
        rhmHS_me22_h->Write();
        rhmHS_me31_h->Write();
        rhmHS_me32_h->Write();
        rhmHS_me41_h->Write();
        rhmHS_me42_h->Write();
        rhHS_h->Write();
        rhHS_rh_h->Write();
        rhmHS_h->Write();
        rhmHS_me11_h->Write();
        rhmHS_rh_h->Write();
        rhmHS_lQ_h->Write();
        rhmHS_mQ_h->Write();
        rhmHS_hQ_h->Write();
        rhmHSb_h->Write();
        rhmHSchID_h->Write();

        myF->Write();
        myF->Close();

        cout << "Nrh: " << Nrh << " Ncomp: " << Ncomp << " NcompM: " << NcompM << " NcompM_lE: " << NcompM_lE << endl;

    }
    else if(op)
    {
        //c2->cd();
        //testH->Draw();
    }
}

void CSCDigiTree::DrawBoxes(float x_pos, float y_pos)
{ 
    TLine *l1 = new TLine(x_pos-0.5,y_pos-0.5,x_pos-0.5,y_pos+0.5);
    l1->SetLineWidth(4);
    l1->Draw();
    TLine *l2 = new TLine(x_pos-0.5,y_pos-0.5,x_pos+0.5,y_pos-0.5);
    l2->SetLineWidth(4);
    l2->Draw();
    TLine *l3 = new TLine(x_pos+0.5,y_pos-0.5,x_pos+0.5,y_pos+0.5);
    l3->SetLineWidth(4);
    l3->Draw();
    TLine *l4 = new TLine(x_pos-0.5,y_pos+0.5,x_pos+0.5,y_pos+0.5);
    l4->SetLineWidth(4);
    l4->Draw();
}

void CSCDigiTree::DrawPats(int pid)
{ 
    if(pid == 2)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(1.0,2.0);
        DrawBoxes(2.0,2.0);
        DrawBoxes(3.0,1.0);
        DrawBoxes(4.0,1.0);
        DrawBoxes(5.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(-1.0,4.0);
        DrawBoxes(-2.0,4.0);
        DrawBoxes(-2.0,5.0);
        DrawBoxes(-3.0,5.0);
        DrawBoxes(-4.0,5.0);
        DrawBoxes(-3.0,6.0);
        DrawBoxes(-4.0,6.0);
        DrawBoxes(-5.0,6.0);
    }
    if(pid == 3)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(-1.0,2.0);
        DrawBoxes(-2.0,2.0);
        DrawBoxes(-3.0,1.0);
        DrawBoxes(-4.0,1.0);
        DrawBoxes(-5.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(1.0,4.0);
        DrawBoxes(2.0,4.0);
        DrawBoxes(2.0,5.0);
        DrawBoxes(3.0,5.0);
        DrawBoxes(4.0,5.0);
        DrawBoxes(3.0,6.0);
        DrawBoxes(4.0,6.0);
        DrawBoxes(5.0,6.0);
    }
    if(pid == 4)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(1.0,2.0);
        DrawBoxes(2.0,2.0);
        DrawBoxes(2.0,1.0);
        DrawBoxes(3.0,1.0);
        DrawBoxes(4.0,1.0);
        DrawBoxes(-1.0,4.0);
        DrawBoxes(-2.0,4.0);
        DrawBoxes(-2.0,5.0);
        DrawBoxes(-3.0,5.0);
        DrawBoxes(-4.0,5.0);
        DrawBoxes(-2.0,6.0);
        DrawBoxes(-3.0,6.0);
        DrawBoxes(-4.0,6.0);
    }
    if(pid == 5)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(-1.0,2.0);
        DrawBoxes(-2.0,2.0);
        DrawBoxes(-2.0,1.0);
        DrawBoxes(-3.0,1.0);
        DrawBoxes(-4.0,1.0);
        DrawBoxes(1.0,4.0);
        DrawBoxes(2.0,4.0);
        DrawBoxes(2.0,5.0);
        DrawBoxes(3.0,5.0);
        DrawBoxes(4.0,5.0);
        DrawBoxes(2.0,6.0);
        DrawBoxes(3.0,6.0);
        DrawBoxes(4.0,6.0);
    }
    if(pid == 6)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(0.0,2.0);
        DrawBoxes(1.0,2.0);
        DrawBoxes(1.0,1.0);
        DrawBoxes(2.0,1.0);
        DrawBoxes(3.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(-1.0,4.0);
        DrawBoxes(-1.0,5.0);
        DrawBoxes(-2.0,5.0);
        DrawBoxes(-1.0,6.0);
        DrawBoxes(-2.0,6.0);
        DrawBoxes(-3.0,6.0);
    }
    if(pid == 7)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(0.0,2.0);
        DrawBoxes(-1.0,2.0);
        DrawBoxes(-1.0,1.0);
        DrawBoxes(-2.0,1.0);
        DrawBoxes(-3.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(1.0,4.0);
        DrawBoxes(1.0,5.0);
        DrawBoxes(2.0,5.0);
        DrawBoxes(1.0,6.0);
        DrawBoxes(2.0,6.0);
        DrawBoxes(3.0,6.0);
    }
    if(pid == 8)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(0.0,2.0);
        DrawBoxes(1.0,2.0);
        DrawBoxes(0.0,1.0);
        DrawBoxes(1.0,1.0);
        DrawBoxes(2.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(-1.0,4.0);
        DrawBoxes(0.0,5.0);
        DrawBoxes(-1.0,5.0);
        DrawBoxes(-2.0,5.0);
        DrawBoxes(0.0,6.0);
        DrawBoxes(-1.0,6.0);
        DrawBoxes(-2.0,6.0);
    }
    if(pid == 9)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(0.0,2.0);
        DrawBoxes(-1.0,2.0);
        DrawBoxes(0.0,1.0);
        DrawBoxes(-1.0,1.0);
        DrawBoxes(-2.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(1.0,4.0);
        DrawBoxes(0.0,5.0);
        DrawBoxes(1.0,5.0);
        DrawBoxes(2.0,5.0);
        DrawBoxes(0.0,6.0);
        DrawBoxes(1.0,6.0);
        DrawBoxes(2.0,6.0);
    }
    if(pid == 10)
    {
        DrawBoxes(0.0,3.0);
        DrawBoxes(0.0,2.0);
        DrawBoxes(-1.0,1.0);
        DrawBoxes(0.0,1.0);
        DrawBoxes(1.0,1.0);
        DrawBoxes(0.0,4.0);
        DrawBoxes(0.0,5.0);
        DrawBoxes(1.0,5.0);
        DrawBoxes(-1.0,5.0);
        DrawBoxes(0.0,6.0);
        DrawBoxes(1.0,6.0);
        DrawBoxes(-1.0,6.0);
    }
}
