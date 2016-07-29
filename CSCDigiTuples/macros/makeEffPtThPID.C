{
    TCanvas c1;

    TH1D *pt_th8_h = new TH1D("pt_th8_h","pt_th8_h",200,0.0,100.0);
    pt_th8_h->Add(pt_pid9_h,pt_pid8_h);
    pt_th8_h->Add(pt_pid10_h);

    TH1D *pt_th6_h = new TH1D("pt_th6_h","pt_th6_h",200,0.0,100.0);
    pt_th6_h->Add(pt_th8_h,pt_pid7_h);
    pt_th6_h->Add(pt_pid6_h);

    TH1D *pt_th4_h = new TH1D("pt_th4_h","pt_th4_h",200,0.0,100.0);
    pt_th4_h->Add(pt_th6_h,pt_pid5_h);
    pt_th4_h->Add(pt_pid4_h);

    TH1D *pt_th2_h = new TH1D("pt_th2_h","pt_th2_h",200,0.0,100.0);
    pt_th2_h->Add(pt_th4_h,pt_pid3_h);
    pt_th2_h->Add(pt_pid2_h);

    TGraphAsymmErrors *eff10 = new TGraphAsymmErrors(pt_pid10_h,pt_h);
    eff10->SetName("eff10");
    TGraphAsymmErrors *eff8 = new TGraphAsymmErrors(pt_th8_h,pt_h);
    eff8->SetName("eff8");
    TGraphAsymmErrors *eff6 = new TGraphAsymmErrors(pt_th6_h,pt_h);
    eff6->SetName("eff6");
    TGraphAsymmErrors *eff4 = new TGraphAsymmErrors(pt_th4_h,pt_h);
    eff4->SetName("eff4");
    TGraphAsymmErrors *eff2 = new TGraphAsymmErrors(pt_th2_h,pt_h);
    eff2->SetName("eff2");

    eff10->SetLineColor(600);
    eff8->SetLineColor(884);
    eff6->SetLineColor(841);
    eff4->SetLineColor(419);
    eff2->SetLineColor(402);

    eff10->SetLineWidth(2);
    eff8->SetLineWidth(2);
    eff6->SetLineWidth(2);
    eff4->SetLineWidth(2);
    eff2->SetLineWidth(2);

    eff10->Draw("");
    eff8->Draw("same");
    eff6->Draw("same");
    eff4->Draw("same");
    eff2->Draw("same");

    //eff10->GetYaxis()->SetRangeUser(0.8,1.0);
    //eff10->GetXaxis()->SetRangeUser(0.0,15.0);
    eff10->SetTitle("");

    TLegend *leg = new TLegend(0.7,0.1,0.9,0.4);
    leg->AddEntry("eff10","pid = 10","lep");
    leg->AddEntry("eff8","pid > 7","lep");
    leg->AddEntry("eff6","pid > 5","lep");
    leg->AddEntry("eff4","pid > 3","lep");
    leg->AddEntry("eff2","pid > 1","lep");
    leg->Draw();


};
