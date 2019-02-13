import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
 
f = r.TFile('../../dat/SingleMuon/zskim2018D/emulation.root')
can = p.Canvas(True,lumi='')
 
match_h = f.Get('emulationStripDiff')
 
 
match = p.Plot(match_h,'Emulation Matching', legType='',option='hist')
can.addMainPlot(match, color=r.kBlue,addS=True)
#pt.GetXaxis().SetRangeUser(60.,120.)
#can.makeLegend(pos='tl')

r.gStyle.SetStatX(0.35)
r.gStyle.SetStatY(0.85)
r.gStyle.SetOptStat(111110)
can.makeStatsBox(match)
   
can.cleanup('stripDiff.pdf', mode='BOB')#