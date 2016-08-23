#define CSCDigiTree_cxx
#include "../include/CSCDigiTree.h"
#include "include/HistGetter.h"
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
    HistGetter plotter;

    //TCanvas *c1 = new TCanvas("c1","c1",1750,400);
    //TCanvas *c2 = new TCanvas("c2","c2",850,700);

    plotter.book1D("RHmean_seg_h","RecHit position of layer 3 in Segments;Position [strips]",220,-10,100);
    plotter.book1D("seg_clctDis_h","Distance between Layer 3 RecHit and closest CLCT;Distance [strips]",100,0.0,10.0);

    plotter.book2D("nSeg_nLCT_h",";Number of Segments;Number of LCTs",10,-0.5,9.5,10,-0.5,9.5);
    plotter.book2D("nLCT_ntfLCT_h",";Number of LCTs;Number of tfLCTs",10,-0.5,9.5,10,-0.5,9.5);
    plotter.book1D("nSegMinusNLCT_h",";Number of Segments - Number of LCTs",21,-10.5,10.5);
    plotter.book1D("nLCTMinusNtfLCT_h",";Number of Segments;Number of LCTs",21,-10.5,10.5);
    plotter.book1D("nRHpSeg_h",";Number of Segments;Number of LCTs",7,-0.5,6.5);
    plotter.book1D("nLCTpSeg_h",";Number of Segments;Number of LCTs",7,-0.5,6.5);
    plotter.book1D("pt_seg_h","p_{T} of Muons for all segments",100,0.0,100.0);
    plotter.book1D("pt_0clct_h","p_{T} of Muons for all segments with 0 clcts",100,0.0,100.0);

    plotter.book1D("pt_h","p_{T} of Muons for all LCTs",200,0.0,100.0);
    plotter.book1D("pt_pmu_h","p_{T} of #mu^{+} for all LCTs",200,0.0,100.0);
    plotter.book1D("pt_mmu_h","p_{T} of #mu^{-} for all LCTs",200,0.0,100.0);

    for(int pat = 10; pat >= 2; pat--)
    {
        for(int station = 1; station <= 4; station++)
        {
            for(int ring = 1; ring <= 3; ring++)
            {
                if(station != 1 && ring > 2) continue;
                if(pat == 10) 
                {
                    plotter.book1D(Form("pt_me%i%i_h",station,((ring-1)%3)+1),
                            Form("p_{T} of Muons for all LCTs in ME%i/%i",station,((ring-1)%3)+1),200,0.0,100.0);
                    plotter.book1D(Form("pt_pmu_1_me%i%i_h",station,((ring-1)%3)+1),
                            Form("p_{T} of #mu^{+} for all LCTs in ME%i/%i",station,((ring-1)%3)+1),200,0.0,100.0);
                    plotter.book1D(Form("pt_mmu_1_me%i%i_h",station,((ring-1)%3)+1),
                            Form("p_{T} of #mu^{-} for all LCTs in ME%i/%i",station,((ring-1)%3)+1),200,0.0,100.0);
                    plotter.book1D(Form("pt_pmu_2_me%i%i_h",station,((ring-1)%3)+1),
                            Form("p_{T} of #mu^{+} for all LCTs in ME%i/%i",station,((ring-1)%3)+1),200,0.0,100.0);
                    plotter.book1D(Form("pt_mmu_2_me%i%i_h",station,((ring-1)%3)+1),
                            Form("p_{T} of #mu^{-} for all LCTs in ME%i/%i",station,((ring-1)%3)+1),200,0.0,100.0);
                    plotter.book2D(Form("xSeg_KHS_me%i%i_h",station,((ring-1)%3)+1),
                            ";Segment X Position [strip/2]; Key Half Strip [Strip/2]",226,-0.5,225.5,226,-0.5,225.5);
                    plotter.book1D(Form("xSegMKHS_me%i%i_h",station,((ring-1)%3)+1),";Difference [Strip]",40,-2.0,2.0);
                    plotter.book2D(Form("SegA_spt_me%i%i_h",station,((ring-1)%3)+1),
                            ";EC*q*p_{T} [e*GeV];#frac{dx}{dz} #left[#frac{strips}{layer}#right]",200,-100.0,100.0,40,-2.0,2.0);
                }

                plotter.book1D(Form("pt_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("p_{T} of Muons in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),200,0.0,100.0);
                plotter.book1D(Form("pt_pmu_1_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("p_{T} of Muons in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),200,0.0,100.0);
                plotter.book1D(Form("pt_mmu_1_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("p_{T} of Muons in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),200,0.0,100.0);
                plotter.book1D(Form("pt_pmu_2_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("p_{T} of Muons in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),200,0.0,100.0);
                plotter.book1D(Form("pt_mmu_2_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("p_{T} of Muons in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),200,0.0,100.0);
                plotter.book1D(Form("SegA_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),
                        Form("Segment Angle in ME%i/%i LCTs with pid %i",station,((ring-1)%3)+1,pat),40,-2.0,2.0);
                plotter.book1D(Form("xSegMKHS_me%i%i_pid%i_h",station,((ring-1)%3)+1,pat),";Difference [Strip]",40,-2.0,2.0);
            }
        }
        plotter.book1D(Form("pt_pid%i_h",pat),Form("p_{T} of Muons for LCTs with pid = %i",pat),200,0.0,100.0);
        plotter.book1D(Form("pt_pmu_1_pid%i_h",pat),Form("p_{T} of #mu^{+} for LCTs with pid = %i",pat),200,0.0,100.0);
        plotter.book1D(Form("pt_mmu_1_pid%i_h",pat),Form("p_{T} of #mu^{-} for LCTs with pid = %i",pat),200,0.0,100.0);
        plotter.book1D(Form("pt_pmu_2_pid%i_h",pat),Form("p_{T} of #mu^{+} for LCTs with pid = %i",pat),200,0.0,100.0);
        plotter.book1D(Form("pt_mmu_2_pid%i_h",pat),Form("p_{T} of #mu^{-} for LCTs with pid = %i",pat),200,0.0,100.0);
    }

    plotter.book1D("pid_h","CLCT Pattern ID Occupancy",9,1.5,10.5);
    plotter.book1D("pid13_h","CLCT Pattern ID Occupancy",9,1.5,10.5);
    plotter.book1D("ED_h","RecHit Energy Deposit",150,0.0,1500.0);
    plotter.book1D("ED_cF0_h","RecHit Energy Deposit",150,0.0,1500.0);
    plotter.book1D("ED_cF1_h","RecHit Energy Deposit",150,0.0,1500.0);
    plotter.book1D("MD_cF0_h","RecHit Max ADC",400,0.0,1000.0);
    plotter.book1D("MD_cF1_h","RecHit Max ADC",400,0.0,1000.0);
    plotter.book1D("recM50_phi_h","RecHit Max > 50 muon #phi",128,-3.2,3.2);
    plotter.book1D("recM50_eta_h","RecHit Max > 50 muon #eta",128,-3.2,3.2);
    plotter.book1D("misslcts_phi_h","Missing LCT muon #phi",128,-3.2,3.2);
    plotter.book1D("misslcts_eta_h","Missing LCT muon #eta",128,-3.2,3.2);

    plotter.book2D("occu_h","Occupancy of Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    plotter.book2D("occuE_h","Occupancy of Even Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    plotter.book2D("occuO_h","Occupancy of Odd Half Strips Shifted by CLCT Key Half Strip",21,-10.5,10.5,6,0.5,6.5);
    plotter.book2D("NoccuE_h","Occupancy of Even Half Strips Shifted by CLCT Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_h","Occupancy of Odd Half Strips Shifted by CLCT Key Half Strip",22,-10.5,11.5,7,0.5,7.5);

    plotter.book2D("NoccuE_pat10_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 10) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat10_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 10) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat9_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 9) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat9_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 9) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat8_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 8) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat8_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 8) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat7_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 7) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat7_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 7) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat6_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 6) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat6_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 6) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat5_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 5) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat5_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 5) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat4_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 4) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat4_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 4) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat3_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 3) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat3_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 3) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuE_pat2_h","Occupancy of Even Half Strips Shifted by CLCT (PID = 2) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);
    plotter.book2D("NoccuO_pat2_h","Occupancy of Odd Half Strips Shifted by CLCT (PID = 2) Key Half Strip",22,-10.5,11.5,7,0.5,7.5);

    plotter.book2D("rhHS_h","Trigger Position and RecHit Position",93*2,-0.5,92.5,82*10,0.0,82.0);
    plotter.book2D("rhHS_rh_h","Trigger Position and RecHit Position",93*2,-0.5,92.5,82*10,0.0,82.0);
    plotter.book2D("rhmHSchID_h","Trigger Position minus RecHit Position in Chambers",24,-3,3,600,0.5,600.5);
    plotter.book1D("rhmHS_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me11_h","Trigger Position minus RecHit Position",243,-60.25,60.25);
    plotter.book1D("rhmHS_rh_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_pme1aE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_pme1aO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_pme1bE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_pme1bO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_mme1aE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_mme1aO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_mme1bE_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_mme1bO_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me12_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me13_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me21_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me22_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me31_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me32_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me41_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_me42_h","Trigger Position minus RecHit Position",200,-1.0,1.0);
    plotter.book1D("rhmHS_lQ_h","Trigger Position minus RecHit Position for Low Energy",200,-1.0,1.0);
    plotter.book1D("rhmHS_mQ_h","Trigger Position minus RecHit Position for Medium Energy",200,-1.0,1.0);
    plotter.book1D("rhmHS_hQ_h","Trigger Position minus RecHit Position for High Energy",200,-1.0,1.0);
    plotter.book1D("rhmHSb_h","Chamber Number of Bad Matches",600,0.5,600.5);
    plotter.book1D("misslctid_h","Chambers missing LCTs",600,0.5,600.5);
    plotter.book1D("misstflctid_h","Chambers missing LCTs",600,0.5,600.5);
    plotter.book1D("miss1lct_phi_h","Missing LCT muon #phi",128,-3.2,3.2);
    plotter.book1D("miss1lct_eta_h","Missing LCT muon #eta",128,-3.2,3.2);
    plotter.book1D("miss1tflct_phi_h","Missing tfLCT muon #phi",128,-3.2,3.2);
    plotter.book1D("miss1tflct_eta_h","Missing tfLCT muon #eta",128,-3.2,3.2);
    plotter.book2D("miss1tflct_etaPhi_h","Muon direction for missing tfLCTs;#eta;#phi",64,-3.2,3.2,64,-3.2,3.2);

    //Format histos to look nice when I draw them
    plotter.get2D("occu_h")->SetStats(0);
    plotter.get2D("NoccuE_h")->SetStats(0);
    plotter.get2D("NoccuO_h")->SetStats(0);
    plotter.get2D("NoccuE_pat10_h")->SetStats(0);
    plotter.get2D("NoccuO_pat10_h")->SetStats(0);
    plotter.get2D("NoccuE_pat9_h")->SetStats(0);
    plotter.get2D("NoccuO_pat9_h")->SetStats(0);
    plotter.get2D("NoccuE_pat8_h")->SetStats(0);
    plotter.get2D("NoccuO_pat8_h")->SetStats(0);
    plotter.get2D("NoccuE_pat7_h")->SetStats(0);
    plotter.get2D("NoccuO_pat7_h")->SetStats(0);
    plotter.get2D("NoccuE_pat6_h")->SetStats(0);
    plotter.get2D("NoccuO_pat6_h")->SetStats(0);
    plotter.get2D("NoccuE_pat5_h")->SetStats(0);
    plotter.get2D("NoccuO_pat5_h")->SetStats(0);
    plotter.get2D("NoccuE_pat4_h")->SetStats(0);
    plotter.get2D("NoccuO_pat4_h")->SetStats(0);
    plotter.get2D("NoccuE_pat3_h")->SetStats(0);
    plotter.get2D("NoccuO_pat3_h")->SetStats(0);
    plotter.get2D("NoccuE_pat2_h")->SetStats(0);
    plotter.get2D("NoccuO_pat2_h")->SetStats(0);
    plotter.get2D("occuE_h")->SetStats(0);
    plotter.get2D("occuO_h")->SetStats(0);
    plotter.get2D("rhHS_h")->SetStats(0);
    plotter.get2D("rhmHSchID_h")->SetStats(0);

    plotter.get1D("rhmHS_h")->SetLineWidth(2);
    plotter.get1D("rhmHS_lQ_h")->SetLineWidth(2);
    plotter.get1D("rhmHS_mQ_h")->SetLineWidth(2);
    plotter.get1D("rhmHS_hQ_h")->SetLineWidth(2);

    plotter.get2D("occu_h")->GetXaxis()->SetTitle("Pattern ID");
    plotter.get2D("occu_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat10_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat10_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat9_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat9_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat8_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat8_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat7_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat7_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat6_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat6_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat5_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat5_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat4_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat4_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat3_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat3_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuE_pat2_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("NoccuO_pat2_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("occuE_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("occuO_h")->GetXaxis()->SetTitle("Half Strip");
    plotter.get2D("rhHS_h")->GetXaxis()->SetTitle("Comparator Position / 0.5 Strips");
    plotter.get1D("rhmHS_h")->GetXaxis()->SetTitle("Difference [Strips]");
    plotter.get1D("rhmHS_lQ_h")->GetXaxis()->SetTitle("Difference [Strips]");
    plotter.get1D("rhmHS_mQ_h")->GetXaxis()->SetTitle("Difference [Strips]");
    plotter.get1D("rhmHS_hQ_h")->GetXaxis()->SetTitle("Difference [Strips]");
    plotter.get1D("rhmHSb_h")->GetXaxis()->SetTitle("Chamber Number");
    plotter.get2D("rhmHSchID_h")->GetXaxis()->SetTitle("Difference [Strips]");

    float titO = 1.5;
    plotter.get2D("occu_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat10_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat10_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat9_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat9_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat8_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat8_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat7_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat7_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat6_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat6_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat5_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat5_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat4_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat4_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat3_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat3_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuE_pat2_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("NoccuO_pat2_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("occuE_h")->GetXaxis()->SetTitleOffset(titO);
    plotter.get2D("occuO_h")->GetXaxis()->SetTitleOffset(titO);

    plotter.get1D("rhmHS_h")->GetYaxis()->SetTitleOffset(1.45);
    plotter.get1D("rhmHS_lQ_h")->GetYaxis()->SetTitleOffset(1.45);
    plotter.get1D("rhmHS_mQ_h")->GetYaxis()->SetTitleOffset(1.45);
    plotter.get1D("rhmHS_hQ_h")->GetYaxis()->SetTitleOffset(1.45);
    plotter.get2D("rhmHSchID_h")->GetYaxis()->SetTitleOffset(1.4);

    plotter.get2D("occu_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat10_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat10_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat9_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat9_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat8_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat8_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat7_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat7_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat6_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat6_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat5_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat5_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat4_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat4_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat3_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat3_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuE_pat2_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("NoccuO_pat2_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("occuE_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("occuO_h")->GetYaxis()->SetTitle("Layer");
    plotter.get2D("rhHS_h")->GetYaxis()->SetTitle("RecHit Position / 0.1 Strips");
    plotter.get1D("rhmHS_h")->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    plotter.get1D("rhmHS_lQ_h")->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    plotter.get1D("rhmHS_mQ_h")->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    plotter.get1D("rhmHS_hQ_h")->GetYaxis()->SetTitle("Entries / 0.01 Strips");
    plotter.get1D("rhmHSb_h")->GetYaxis()->SetTitle("Entries");
    plotter.get2D("rhmHSchID_h")->GetYaxis()->SetTitle("Chamber Number");

    plotter.get2D("rhHS_h")->GetXaxis()->CenterTitle();
    plotter.get2D("occu_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat10_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat10_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat9_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat9_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat8_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat8_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat7_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat7_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat6_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat6_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat5_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat5_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat4_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat4_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat3_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat3_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat2_h")->GetXaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat2_h")->GetXaxis()->CenterTitle();
    plotter.get2D("occuE_h")->GetXaxis()->CenterTitle();
    plotter.get2D("occuO_h")->GetXaxis()->CenterTitle();
    plotter.get2D("rhHS_h")->GetXaxis()->CenterTitle();
    plotter.get1D("rhmHS_h")->GetXaxis()->CenterTitle();
    plotter.get1D("rhmHS_lQ_h")->GetXaxis()->CenterTitle();
    plotter.get1D("rhmHS_mQ_h")->GetXaxis()->CenterTitle();
    plotter.get1D("rhmHS_hQ_h")->GetXaxis()->CenterTitle();
    plotter.get1D("rhmHSb_h")->GetXaxis()->CenterTitle();
    plotter.get2D("rhmHSchID_h")->GetXaxis()->CenterTitle();

    plotter.get2D("rhHS_h")->GetYaxis()->CenterTitle();
    plotter.get2D("occu_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat10_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat10_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat9_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat9_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat8_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat8_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat7_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat7_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat6_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat6_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat5_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat5_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat4_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat4_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat3_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat3_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuE_pat2_h")->GetYaxis()->CenterTitle();
    plotter.get2D("NoccuO_pat2_h")->GetYaxis()->CenterTitle();
    plotter.get2D("occuE_h")->GetYaxis()->CenterTitle();
    plotter.get2D("occuO_h")->GetYaxis()->CenterTitle();
    plotter.get2D("rhHS_h")->GetYaxis()->CenterTitle();
    plotter.get1D("rhmHS_h")->GetYaxis()->CenterTitle();
    plotter.get1D("rhmHS_lQ_h")->GetYaxis()->CenterTitle();
    plotter.get1D("rhmHS_mQ_h")->GetYaxis()->CenterTitle();
    plotter.get1D("rhmHS_hQ_h")->GetYaxis()->CenterTitle();
    plotter.get1D("rhmHSb_h")->GetYaxis()->CenterTitle();
    plotter.get2D("rhmHSchID_h")->GetYaxis()->CenterTitle();

    float labelS = 0.08;
    plotter.get2D("occu_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat10_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat10_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat9_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat9_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat8_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat8_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat7_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat7_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat6_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat6_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat5_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat5_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat4_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat4_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat3_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat3_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat2_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat2_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("occuE_h")->GetXaxis()->SetLabelSize(labelS);
    plotter.get2D("occuO_h")->GetXaxis()->SetLabelSize(labelS);

    labelS = 0.11;
    plotter.get2D("occu_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat10_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat10_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat9_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat9_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat8_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat8_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat7_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat7_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat6_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat6_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat5_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat5_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat4_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat4_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat3_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat3_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat2_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat2_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("occuE_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("occuO_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get1D("rhmHS_h")->GetYaxis()->SetLabelSize(0.03);
    plotter.get1D("rhmHS_lQ_h")->GetYaxis()->SetLabelSize(0.03);
    plotter.get1D("rhmHS_mQ_h")->GetYaxis()->SetLabelSize(0.03);
    plotter.get1D("rhmHS_hQ_h")->GetYaxis()->SetLabelSize(0.03);

    labelS = 0.08;
    plotter.get2D("occu_h")->GetZaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat10_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat10_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat9_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat9_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat8_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat8_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat7_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat7_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat6_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat6_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat5_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat5_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat4_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat4_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat3_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat3_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuE_pat2_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("NoccuO_pat2_h")->GetYaxis()->SetLabelSize(labelS);
    plotter.get2D("occuE_h")->GetZaxis()->SetLabelSize(labelS);
    plotter.get2D("occuO_h")->GetZaxis()->SetLabelSize(labelS);

    int mCol = 2;
    plotter.get2D("occu_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat10_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat10_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat9_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat9_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat8_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat8_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat7_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat7_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat6_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat6_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat5_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat5_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat4_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat4_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat3_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat3_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuE_pat2_h")->SetMarkerColor(mCol);
    plotter.get2D("NoccuO_pat2_h")->SetMarkerColor(mCol);
    plotter.get2D("occuE_h")->SetMarkerColor(mCol);
    plotter.get2D("occuO_h")->SetMarkerColor(mCol);

    float mS = 1.8;
    plotter.get2D("occu_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat10_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat10_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat9_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat9_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat8_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat8_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat7_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat7_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat6_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat6_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat5_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat5_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat4_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat4_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat3_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat3_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuE_pat2_h")->SetMarkerSize(mS);
    plotter.get2D("NoccuO_pat2_h")->SetMarkerSize(mS);
    plotter.get2D("occuE_h")->SetMarkerSize(mS);
    plotter.get2D("occuO_h")->SetMarkerSize(mS);

    plotter.get2D("NoccuE_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat10_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat10_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat10_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat10_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat9_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat9_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat9_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat9_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat8_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat8_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat8_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat8_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat7_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat7_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat7_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat7_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat6_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat6_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat6_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat6_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat5_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat5_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat5_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat5_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat4_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat4_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat4_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat4_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat3_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat3_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat3_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat3_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuE_pat2_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuE_pat2_h")->GetYaxis()->SetBinLabel(7,"#Sigma");
    plotter.get2D("NoccuO_pat2_h")->GetXaxis()->SetBinLabel(22,"#Sigma");
    plotter.get2D("NoccuO_pat2_h")->GetYaxis()->SetBinLabel(7,"#Sigma");

    int Nrh = 0;
    int Ncomp = 0;
    int NcompM = 0;
    int NcompM_lE = 0;

    for(int iy = 1; iy < 7; iy++)
    {
        plotter.get2D("occu_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat10_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat10_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat9_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat9_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat8_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat8_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat7_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat7_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat6_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat6_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat5_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat5_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat4_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat4_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat3_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat3_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuE_pat2_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
        plotter.get2D("NoccuO_pat2_h")->GetYaxis()->SetBinLabel(iy,Form("%i",iy));
    }

    for(int ix = 1; ix < 23; ix+=5)
    {
        plotter.get2D("occu_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat10_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat10_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat9_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat9_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat8_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat8_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat7_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat7_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat6_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat6_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat5_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat5_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat4_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat4_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat3_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat3_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuE_pat2_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
        plotter.get2D("NoccuO_pat2_h")->GetXaxis()->SetBinLabel(ix,Form("%i",ix - 11));
    }
    TH1D *testH = new TH1D("testH","testH",11,-0.5,10.5);

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries/1;jentry++) 
    {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;

        if(jentry%(nentries/100) == 0) cout << "Loading event " << jentry << " out of " << nentries << endl;
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
                float RHmean_seg = -9;
                float RHsum = 0;
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
                    RHsum += recPos;
                    float minD = 999.0;
                    float fCompP = 999.0;
                    bool compFound = false;
                    for(int icomp = 0; icomp < int(compId->size()); icomp++)
                    {
                        if(iID != compId->at(icomp) || iLay != compLay->at(icomp)) continue;
                        if(compStr->at(icomp).size() == 0) continue;
                        for(int istr = 0; istr < int(compStr->at(icomp).size()); istr++)
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
                        plotter.get1D("ED_cF0_h")->Fill(recE);
                        plotter.get1D("MD_cF0_h")->Fill(recM);
                        NcompM++; 
                        if(recE < 200.0) NcompM_lE++; 
                        if(recM > 50.0)
                        {
                            plotter.get1D("recM50_phi_h")->Fill(phi);
                            plotter.get1D("recM50_eta_h")->Fill(eta);
                            //cout << "Missing Comparator above threshold: " << jentry << endl;
                            //cout << "Run Number: " << Event_RunNumber << endl;
                            //cout << "Event Number: " << Event_EventNumber << endl;
                            //cout << "Lumi Section: " << Event_LumiSection << endl;
                            //cout << "Bunch Crossing: " << Event_BXCrossing << endl << endl;
                        }
                        continue;
                    }

                    if(EC==1 && ST==1 && RI==4 && CH%2==0) plotter.get1D("rhmHS_pme1aE_h")->Fill(minD);
                    if(EC==1 && ST==1 && RI==4 && CH%2==1) plotter.get1D("rhmHS_pme1aO_h")->Fill(minD);
                    if(EC==1 && ST==1 && RI==1 && CH%2==0) plotter.get1D("rhmHS_pme1bE_h")->Fill(minD);
                    if(EC==1 && ST==1 && RI==1 && CH%2==1) plotter.get1D("rhmHS_pme1bO_h")->Fill(minD);
                    if(EC==2 && ST==1 && RI==4 && CH%2==0) plotter.get1D("rhmHS_mme1aE_h")->Fill(minD);
                    if(EC==2 && ST==1 && RI==4 && CH%2==1) plotter.get1D("rhmHS_mme1aO_h")->Fill(minD);
                    if(EC==2 && ST==1 && RI==1 && CH%2==0) plotter.get1D("rhmHS_mme1bE_h")->Fill(minD);
                    if(EC==2 && ST==1 && RI==1 && CH%2==1) plotter.get1D("rhmHS_mme1bO_h")->Fill(minD);
                    if(ST==1 && RI==2)                     plotter.get1D("rhmHS_me12_h")->Fill(minD);
                    if(ST==1 && RI==3)                     plotter.get1D("rhmHS_me13_h")->Fill(minD);
                    if(ST==2 && RI==1)                     plotter.get1D("rhmHS_me21_h")->Fill(minD);
                    if(ST==2 && RI==2)                     plotter.get1D("rhmHS_me22_h")->Fill(minD);
                    if(ST==3 && RI==1)                     plotter.get1D("rhmHS_me31_h")->Fill(minD);
                    if(ST==3 && RI==2)                     plotter.get1D("rhmHS_me32_h")->Fill(minD);
                    if(ST==4 && RI==1)                     plotter.get1D("rhmHS_me41_h")->Fill(minD);
                    if(ST==4 && RI==2)                     plotter.get1D("rhmHS_me42_h")->Fill(minD);

                    Ncomp++;
                    plotter.get1D("ED_cF1_h")->Fill(rhE->at(irh));
                    plotter.get1D("MD_cF1_h")->Fill(rhMax->at(irh));
                    plotter.get2D("rhHS_rh_h")->Fill(fCompP,recPos);
                    if((EC==1 && me1a)||(EC==2 && me1b)) plotter.get1D("rhmHS_rh_h")->Fill(-1.0*minD);
                    else plotter.get1D("rhmHS_rh_h")->Fill(minD);
                }
                RHmean_seg = RHsum/float(Nrh_seg);
                plotter.get1D("RHmean_seg_h")->Fill(RHmean_seg);

                //if (me11) continue;

                int Nclct = 0;
                int pid = -9;
                int khs = -9;
                int Nlct_seg = 0;
                float seg_clctDis = 999.9;
                plotter.get1D("pt_seg_h")->Fill(Pt);

                //Count LCTs
                for(int ilct = 0; ilct < (int) lctId->size(); ilct++)
                {
                    if(!(chSid == lctId->at(ilct))) continue;
                    for(int jlct = 0; jlct < int(lctQ->at(ilct).size()); jlct++)
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
                    for(int jtflct = 0; jtflct < int(tflctQ->at(itflct).size()); jtflct++)
                    {
                        Ntflct++;
                        Ntflct_seg++;
                    }
                }

                //Look for a CLCT
                for(int iclct = 0; iclct < (int) clctId->size(); iclct++)
                {
                    if(!(chSid == clctId->at(iclct))) continue;
                    Nclct++;
                    for(int jclct = 0; jclct < int(clctQ->at(iclct).size()); jclct++)
                    {
                        int KHS = 32*clctCFEB->at(iclct).at(jclct)+clctKHS->at(iclct).at(jclct);
                        plotter.get1D("pid_h")->Fill(clctPat->at(iclct).at(jclct));
                        if(fabs( ( (KHS/2.0) + 0.75 ) - RHmean_seg ) < seg_clctDis )
                        {
                            seg_clctDis = fabs( ( (KHS/2.0) + 0.75 ) - RHmean_seg );
                            pid = clctPat->at(iclct).at(jclct);
                            khs = KHS;
                        }

                        if(ST==1 && RI==3 && Pt < 5.0) plotter.get1D("pid13_h")->Fill(clctPat->at(iclct).at(jclct));
                        //Find Comparators for this clct
                        for(int icomp = 0; icomp < int(compId->size()); icomp++)
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
                            plotter.get2D("occu_h")->Fill(minD,layN);
                            float recPos = -99.9;
                            float recE = -999.0;
                            for(int irh = 0; irh < int(rhId->size()); irh++)
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
                            plotter.get2D("rhHS_h")->Fill(compPos,recPos);
                            if(recE > 0.0) plotter.get1D("rhmHS_h")->Fill(compPos - recPos);
                            if(recE > 0.0 && me11) plotter.get1D("rhmHS_me11_h")->Fill(compPos - recPos);
                            if(recE > 0.0 && recE < 200.0) plotter.get1D("rhmHS_lQ_h")->Fill(compPos - recPos);
                            if(recE > 200.0 && recE < 500.0) plotter.get1D("rhmHS_mQ_h")->Fill(compPos - recPos);
                            if(recE > 500.0) plotter.get1D("rhmHS_hQ_h")->Fill(compPos - recPos);
                            if(fabs(compPos - recPos) > 0.5) plotter.get1D("rhmHSb_h")->Fill(chSid);
                            plotter.get2D("rhmHSchID_h")->Fill(compPos - recPos,chSid);
                            if(recE) plotter.get1D("ED_h")->Fill(recE);

                            if(minD > 10 || minD < -10) continue;
                            if(KHS%2==1)
                            {
                                plotter.get2D("occuO_h")->Fill(minD,layN);
                                plotter.get2D("NoccuO_h")->Fill(minD,layN);
                                plotter.get2D("NoccuO_h")->Fill(minD,7);
                                plotter.get2D("NoccuO_h")->Fill(11,layN);
                                if(clctPat->at(iclct).at(jclct) == 10)
                                {
                                    plotter.get2D("NoccuO_pat10_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat10_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat10_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 9)
                                {
                                    plotter.get2D("NoccuO_pat9_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat9_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat9_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 8)
                                {
                                    plotter.get2D("NoccuO_pat8_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat8_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat8_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 7)
                                {
                                    plotter.get2D("NoccuO_pat7_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat7_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat7_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 6)
                                {
                                    plotter.get2D("NoccuO_pat6_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat6_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat6_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 5)
                                {
                                    plotter.get2D("NoccuO_pat5_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat5_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat5_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 4)
                                {
                                    plotter.get2D("NoccuO_pat4_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat4_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat4_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 3)
                                {
                                    plotter.get2D("NoccuO_pat3_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat3_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat3_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 2)
                                {
                                    plotter.get2D("NoccuO_pat2_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuO_pat2_h")->Fill(minD,7);
                                    plotter.get2D("NoccuO_pat2_h")->Fill(11,layN);
                                }
                            }
                            else 
                            {
                                plotter.get2D("occuE_h")->Fill(minD,layN);
                                plotter.get2D("NoccuE_h")->Fill(minD,layN);
                                plotter.get2D("NoccuE_h")->Fill(minD,7);
                                plotter.get2D("NoccuE_h")->Fill(11,layN);
                                if(clctPat->at(iclct).at(jclct) == 10)
                                {
                                    plotter.get2D("NoccuE_pat10_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat10_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat10_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 9)
                                {
                                    plotter.get2D("NoccuE_pat9_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat9_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat9_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 8)
                                {
                                    plotter.get2D("NoccuE_pat8_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat8_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat8_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 7)
                                {
                                    plotter.get2D("NoccuE_pat7_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat7_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat7_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 6)
                                {
                                    plotter.get2D("NoccuE_pat6_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat6_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat6_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 5)
                                {
                                    plotter.get2D("NoccuE_pat5_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat5_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat5_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 4)
                                {
                                    plotter.get2D("NoccuE_pat4_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat4_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat4_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 3)
                                {
                                    plotter.get2D("NoccuE_pat3_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat3_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat3_h")->Fill(11,layN);
                                }
                                if(clctPat->at(iclct).at(jclct) == 2)
                                {
                                    plotter.get2D("NoccuE_pat2_h")->Fill(minD,layN);
                                    plotter.get2D("NoccuE_pat2_h")->Fill(minD,7);
                                    plotter.get2D("NoccuE_pat2_h")->Fill(11,layN);
                                }
                            }
                        }//icomp
                    }
                }//iclct
                //if(Nrh_seg == 4 && Nlct_seg == 0) cout << "Show event " << jentry << endl;
                plotter.get1D("nRHpSeg_h")->Fill(Nrh_seg);
                plotter.get1D("nLCTpSeg_h")->Fill(Nlct_seg);
                if(Nlct_seg == 0 || Ntflct_seg == 0) missID = chSid;
                //if(Nrh_seg != 4) continue;
                plotter.get1D("seg_clctDis_h")->Fill(seg_clctDis);

                //Fill Segment info
                plotter.get2D(Form("xSeg_KHS_me%i%i_h",ST,((RI-1)%3)+1))->Fill(2.0*segX->at(iseg),khs);
                plotter.get1D(Form("xSegMKHS_me%i%i_h",ST,((RI-1)%3)+1))->Fill(segX->at(iseg) - float(khs + 0.5)/2.0);
                plotter.get2D(Form("SegA_spt_me%i%i_h",ST,((RI-1)%3)+1))->Fill(float(-2*EC+3)*float(q)*Pt,segdXdZ->at(iseg));

                //Fill Numerators
                plotter.get1D("pt_h")->Fill(Pt);
                if(q == 1) 
                {
                    plotter.get1D("pt_pmu_h")->Fill(Pt);
                    plotter.get1D(Form("pt_pmu_%i_me%i%i_h",EC,ST,((RI-1)%3)+1))->Fill(Pt);
                }
                if(q == -1) 
                {
                    plotter.get1D("pt_mmu_h")->Fill(Pt);
                    plotter.get1D(Form("pt_mmu_%i_me%i%i_h",EC,ST,((RI-1)%3)+1))->Fill(Pt);
                }

                plotter.get1D(Form("pt_me%i%i_h",ST,((RI-1)%3)+1))->Fill(Pt);
                if(Nclct == 0) plotter.get1D("pt_0clct_h")->Fill(Pt);

                //Check to see if there was an LCT and Fill Denominators
                if(pid == -9) continue;

                plotter.get1D(Form("SegA_me%i%i_pid%i_h",ST,((RI-1)%3)+1,pid))->Fill(segdXdZ->at(iseg));
                plotter.get1D(Form("xSegMKHS_me%i%i_pid%i_h",ST,((RI-1)%3)+1,pid))->Fill(segX->at(iseg) - float(khs + 0.5)/2.0);

                if(q==1) 
                {
                    plotter.get1D(Form("pt_pmu_%i_pid%i_h",EC,pid))->Fill(Pt);
                    plotter.get1D(Form("pt_pmu_%i_me%i%i_pid%i_h",EC,ST,((RI-1)%3)+1,pid))->Fill(Pt);
                }

                if(q==-1) 
                {
                    plotter.get1D(Form("pt_mmu_%i_pid%i_h",EC,pid))->Fill(Pt);
                    plotter.get1D(Form("pt_mmu_%i_me%i%i_pid%i_h",EC,ST,((RI-1)%3)+1,pid))->Fill(Pt);
                }

                plotter.get1D(Form("pt_me%i%i_pid%i_h",ST,((RI-1)%3)+1,pid))->Fill(Pt);

                plotter.get1D(Form("pt_pid%i_h",pid))->Fill(Pt);
            }//iseg
            plotter.get2D("nLCT_ntfLCT_h")->Fill(Nlct,Ntflct);
            if(Nlct != 0 && Nlct - Ntflct == 1) 
            { 
                plotter.get1D("misstflctid_h")->Fill(missID); 
                plotter.get1D("miss1tflct_phi_h")->Fill(phi); 
                plotter.get1D("miss1tflct_eta_h")->Fill(eta); 
                plotter.get2D("miss1tflct_etaPhi_h")->Fill(eta,phi);
            }
            if(Nlct != 0 && Nlct - Ntflct == -1) { plotter.get1D("misslctid_h")->Fill(missID); plotter.get1D("miss1lct_phi_h")->Fill(phi); plotter.get1D("miss1lct_eta_h")->Fill(eta); }
            if(Nlct == 0 && Ntflct > 0) { plotter.get1D("misslcts_phi_h")->Fill(phi); plotter.get1D("misslcts_eta_h")->Fill(eta); }
            plotter.get2D("nSeg_nLCT_h")->Fill(Nseg,Nlct);
            plotter.get1D("nSegMinusNLCT_h")->Fill(Nseg-Nlct);
            plotter.get1D("nLCTMinusNtfLCT_h")->Fill(Nlct-Ntflct);
        }//option if
    }

    for(int ix = 1; ix < 22; ix++)
    {
        for(int iy = 1; iy < 7; iy++)
        {
            plotter.get2D("NoccuO_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat10_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat10_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat10_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat10_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat10_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat10_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat9_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat9_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat9_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat9_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat9_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat9_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat8_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat8_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat8_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat8_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat8_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat8_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat7_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat7_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat7_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat7_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat7_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat7_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat6_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat6_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat6_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat6_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat6_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat6_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat5_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat5_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat5_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat5_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat5_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat5_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat4_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat4_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat4_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat4_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat4_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat4_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat3_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat3_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat3_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat3_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat3_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat3_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuO_pat2_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuO_pat2_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuO_pat2_h")->GetBinContent(22,iy)))/10.0);
            plotter.get2D("NoccuE_pat2_h")->SetBinContent(ix,iy,float(int(1000.0*plotter.get2D("NoccuE_pat2_h")->GetBinContent(ix,iy)/plotter.get2D("NoccuE_pat2_h")->GetBinContent(22,iy)))/10.0);
        }
    }




    plotter.get2D("NoccuO_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat10_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat10_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat9_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat9_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat8_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat8_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat7_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat7_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat6_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat6_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat5_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat5_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat4_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat4_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat3_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat3_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuO_pat2_h")->SetAxisRange(0.0,100.0,"Z");
    plotter.get2D("NoccuE_pat2_h")->SetAxisRange(0.0,100.0,"Z");

    TLine *l1 = new TLine(10.5,0.5,10.5,6.5);
    l1->SetLineWidth(3);
    TLine *l2 = new TLine(-10.5,6.5,10.5,6.5);
    l2->SetLineWidth(3);

    if(!op)
    {

        /*c1->cd();
          plotter.get2D("occu_h")->Draw("colz text");
          c1->SaveAs(Form("../%s/plots/occu.png",sName.c_str()));

          plotter.get2D("occuE_h")->Draw("colz text");
          c1->SaveAs(Form("../%s/plots/occuEven.png",sName.c_str()));

          plotter.get2D("occuO_h")->Draw("colz text");
          c1->SaveAs(Form("../%s/plots/occuOdd.png",sName.c_str()));

          plotter.get2D("NoccuE_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          c1->SaveAs(Form("../%s/plots/NoccuEven.png",sName.c_str()));

          plotter.get2D("NoccuO_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          c1->SaveAs(Form("../%s/plots/NoccuOdd.png",sName.c_str()));

          plotter.get2D("NoccuE_pat10_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(10);
          c1->SaveAs(Form("../%s/plots/NoccuEven_pat10.png",sName.c_str()));
          plotter.get2D("NoccuO_pat10_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(10);
          c1->SaveAs(Form("../%s/plots/NoccuOdd_pat10.png",sName.c_str()));
          plotter.get2D("NoccuE_pat9_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(9);
          c1->SaveAs(Form("../%s/plots/NoccuEven_pat9.png",sName.c_str()));
          plotter.get2D("NoccuO_pat9_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(9);
          c1->SaveAs(Form("../%s/plots/NoccuOdd_pat9.png",sName.c_str()));
          plotter.get2D("NoccuE_pat8_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(8);
          c1->SaveAs(Form("../%s/plots/NoccuEven_pat8.png",sName.c_str()));
          plotter.get2D("NoccuO_pat8_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(8);
          c1->SaveAs(Form("../%s/plots/NoccuOdd_pat8.png",sName.c_str()));
          plotter.get2D("NoccuE_pat7_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(7);
          c1->SaveAs(Form("../%s/plots/NoccuEven_pat7.png",sName.c_str()));
          plotter.get2D("NoccuO_pat7_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(7);
          c1->SaveAs(Form("../%s/plots/NoccuOdd_pat7.png",sName.c_str()));
          plotter.get2D("NoccuE_pat6_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(6);
          c1->SaveAs(Form("../%s/plots/NoccuEven_pat6.png",sName.c_str()));
          plotter.get2D("NoccuO_pat6_h")->Draw("colz text");
          l1->Draw();
          l2->Draw();
          DrawPats(6);
          c1->SaveAs(Form("../%s/plots/NoccuOdd_pat6.png",sName.c_str()));
          plotter.get2D("NoccuE_pat5_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(5);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat5.png",sName.c_str()));
        plotter.get2D("NoccuO_pat5_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(5);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat5.png",sName.c_str()));
        plotter.get2D("NoccuE_pat4_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(4);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat4.png",sName.c_str()));
        plotter.get2D("NoccuO_pat4_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(4);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat4.png",sName.c_str()));
        plotter.get2D("NoccuE_pat3_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(3);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat3.png",sName.c_str()));
        plotter.get2D("NoccuO_pat3_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(3);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat3.png",sName.c_str()));
        plotter.get2D("NoccuE_pat2_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(2);
        c1->SaveAs(Form("../%s/plots/NoccuEven_pat2.png",sName.c_str()));
        plotter.get2D("NoccuO_pat2_h")->Draw("colz text");
        l1->Draw();
        l2->Draw();
        DrawPats(2);
        c1->SaveAs(Form("../%s/plots/NoccuOdd_pat2.png",sName.c_str()));

        c2->cd();

        plotter.get2D("rhmHSchID_h")->Draw("colz");
        c2->SaveAs(Form("../%s/plots/rhmHSchID.png",sName.c_str()));

        plotter.get1D("rhmHSb_h")->Draw();
        c2->SaveAs(Form("../%s/plots/rhHSb.png",sName.c_str()));

        plotter.get2D("rhHS_h")->Draw("colz");
        c2->SaveAs(Form("../%s/plots/rhHS.png",sName.c_str()));

        plotter.get1D("rhmHS_h")->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS.png",sName.c_str()));

        plotter.get1D("rhmHS_lQ_h")->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_lQ.png",sName.c_str()));

        plotter.get1D("rhmHS_mQ_h")->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_mQ.png",sName.c_str()));

        plotter.get1D("rhmHS_hQ_h")->Draw();
        c2->SaveAs(Form("../%s/plots/rhmHS_hQ.png",sName.c_str()));*/



            plotter.write(Form("%sCSCDigiTreeAna.root",sName.c_str()));

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
