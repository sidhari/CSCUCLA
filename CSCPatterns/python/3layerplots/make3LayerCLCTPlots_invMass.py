import ROOT as r
import Plotter as p
 
f = r.TFile('../data/SingleMuon/zskim2018D-full/CSCDigiTree-Full.root')
can = p.Canvas(lumi='')
 
pt_h = f.Get('h_premassCutInvMass')
 
 
pt = p.Plot(pt_h,'#bf{Entries:}%i'%pt_h.GetEntries(), legType='',option='hist')
can.addMainPlot(pt, color=r.kBlue)
pt.GetXaxis().SetRangeUser(60.,120.)
can.makeLegend(pos='tl')
   
can.cleanup('invMass.pdf', mode='BOB')#
