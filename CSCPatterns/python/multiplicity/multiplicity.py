import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181128-Multiplicity/'
 

f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-Full.root')
can = p.Canvas(True,lumi='')


me11b = p.Plot('ME11B', f,'ME11B', legType = 'l', option='hist')
me11a = p.Plot('ME11A', f,'ME11A', legType = 'l', option='hist')
me12 = p.Plot('ME12', f,'ME12', legType = 'l', option='hist')
me13 = p.Plot('ME13', f,'ME13', legType = 'l', option='hist')
me21 = p.Plot('ME21', f,'ME21', legType = 'l', option='hist')
me22 = p.Plot('ME22', f,'ME22', legType = 'l', option='hist')
me31 = p.Plot('ME31', f,'ME31', legType = 'l', option='hist')
me32 = p.Plot('ME32', f,'ME32', legType = 'l', option='hist')
me41 = p.Plot('ME41', f,'ME41', legType = 'l', option='hist')
me42 = p.Plot('ME42', f,'ME42', legType = 'l', option='hist')

plots = [me11b, me11a, me12, me13, me21, me22, me31, me32, me41, me42]

for plot in plots:
    norm = plot.GetEntries()
    if(norm != 0.) :plot.Scale(1./norm)
    plot.legName = '#bf{%s} Entries: %i'%(plot.legName, plot.GetEntries())
 
 

#me11b.legName = '#splitline{#splitline{#bf{%s}}{Entries: %i}}{Overflow: %i}'%(me11b.legName, me11b.GetEntries(), me11b.GetBinContent(me11b.GetSize()+1))
#me11b.legName = '#bf{%s} #splitline{Entries: %i}{Overflow: %i}'%(me11b.legName, me11b.GetEntries(), me11b.GetBinContent(me11b.GetSize()+1))
 

can.addMainPlot(me11b)
can.addMainPlot(me11a)
can.addMainPlot(me12)
can.addMainPlot(me13)
can.addMainPlot(me21)
can.addMainPlot(me22)
can.addMainPlot(me31)
can.addMainPlot(me32)
can.addMainPlot(me41)
can.addMainPlot(me42)
#match_h = f.Get('emulationStripDiff')

can.firstPlot.setTitles(Y='Fraction')


leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.24)
leg.resizeHeight(1.2)
can.cleanup(writePath+'multiplicity.pdf', 'BOB')
 