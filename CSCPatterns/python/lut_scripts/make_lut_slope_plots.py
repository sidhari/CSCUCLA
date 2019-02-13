#!/usr/bin/env python
import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181202-CCStatus/'
#writePath =''

plotType = 'Slope'


#filename = '/uscms/home/wnash/CSCUCLA/CSCPatterns/dat/Charmonium/charmonium2016F+2017BCEF/SingleMuon/zskim2018D/CLCTMatch-Full.root'
filename = '~/workspace/CSCUCLA/CSCPatterns/dat/Charmonium/charmonium2016F+2017BCEF/SingleMuon/zskim2018D/CLCTMatch-Full.root'

f = r.TFile(filename)
h_new = f.Get('h_lutSegment'+plotType+'Diff')
h_new.GetXaxis().SetTitle('Segment - LUT [strips]')
h_new.GetYaxis().SetTitle('Segments')

h_old = f.Get('h_legacy'+plotType+'Diff')


new = p.Plot(h_new, legType='l')
new.legName='#splitline{#bf{Comparator Codes}}{#splitline{Entries: %i}{Width: %5.4f}}'%(h_new.GetEntries(),h_new.GetStdDev())


old = p.Plot(h_old, legType='l')
old.legName='#splitline{#bf{Current Patterns}}{#splitline{Entries: %i}{Width: %5.4f}}'%(h_old.GetEntries(), h_old.GetStdDev())


can = p.Canvas(lumi='')

can.addMainPlot(new, color=r.kBlack)
can.addMainPlot(old, color = r.kRed)

#can = p.Canvas(lumi='')
#slopes = p.Plot(slopes_h,legType= 'l',legName='#splitline{#bf{Entries}: %i}{#bf{UFlow}: %i #bf{OFlow}: %i}'\
#                %(slopes_h.GetEntries(), slopes_h.GetBinContent(0), slopes_h.GetBinContent(slopes_h.GetNbinsX()+1)), option='hist')
#can.addMainPlot(slopes)
#can.setMaximum(factor = 1.2)
#can.firstPlot.SetMinimum(0)

leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.15,Y=-0.01)
leg.resizeHeight(3.)

can.cleanup(writePath+plotType+'.pdf', 'BOB')
