import ROOT as r
import Plotter as p
import StatsTools as s
 
f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')


writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'

#den_h = f.Get('h_clctEff_den')
#hasClct = f.Get('')
 
#pt_h = f.Get('h_eventCuts')

mep11a ='me_p11a_11'
mep11b ='me_p11b_11'
mem11a ='me_m11a_11'
mem11b ='me_m11b_11'

chambers =[mep11a,mep11b,mem11a,mem11b]

for chamber in chambers:
    
    can = p.Canvas(logy=True,lumi='')
     
     
    den = p.Plot('h_clctEff_den_'+chamber,f, legName=chamber+' Segments',legType='',option='hist')
    hasClct = p.Plot('h_clctEff_hasClct_'+chamber,f,legName='w/ CLCT ',legType='l',option='pe')
    has3LayClct = p.Plot('h_clctEff_has3layClct_'+chamber,f,legName='w/ 3Lay CLCT',legType='l',option='pe')
    ThreeLayClct = p.Plot('h_clctEff_3LayClct_'+chamber,f,legName='w/ matched 3Lay CLCT',legType='l',option='pe')
    
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
       
    can.cleanup(writePath+chamber+'_ClctEff.pdf', mode='BOB')#




