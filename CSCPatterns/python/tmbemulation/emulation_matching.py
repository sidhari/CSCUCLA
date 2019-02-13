import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
 
f = r.TFile('../../dat/SingleMuon/zskim2018D/emulation.root')
can = p.Canvas(True,lumi='')
 
match_h = f.Get('emulationMatching')
 
 
match = p.Plot(match_h,'Emulation Matching', legType='',option='hist')
can.addMainPlot(match, color=r.kBlue)
#pt.GetXaxis().SetRangeUser(60.,120.)
#can.makeLegend(pos='tl')

   
   

can.cleanup('matching.pdf', mode='BOB')#