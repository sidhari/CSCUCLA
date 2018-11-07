import ROOT as r
import Plotter as p



f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
print f
if not hasattr(f, 'IsOpen'):
    print "can't open file"
    exit()
    
    

me11a_h = f.Get('h_clctLayerCount_me_m11a11')
me11b_h = f.Get('h_clctLayerCount_me_m11b11')

can = p.Canvas(True,lumi='')

me11a = p.Plot(me11a_h, '#splitline{#bf{ME-11A_11}}{Entries:%i}'%me11a_h.GetEntries(),legType = 'l',option='hist')
me11b = p.Plot(me11b_h, '#splitline{#bf{ME-11B_11}}{Entries:%i}'%me11b_h.GetEntries(),legType = 'l',option='hist')

can.addMainPlot(me11a, addS=True)
can.addMainPlot(me11b, addS=True)

can.setMaximum(factor = 2)

can.makeLegend(pos='tl')


can.cleanup('ME11ab11_3layerCLCT_minus.pdf',mode='BOB')
