{
    TCanvas c1("c1","c1",1000,1000);
    TCanvas c2("c2","c2",1000,1000);
    c1.cd();
    int colz[11] = {0,0,402,kOrange,419,612,632,841,884,410,600};
    bool fD = true;
    int buf = 0;
    TFile myF("AnaCSCDigiTree.root");
    TGraph *grPIDmean[11];
    TGraph *grPIDrms[11];
    TLegend *leg = new TLegend(0.7,0.6,0.9,0.9);

    auto getCT = [](int s, int r) -> int {return 2*(4-s)+(r-1);};
    auto getRI = [](int ct) -> int {return (1 - (ct/6) )*( (ct%2) + 1 ) + (ct/6)*(ct - 5);};
    auto getST = [](int ct) -> int {return (1 - (ct/6) )*( 4 - (ct/2) ) + (ct/6);};

    for(int pid = 10; pid >= 2; pid--)
    {
        float mean[9];
        float rms[9];
        float ctA[9];
        float ctE[9];
        for(int i = 0; i < 9; i++)
        {
            mean[i] = 0;
            rms[i] = 0;
            ctA[i] = 0;
            ctE[i] = 0;
        }
        for(int ST = 1; ST <= 4; ST++)
        {
            for(int RI = 1; RI <= 3; RI++)
            {
                if(ST > 1 && RI > 2) continue;
                TH1F *segX = (TH1F*)myF.Get(Form("xSegMKHS_me%i%i_pid%i_h",ST,RI,pid));
                mean[getCT(ST,RI)] = segX->GetMean();
                rms[getCT(ST,RI)] = segX->GetRMS();
                ctA[getCT(ST,RI)] = getCT(ST,RI) + 1;
                ctE[getCT(ST,RI)] = 0;
                cout << "CT: " << getCT(ST,RI) << " mean: " << mean[getCT(ST,RI)] << " rms: " << mean[getCT(ST,RI)] << endl;

            }
        }
        // Build graphs
        grPIDmean[pid] = new TGraph(9,ctA,mean);
        grPIDmean[pid]->SetName(Form("Mgr%i",pid));
        grPIDmean[pid]->SetTitle("Means of KHS Resolution");
        grPIDmean[pid]->SetLineColor(colz[pid]);
        grPIDmean[pid]->SetLineWidth(2);
        grPIDmean[pid]->SetMarkerColor(colz[pid]);
        grPIDmean[pid]->SetMarkerStyle(20);
        grPIDmean[pid]->SetMarkerSize(1.5);

        grPIDrms[pid] = new TGraph(9,ctA,rms);
        grPIDrms[pid]->SetName(Form("Rgr%i",pid));
        grPIDrms[pid]->SetTitle("RMSs of KHS Resolution");
        grPIDrms[pid]->SetLineColor(colz[pid]);
        grPIDrms[pid]->SetLineWidth(2);
        grPIDrms[pid]->SetMarkerColor(colz[pid]);
        grPIDrms[pid]->SetMarkerStyle(20);
        grPIDrms[pid]->SetMarkerSize(1.5);
        if(fD) 
        { 
            for(int i = 0; i < 10; i++)
            {
                grPIDmean[pid]->GetXaxis()->SetBinLabel(grPIDmean[pid]->GetXaxis()->FindBin(i+1),Form("ME%i/%i",getST(i),getRI(i)));
                grPIDrms[pid]->GetXaxis()->SetBinLabel(grPIDmean[pid]->GetXaxis()->FindBin(i+1),Form("ME%i/%i",getST(i),getRI(i)));
            }
            grPIDmean[pid]->GetYaxis()->SetTitle("KHS Resolution Mean [strips]");
            c1.cd();
            grPIDmean[pid]->Draw("AP"); fD = false; 
            grPIDrms[pid]->GetYaxis()->SetTitle("KHS Resolution RMS [strips]");
            c2.cd();
            grPIDrms[pid]->Draw("AP"); fD = false; 
        }
        else 
        { 
            c1.cd();
            grPIDmean[pid]->Draw("Psame"); 
            c2.cd();
            grPIDrms[pid]->Draw("Psame"); 
        }
        leg->AddEntry(Form("Rgr%i",pid),Form("pid = %i",pid),"p");
    }

    c1.cd();
    leg->Draw();
    c2.cd();
    leg->Draw();

    c1.SaveAs("plots/SegAmeans.png");

};
