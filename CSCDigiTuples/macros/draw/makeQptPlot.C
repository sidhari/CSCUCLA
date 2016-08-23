{
    TCanvas c1("c1","c1",1000,1000);

    TGraphAsymmErrors *eff10 = new TGraphAsymmErrors(pt_pmu_2_me12_pid10_h,pt_pmu_2_me12_h);
    eff10->SetName("eff10");

    TGraphAsymmErrors *eff9 = new TGraphAsymmErrors(pt_pmu_2_me12_pid9_h,pt_pmu_2_me12_h);
    eff9->SetName("eff9");

    TGraphAsymmErrors *eff8 = new TGraphAsymmErrors(pt_pmu_2_me12_pid8_h,pt_pmu_2_me12_h);
    eff8->SetName("eff8");

    TGraphAsymmErrors *eff7 = new TGraphAsymmErrors(pt_pmu_2_me12_pid7_h,pt_pmu_2_me12_h);
    eff7->SetName("eff7");

    TGraphAsymmErrors *eff6 = new TGraphAsymmErrors(pt_pmu_2_me12_pid6_h,pt_pmu_2_me12_h);
    eff6->SetName("eff6");

    TGraphAsymmErrors *eff5 = new TGraphAsymmErrors(pt_pmu_2_me12_pid5_h,pt_pmu_2_me12_h);
    eff5->SetName("eff5");

    TGraphAsymmErrors *eff4 = new TGraphAsymmErrors(pt_pmu_2_me12_pid4_h,pt_pmu_2_me12_h);
    eff4->SetName("eff4");

    TGraphAsymmErrors *eff3 = new TGraphAsymmErrors(pt_pmu_2_me12_pid3_h,pt_pmu_2_me12_h);
    eff3->SetName("eff3");

    TGraphAsymmErrors *eff2 = new TGraphAsymmErrors(pt_pmu_2_me12_pid2_h,pt_pmu_2_me12_h);
    eff2->SetName("eff2");

    eff10->SetLineColor(600);
    eff9->SetLineColor(410);
    eff8->SetLineColor(884);
    eff7->SetLineColor(841);
    eff6->SetLineColor(632);
    eff5->SetLineColor(612);
    eff4->SetLineColor(419);
    eff3->SetLineColor(kOrange);
    eff2->SetLineColor(402);

    eff10->SetLineWidth(2);
    eff9->SetLineWidth(2);
    eff8->SetLineWidth(2);
    eff7->SetLineWidth(2);
    eff6->SetLineWidth(2);
    eff5->SetLineWidth(2);
    eff4->SetLineWidth(2);
    eff3->SetLineWidth(2);
    eff2->SetLineWidth(2);

    eff10->Draw("AP");
    eff9->Draw("P,same");
    eff8->Draw("P,same");
    eff7->Draw("P,same");
    eff6->Draw("P,same");
    eff5->Draw("P,same");
    eff4->Draw("P,same");
    eff3->Draw("P,same");
    eff2->Draw("P,same");

    eff10->GetYaxis()->SetRangeUser(0.0,1.0);
    eff10->GetXaxis()->SetRangeUser(0.0,15.0);
    eff10->GetXaxis()->SetTitle("p_{T} [GeV]");
    eff10->SetTitle("#mu^{+} in ME-1/2");

    TLegend *leg = new TLegend(0.7,0.6,0.9,0.9);
    leg->AddEntry("eff10","pid = 10","lep");
    leg->AddEntry("eff9","pid = 9","lep");
    leg->AddEntry("eff8","pid = 8","lep");
    leg->AddEntry("eff7","pid = 7","lep");
    leg->AddEntry("eff6","pid = 6","lep");
    leg->AddEntry("eff5","pid = 5","lep");
    leg->AddEntry("eff4","pid = 4","lep");
    leg->AddEntry("eff3","pid = 3","lep");
    leg->AddEntry("eff2","pid = 2","lep");
    leg->Draw();
    c1.SaveAs("pmu_mem12.png");


};
