import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181128-Multiplicity/'
 

f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-Full.root')
#f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-184.root')

suffixes = ['','_1','_2','_3','_4','_5']

for suffix in suffixes:
    can = p.Canvas(lumi='')
    energyVsP = p.Plot('h_energyPerChamberVsP'+suffix,f, '', legType='l', option='colz')
    #if len(suffix):
    #    energyVsP.SetTitle('Average Energy Deposited Over '+suffix+' Chambers')
    energyVsP.GetYaxis().SetRangeUser(-20000,-2000)
    #energyVsP.GetXaxis().SetRangeUser(1,800)
    
    for i in range(0, energyVsP.GetNbinsX()+1):
        print "=== New P Bin ==="
        norm = 0
        P = energyVsP.GetXaxis().GetBinCenter(i)
        
        for j in range(0, energyVsP.GetNbinsY()+1):
            norm += energyVsP.GetBinContent(i,j)
        if norm != 0.:
            for j in range(0, energyVsP.GetNbinsY()+1):
                content = 1.*energyVsP.GetBinContent(i,j)
                energyVsP.SetBinContent(i,j, content/norm)
                
    
    can.addMainPlot(energyVsP)
    #can.mainPad.SetLogz(True)
    #can.mainPad.SetLogx(True)
    can.Update()
    r.gPad.Update();
    palette = energyVsP.GetListOfFunctions().FindObject("palette");

    palette.SetX1NDC(0.9);
    palette.SetX2NDC(0.95);
    palette.SetY1NDC(0.2);
    palette.SetY2NDC(0.8);
    r.gPad.Modified();
    r.gPad.Update();
    
    #energyVsP.GetYaxis().SetRangeUser(-15000,0)

    can.cleanup(writePath+'energyVsP'+suffix+'.pdf', 'BOB')
 