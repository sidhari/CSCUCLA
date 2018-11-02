import ROOT as r
import Plotter as p
import StatsTools as s
 
f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
can = p.Canvas(logy=True,lumi='')


#den_h = f.Get('h_clctEff_den')
#hasClct = f.Get('')
 
#pt_h = f.Get('h_eventCuts')
 
 
den = p.Plot('h_clctEff_den',f, legName='All ME11A/B Segments',legType='',option='hist')
hasClct = p.Plot('h_clctEff_hasClct',f,legName='w/ CLCT ',legType='l',option='pe')
has3LayClct = p.Plot('h_clctEff_has3layClct;1',f,legName='w/ 3Lay CLCT',legType='l',option='pe')
ThreeLayClct = p.Plot('h_clctEff_3LayClct',f,legName='w/ matched 3Lay CLCT',legType='l',option='pe')

hasClct.BinomialDivide(den.plot)
has3LayClct.BinomialDivide(den.plot)
ThreeLayClct.BinomialDivide(den.plot)


#hasClctEff = p.Plot(s.binomial_divide(hasClct.plot, den.plot)[0], legName='w/ CLCT ',legType='l',option='pe')
#has3LayClctEff =p.Plot(s.binomial_divide(has3LayClct.plot, den.plot)[0],legName='w/ CLCT ',legType='l',option='pe')
#ThreeLayClctEff = p.Plot(s.binomial_divide(ThreeLayClct.plot, den.plot)[0],legName='w/ CLCT ',legType='l',option='pe')

can.addMainPlot(den, color=r.kWhite)
can.addMainPlot(hasClct)
can.addMainPlot(has3LayClct)
can.addMainPlot(ThreeLayClct)

#can.setMaximum(factor=2.)
can.firstPlot.SetMinimum(0.001)
can.firstPlot.SetMaximum(1.1)
#hasClct.GetXaxis().SetRangeUser(20.,100.)
#can.Update()
#den.scaleTitleOffsets(1.2,'Y')
leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.2)
   
can.cleanup('me11ClctEff.pdf', mode='BOB')#