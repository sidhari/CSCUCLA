{
    int fN = 0;
    TFile myF(Form("/data/cscPats/cscDigiTrees/CSCDigiTree%i.root",fN));
    TTree *myT = (TTree*)myF.Get("CSCDigiTree");
    CSCDigiTree tt(myT);
    tt.Loop(Form("Char%i",fN));
};
