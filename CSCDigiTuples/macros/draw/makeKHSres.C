{
    for(int ST = 1; ST <= 4; ST++)
    {
        for(int RI = 1; RI <= 3; RI++)
        {
            if(ST > 1 && RI > 2) continue;
            TCanvas c1("c1","c1",1000,1200);
            c1.cd();

            THStack myS("myS",Form("KHS Resolution in ME%i/%i",ST,RI));

            TFile myF("AnaCSCDigiTree.root");
            TH1F *KHSres10 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid10_h",ST,RI));
            KHSres10->SetName("KHSres10");
            TH1F *KHSres9 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid9_h",ST,RI));
            KHSres9->SetName("KHSres9");
            TH1F *KHSres8 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid8_h",ST,RI));
            KHSres8->SetName("KHSres8");
            TH1F *KHSres7 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid7_h",ST,RI));
            KHSres7->SetName("KHSres7");
            TH1F *KHSres6 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid6_h",ST,RI));
            KHSres6->SetName("KHSres6");
            TH1F *KHSres5 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid5_h",ST,RI));
            KHSres5->SetName("KHSres5");
            TH1F *KHSres4 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid4_h",ST,RI));
            KHSres4->SetName("KHSres4");
            TH1F *KHSres3 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid3_h",ST,RI));
            KHSres3->SetName("KHSres3");
            TH1F *KHSres2 = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid2_h",ST,RI));
            KHSres2->SetName("KHSres2");

            KHSres9->SetStats(0);

            KHSres10->SetLineColor(600);
            KHSres9->SetLineColor(410);
            KHSres8->SetLineColor(884);
            KHSres7->SetLineColor(841);
            KHSres6->SetLineColor(632);
            KHSres5->SetLineColor(612);
            KHSres4->SetLineColor(419);
            KHSres3->SetLineColor(kOrange);
            KHSres2->SetLineColor(402);

            KHSres10->SetFillColor(600);
            KHSres9->SetFillColor(410);
            KHSres8->SetFillColor(884);
            KHSres7->SetFillColor(841);
            KHSres6->SetFillColor(632);
            KHSres5->SetFillColor(612);
            KHSres4->SetFillColor(419);
            KHSres3->SetFillColor(kOrange);
            KHSres2->SetFillColor(402);

            myS.Add(KHSres2);
            myS.Add(KHSres3);
            myS.Add(KHSres4);
            myS.Add(KHSres5);
            myS.Add(KHSres6);
            myS.Add(KHSres7);
            myS.Add(KHSres8);
            myS.Add(KHSres9);
            myS.Add(KHSres10);

            myS.Draw("hist");

            //KHSres10->GetYaxis()->SetRangeUser(0.0,1.0);
            //KHSres10->GetXaxis()->SetRangeUser(-2.0,2.0);
            myS.GetXaxis()->SetTitle("Segment X - KHS [strip]");
            myS.GetXaxis()->CenterTitle();
            myS.GetXaxis()->SetTitleOffset(1.2);
            //KHSres10->SetTitle("Segment Angles");

            TLegend *leg = new TLegend(0.7,0.6,0.9,0.9);
            leg->AddEntry("KHSres10","pid = 10","f");
            leg->AddEntry("KHSres9","pid = 9","f");
            leg->AddEntry("KHSres8","pid = 8","f");
            leg->AddEntry("KHSres7","pid = 7","f");
            leg->AddEntry("KHSres6","pid = 6","f");
            leg->AddEntry("KHSres5","pid = 5","f");
            leg->AddEntry("KHSres4","pid = 4","f");
            leg->AddEntry("KHSres3","pid = 3","f");
            leg->AddEntry("KHSres2","pid = 2","f");
            leg->Draw();
            c1.SaveAs(Form("plots/xSegMKHS_me%i%i.png",ST,RI));
            c1.SetLogy();
            c1.SaveAs(Form("plots/xSegMKHS_me%i%i_logy.png",ST,RI));
        }
    }


};
