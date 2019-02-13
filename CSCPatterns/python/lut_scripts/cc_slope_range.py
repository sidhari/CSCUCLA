#!/usr/bin/env python
import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181202-CCStatus/'

r.gSystem.Load('../../lib/LUTClasses_cpp')
r.gSystem.Load('../../lib/PatternFinderHelperFunctions_cpp')
r.gSystem.Load('../../lib/PatternFinderClasses_cpp')


lut = r.LUT("test", "../../dat/linearFits.lut");

#lut.printPython()

lut.makeFinal()

slopes_h = r.TH1F("slopes",";Slope [strips/layer]; Comparator Codes", 50, -1.6,1.6)

for [key, entry] in lut:
    #print entry.slope()
    if(entry._layers < 3): continue
    slopes_h.Fill(entry.slope())
    
    
can = p.Canvas(lumi='')
slopes = p.Plot(slopes_h,legType= 'l',legName='#splitline{#bf{Entries}: %i}{#bf{UFlow}: %i #bf{OFlow}: %i}'\
                %(slopes_h.GetEntries(), slopes_h.GetBinContent(0), slopes_h.GetBinContent(slopes_h.GetNbinsX()+1)), option='hist')
can.addMainPlot(slopes)
can.setMaximum(factor = 1.2)
can.firstPlot.SetMinimum(0)

leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.2,Y=0.05)

can.cleanup(writePath+"slopesFullRange.pdf", 'BOB')