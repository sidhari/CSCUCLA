import ROOT as r
import Plotter as p
from array import array

f = r.TFile('../dat/PEstimation_2Sets.root')

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

pBinSize = 500
maxP = 4000

pBins = []
for P in range(0, maxP+pBinSize, pBinSize):
    pBins.append(P)




fits =[]
for detector in detectors:
    can = p.Canvas(lumi='')
    
    for i in range(len(pBins)-1):
        if(pBins[i]%1000 == 0): continue
        h_name = "%s_energy%i_%i"%(detector,pBins[i],pBins[i+1])
        
        #detector + '_energy'+ pBins[i]+'_'+pBins[i+1]
    
        plot = p.Plot(h_name,f,'',legType='l', option='hist')
        
        
        fit = r.TF1("f%s_%i"%(detector, i),"landau",0,0.3*plot.GetXaxis().GetXmax())
        fits.append(fit)
        plot.legName = '#splitline{#bf{%4i #leq P < %4i}}{Entries: %i, Overflow: %i}'%(pBins[i], pBins[i+1], plot.GetEntries(), plot.GetBinContent(plot.GetXaxis().GetNbins()+1))
        #plot.GetYaxis().SetRangeUser(0,0.5)
        #plot.Fit("landau")
        #add overflow to last bin
        plot.AddBinContent(plot.GetXaxis().GetNbins(), plot.GetBinContent(plot.GetXaxis().GetNbins()+1))
        can.addMainPlot(plot)
        fit.SetParameter(0,plot.GetMaximum())
        fit.SetParameter(1,plot.GetXaxis().GetBinCenter(plot.GetMaximumBin()))
        #plot.Fit("f%s_%i"%(detector, i), 'R')
        #fit.Draw('same')
        #plot.Fit("landau")

    
    leg = can.makeLegend(pos='tr')
    leg.resizeHeight(2)
    leg.moveLegend(X=-0.3)
    if detector in ['ECAL', 'HCAL']:
        can.firstPlot.GetXaxis().SetTitle('Energy Deposition [GeV]')
    else:
        can.firstPlot.GetXaxis().SetTitle('Energy Deposition [?]')
    can.firstPlot.GetYaxis().SetTitle("Count")
    
    can.cleanup('../plots/%s_EnergyDistributions.pdf'%detector, mode='BOB')