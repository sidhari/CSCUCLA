#!/usr/bin/env python
import ROOT as r
import Plotter as p

writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'
def afterSecondUpdate(evt):
    return int(evt) > 323362
# 
# writePath ='~/Documents/Presentations/2018/181026-3LayerEff/secondFirmwareUpdate/'
# def badEvent(evt):
#     return int(evt) <= 323362


lumifile = open('../data/SingleMuon/zskim2018D-full/lumi2018D-Nov6-2018','r')

#data = lumifile.readlines()

for i in range(4):
     lumifile.readline()
#     
# for i in range(3):
h_before = r.TH1F("lumiHist","lumiHist; Inst. Lumi [10^{34} cm^{-2}s^{-1}] ; Lumi Sections",50, 0.10,2)
h_after = r.TH1F("lumiHist","lumiHist; Inst. Lumi [10^{34} cm^{-2}s^{-1}] ; Lumi Sections",50, 0.10,2)

for line in lumifile.readlines():
     #line = lumifile.readline()
     
     try:   
        lumi = line.split('|')[7]
     except IndexError:
        break
    
     #print line.split('|')[1]
     evt = line.split('|')[1].split(':')[0]
     #print evt
     
     try:
         lumi = float(lumi)/10000.
         #lumi = float(lumi)/10000.
     except ValueError:
         break
     
     if afterSecondUpdate(evt):
         h_after.Fill(lumi)
     else:
         h_before.Fill(lumi)
    
    #if(evt.RunNumber > 323362) continue;
    
    
        
     #Ah.Fill(lumi/10000.)

can = p.Canvas(lumi='')

before = p.Plot(h_before, legName='Aug23 - Sep12',legType='l', option='hist')
after = p.Plot(h_after, legName='Sep12 - Sep30',legType='l', option='hist')

can.addMainPlot(before)
can.addMainPlot(after,addS=True)


leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.1)
leg.resizeHeight(0.8)
r.gStyle.SetStatX(0.35)
r.gStyle.SetStatY(0.85)
r.gStyle.SetOptStat(111110)


beforeStats = before.plot.FindObject('stats')
beforeStats.SetX1NDC(0.15)
beforeStats.SetX2NDC(0.35)
beforeStats.SetY1NDC(0.65)
beforeStats.SetY2NDC(0.85)

afterStats = after.plot.FindObject('stats')
afterStats.SetX1NDC(0.15)
afterStats.SetX2NDC(0.35)
afterStats.SetY1NDC(0.4)
afterStats.SetY2NDC(0.6)
afterStats.SetTextColor(r.kRed)


can.makeStatsBox(before)
can.makeStatsBox(after)

can.cleanup(writePath+'lumiPlot.pdf',mode='BOB')

