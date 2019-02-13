import ROOT as r
import Plotter as p

f = r.TFile('../dat/PEstimation.root')

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


can = p.Canvas(lumi='')
for detector in detectors:
    plot = p.Plot(detector,f,'',legType='l', option='pe')
    plot.GetYaxis().SetRangeUser(0,0.5)
    can.addMainPlot(plot)
    
leg = can.makeLegend(pos='tl')
leg.resizeHeight(1.02)
can.firstPlot.GetXaxis().SetTitle('P [GeV]')
can.firstPlot.GetYaxis().SetTitle("Probability")

can.cleanup('../plots/ShowerProbabilities.pdf', mode='BOB')