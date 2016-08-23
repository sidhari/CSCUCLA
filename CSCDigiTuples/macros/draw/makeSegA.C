{
    
    THStack *st21;
    TLegend *myL;

    for(int ST = 1; ST <= 4; ST++)
    {
        for(int RI = 1; RI <= 3; RI++)
        {
            if(ST > 1 && RI > 2) continue;
            TCanvas c1("c1","c1",1000,1200);
            c1.cd();

            THStack myS("myS",Form("Segment Angles in ME%i/%i",ST,RI));

            TFile myF("AnaCSCDigiTree.root");
            TH1F *segA10 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid10_h",ST,RI));
            segA10->SetName("segA10");
            TH1F *segA9 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid9_h",ST,RI));
            segA9->SetName("segA9");
            TH1F *segA8 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid8_h",ST,RI));
            segA8->SetName("segA8");
            TH1F *segA7 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid7_h",ST,RI));
            segA7->SetName("segA7");
            TH1F *segA6 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid6_h",ST,RI));
            segA6->SetName("segA6");
            TH1F *segA5 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid5_h",ST,RI));
            segA5->SetName("segA5");
            TH1F *segA4 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid4_h",ST,RI));
            segA4->SetName("segA4");
            TH1F *segA3 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid3_h",ST,RI));
            segA3->SetName("segA3");
            TH1F *segA2 = (TH1F*)myF.Get(Form("SegA_me%i%i_pid2_h",ST,RI));
            segA2->SetName("segA2");

            segA9->SetStats(0);

            segA10->SetLineColor(600);
            segA9->SetLineColor(410);
            segA8->SetLineColor(884);
            segA7->SetLineColor(841);
            segA6->SetLineColor(632);
            segA5->SetLineColor(612);
            segA4->SetLineColor(419);
            segA3->SetLineColor(kOrange);
            segA2->SetLineColor(402);

            segA10->SetFillColor(600);
            segA9->SetFillColor(410);
            segA8->SetFillColor(884);
            segA7->SetFillColor(841);
            segA6->SetFillColor(632);
            segA5->SetFillColor(612);
            segA4->SetFillColor(419);
            segA3->SetFillColor(kOrange);
            segA2->SetFillColor(402);

            myS.Add(segA2);
            myS.Add(segA3);
            myS.Add(segA4);
            myS.Add(segA5);
            myS.Add(segA6);
            myS.Add(segA7);
            myS.Add(segA8);
            myS.Add(segA9);
            myS.Add(segA10);

            myS.Draw("hist");

            //segA10->GetYaxis()->SetRangeUser(0.0,1.0);
            //segA10->GetXaxis()->SetRangeUser(-2.0,2.0);
            myS.GetXaxis()->SetTitle("#frac{dx}{dz} of Segment #left[#frac{strip}{layer}#right]");
            myS.GetXaxis()->CenterTitle();
            myS.GetXaxis()->SetTitleOffset(1.2);
            //segA10->SetTitle("Segment Angles");

            TLegend *leg = new TLegend(0.7,0.6,0.9,0.9);
            leg->AddEntry("segA10","pid = 10","f");
            leg->AddEntry("segA9","pid = 9","f");
            leg->AddEntry("segA8","pid = 8","f");
            leg->AddEntry("segA7","pid = 7","f");
            leg->AddEntry("segA6","pid = 6","f");
            leg->AddEntry("segA5","pid = 5","f");
            leg->AddEntry("segA4","pid = 4","f");
            leg->AddEntry("segA3","pid = 3","f");
            leg->AddEntry("segA2","pid = 2","f");
            leg->Draw();
            c1.SaveAs(Form("plots/SegA_me%i%i.png",ST,RI));
            myS.GetYaxis()->SetRangeUser(2.0,1.1*myS.GetMaximum());
            c1.SetLogy();
            c1.SaveAs(Form("plots/SegA_me%i%i_log.png",ST,RI));
            if(ST == 2 && RI == 1) 
            {
                st21 = (THStack*)myS.Clone();
                myL = (TLegend*)leg->Clone();
            }
        }
    }


};
