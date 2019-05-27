#makeSegmentEffPlots.py
# Created on: Aug 2, 2018
#     Author: wnash

import ROOT as r
import common


def makeEffHist(name, h_num, h_den):
    _h = h_num.Clone()
    _h.Divide(h_den)
    _h.SetName(name)
    _h.SetTitle(name)
    return _h

def formatHist(_h,color=0):
    return formatHist(_h,  _h.GetMinimum(), _h.GetMaximum(),color)

def formatHist(_h, min,max,color=0):
    _h.SetLineWidth(2)
    _h.SetLineColor(color)
    _h.SetMaximum(max)
    _h.SetMinimum(min)
    return _h

dataset = "2017C"
posDiff = 0.3
slopeDiff= 0.25
ordering= "layers, chi2, slope"

filename = "~/workspace/CSCUCLA/CSCPatterns/data/Charmonium/charmonium"+dataset+"/CLCTMatch-Full.root"

r.gStyle.SetOptStat(0)

cT = r.TCanvas()
cT.cd()
legT = r.TLegend(0.20,0.30, 0.40, 0.45)

f = r.TFile(filename)

hists = []
h_den = f.Get("segEffDen")

for i in range(0,3):
    h_num = f.Get("segEffNum%i"%i)

    h = makeEffHist("%s -  quality = (%s), #deltax=%3.2f #deltas=%3.2f"%(dataset,ordering, posDiff,slopeDiff),h_num,h_den)
    hists.append(formatHist(h,0.91, 1.01,common.colors[i]))
    hists[i].GetYaxis().SetTitle("Efficiency / 5 GeV")
    if(i ==0):
        hists[i].Draw()
    else:
        hists[i].Add(hists[i-1])
        hists[i].Draw("same")
    legT.AddEntry(hists[i],"Top %i Candidate(s)"%(i+1))
    
legT.Draw()
cT.Print("SegmentEfficiency.pdf")


cT2 = r.TCanvas()
cT2.cd()

preselectionEfficiency = makeEffHist("Preselection Efficiency", f.Get("foundOneMatchEffNum"), f.Get("foundOneMatchEffDen"))
preselectionEfficiency.GetYaxis().SetTitle("Efficiency / 5 GeV")
preselectionEfficiency = formatHist(preselectionEfficiency, 0.91, 1.01, r.kBlue)
preselectionEfficiency.Draw()

cT2.Print("PreselectionEfficiency.pdf")

