#!/usr/bin/env python

import ROOT as r
from array import array

useCompHits = 1 # 0 means use recHits

folder = ""
coverage = 0.99 #look for bounds that give us 99% of the all entries in the histogram
if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"

threeWideColor = 9
legacyColor = 46
ccColor = 1


chambers = []

legacyPos = [array('d'), array('d')]
threeWidePos = [array('d'), array('d')]
ccPos = [array('d'), array('d')]

legacySlope = [array('d'), array('d')]
threeWideSlope = [array('d'), array('d')]
ccSlope = [array('d'), array('d')]

with open("intervals_%s.txt"%folder) as f:
    for l,line in enumerate(f):
        if l == 0: continue
        data  = line.strip("\n").split()
        chambers.append(data[0])
        legacyPos[0].append(abs(float(data[1])))
        legacyPos[1].append(float(data[2]))
        threeWidePos[0].append(abs(float(data[3])))
        threeWidePos[1].append(float(data[4]))
        ccPos[0].append(abs(float(data[5])))
        ccPos[1].append(float(data[6]))
        
        
        legacySlope[0].append(abs(float(data[7])))
        legacySlope[1].append(float(data[8]))
        threeWideSlope[0].append(abs(float(data[9])))
        threeWideSlope[1].append(float(data[10]))
        ccSlope[0].append(abs(float(data[11])))
        ccSlope[1].append(float(data[12]))


nchambers = len(chambers)

x = array('d')
y = array('d')
ex = array('d')


for i in range(nchambers):
    x.append(i)
    y.append(0)
    ex.append(0.5)


legacy = [legacyPos, legacySlope]
threeWide = [threeWidePos, threeWideSlope]
cc = [ccPos, ccSlope]

for i in range(2):
    c = r.TCanvas()
    
    legGraph = r.TGraphAsymmErrors(nchambers,x,y,ex,ex,legacy[i][0], legacy[i][1])
    threeGraph = r.TGraphAsymmErrors(nchambers,x,y,ex,ex,threeWide[i][0], threeWide[i][1])
    ccGraph = r.TGraphAsymmErrors(nchambers,x,y,ex,ex,cc[i][0], cc[i][1])
    
    
    for j in range(0,nchambers):
        threeGraph.GetXaxis().SetBinLabel(threeGraph.GetXaxis().FindBin(x[j]),chambers[j])
        
    threeGraph.GetXaxis().LabelsOption("u")
    
    if i == 0:
        threeGraph.GetYaxis().SetTitle("90% Coverage Segment - Pattern Position [strips]")
    else:
        threeGraph.GetYaxis().SetTitle("90% Coverage Segment - Pattern Slope [strips/layer]")
    threeGraph.GetYaxis().CenterTitle()
    threeGraph.SetTitle("")
    
    threeGraph.SetFillColor(threeWideColor)
    threeGraph.Draw("a2")
    
    
    legGraph.SetFillColor(legacyColor)
    legGraph.Draw("2")
    
    
    ccGraph.SetFillColor(ccColor)
    ccGraph.Draw("2")
    
    
    leg = r.TLegend(0.75,0.77, 0.97, 0.95)
    leg.AddEntry(legGraph,"Legacy")
    leg.AddEntry(threeGraph,"Three-Wide")
    leg.AddEntry(ccGraph,"Comparator Code")
    leg.Draw()
    
    if i == 0: 
        c.SaveAs("../img/%s/coverageIntervalPosition.pdf"%(folder))
    else:
        c.SaveAs("../img/%s/coverageIntervalSlope.pdf"%(folder))

