import ROOT as r
import Plotter as p
import StatsTools as s

#f = r.TFile('../dat/PEstimation_2Sets.root')
f = r.TFile('../dat/PEstimation_1.root')

detectors = ["ECAL",
        "HCAL",
        "ME11B",
        "ME11A",
        "ME12",
        "ME13",
        "ME21",
        "ME22",
        "ME31",
        "ME32",
        "ME41",
        "ME42"]

#detectors = ["ECAL",
#        "HCAL"]


can = p.Canvas(lumi='')
for detector in detectors:
    #plot = p.Plot(detector,f,'',legType='l', option='pe')
    num = p.Plot(detector+'_num',f,'',legType='l', option='pe')
    den = p.Plot(detector+'_den',f,'',legType='', option='hist')
    
    if den.plot.GetMaximum() == 0: 
        continue
    #print num.plot
    #print den.plot
    num.BinomialDivide(den.plot)
    num.GetYaxis().SetRangeUser(0,0.5)
    num.SetTitle(detector)
    can.addMainPlot(num)
    
leg = can.makeLegend(pos='tl')
leg.resizeHeight(1.02)
can.firstPlot.GetXaxis().SetTitle('P [GeV]')
can.firstPlot.GetYaxis().SetTitle("Probability")

can.cleanup('../plots/ShowerProbabilities.pdf', mode='BOB')