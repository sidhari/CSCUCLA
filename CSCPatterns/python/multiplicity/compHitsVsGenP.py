import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181128-Multiplicity/'
 

f = r.TFile('../../dat/MuonGun/MuonGun-Test.root')
#f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-184.root')

suffixes = ['_1','_2','_3','_4','_5']

for suffix in suffixes:
    can = p.Canvas(lumi='')
    compHitsVsGenP = p.Plot('h_compHitsPerChamberVsGenP'+suffix,f, '', legType='l', option='colz')
    #if len(suffix):
    #    compHitsVsGenP.SetTitle('Average Energy Deposited Over '+suffix+' Chambers')
    compHitsVsGenP.GetYaxis().SetRangeUser(0,100)
    compHitsVsGenP.GetXaxis().SetRangeUser(1,5000)
    
    for i in range(0, compHitsVsGenP.GetNbinsX()+1):
        print "=== New P Bin ==="
        norm = 0
        P = compHitsVsGenP.GetXaxis().GetBinCenter(i)
        
        for j in range(0, compHitsVsGenP.GetNbinsY()+1):
            norm += compHitsVsGenP.GetBinContent(i,j)
        if norm != 0.:
            for j in range(0, compHitsVsGenP.GetNbinsY()+1):
                content = 1.*compHitsVsGenP.GetBinContent(i,j)
                compHitsVsGenP.SetBinContent(i,j, content/norm)
                
    
    can.addMainPlot(compHitsVsGenP)
    #can.mainPad.SetLogz(True)
    #can.mainPad.SetLogx(True)
    can.Update()
    r.gPad.Update();
    palette = compHitsVsGenP.GetListOfFunctions().FindObject("palette");

    palette.SetX1NDC(0.9);
    palette.SetX2NDC(0.95);
    palette.SetY1NDC(0.12);
    palette.SetY2NDC(0.9);
    can.mainPad.SetRightMargin(0.12)
    r.gPad.Modified();
    r.gPad.Update();
    
    #compHitsVsGenP.GetYaxis().SetRangeUser(-15000,0)

    can.scaleMargins(0.9,'L')
    can.cleanup(writePath+'compHitsVsGenP'+suffix+'.pdf', 'BOB')
 