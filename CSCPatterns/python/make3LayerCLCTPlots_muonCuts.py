import ROOT as r
import Plotter as p
 
f = r.TFile('../data/SingleMuon/zskim2018D-full/CSCDigiTree-Full.root')
can = p.Canvas(lumi='')
 
pt_h = f.Get('h_muonCuts')
 
 
pt = p.Plot(pt_h,'', legType='',option='hist')

can.addMainPlot(pt, color=r.kBlue)
pt.scaleTitleOffsets(1.2,'Y')
#can.makeLegend(pos='tl')
   
can.cleanup('muonCuts.pdf', mode='BOB')#