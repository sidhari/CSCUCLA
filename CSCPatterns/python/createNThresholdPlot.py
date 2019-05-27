#!/usr/bin/env python

import ROOT as r
import glob


path = '/uscms/home/wnash/work/CSCUCLA/sw/CMSSW_X_X/src/CSCUCLA/CSCPatterns/data/charmonium2017C/charmonium2017D'

colors = [r.kRed-4, r.kGreen+1, r.kBlue+1, r.kMagenta-4, r.kYellow-3, r.kCyan-3, r.kBlue+3, r.kRed+2, r.kOrange+7, r.kBlue-8]

cT = r.TCanvas()
cT.SetLogx()
legT = r.TLegend(0.85,0.67, 0.97, 0.95)

for i, filename in enumerate(glob.glob(path+'/*.root')):
    chamber = filename.split('/')[-1].split('_')[0]
    print "Analyzing chamber: %s"%chamber
    f = r.TFile(filename)

    g = f.Get("Graph")
    g.SetLineColor(colors[i])
    g.SetLineWidth(5)
    g.SetFillColor(r.kWhite)
    cT.cd()
    legT.AddEntry(g, chamber)
    if(i): 
        g.Draw("l same") 
    else: 
        g.SetTitle("")
        g.GetXaxis().SetTitleOffset(1.4)
        g.GetYaxis().SetTitleOffset(1.4)
        g.GetYaxis().SetRangeUser(0.075, 0.105)
        g.Draw("al")
        #cT.Update()
        #g.Draw("al")
    
    
legT.Draw()
cT.Print("compiledrmsVsNGraph.pdf")