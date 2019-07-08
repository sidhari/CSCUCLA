#!/usr/bin/env python
import ROOT as r
import common



rootfilepath = "../data/charmonium2017C/ME11B-LUT.root"
plot         = "Graph"
title        = "ME11B"
xaxis        = "Segments"
yaxis        = "Unique Comparator Codes"
outputname   = "../data/charmonium2017C/ME11B-UniqueCCvsSegments.pdf"


cT = r.TCanvas()
#cT.SetLogx()
cT.cd()
#legT = r.TLegend(0.85,0.87, 0.97, 0.95)


f = r.TFile(rootfilepath)
g = f.Get(plot)
g.SetLineColor(common.colors[0])
g.SetLineWidth(5)
g.SetFillColor(r.kWhite)

#legT.AddEntry(g, "Line Fits")
g.GetXaxis().SetTitle(xaxis)
g.GetYaxis().SetTitleOffset(1.3)
g.GetYaxis().SetTitle(yaxis)
g.SetTitle(title)
g.Draw("al")

    
#legT.Draw()
cT.Print(outputname)


