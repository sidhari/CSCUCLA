import ROOT as r
import Plotter as p
 
f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
can = p.Canvas(logy=True,lumi='')



matchedA = p.Plot('h_clctLayerCount_me_p11a11',f,legName='Matched ME11A',legType='l',option='pe')
unmatchedA = p.Plot('h_unmatched_clctLayerCount_me_p11a11',f,legName='Unmatched ME11A',legType='l',option='pe')
matchedB = p.Plot('h_clctLayerCount_me_p11b11',f,legName='Matched ME11B',legType='l',option='pe')
unmatchedB = p.Plot('h_unmatched_clctLayerCount_me_p11b11',f,legName='Unmatched ME11B',legType='l',option='pe')
 
 
#ratA_h = matchedA.Divide(unmatchedA.plot)
unmatchedA.Divide(matchedA.plot)
ratA_h = matchedA.plot
#ratB_h = matchedB.Divide(unmatchedB.plot)
unmatchedB.Divide(matchedB.plot)
ratB_h = matchedB.plot

ratA = p.Plot(ratA_h, legName='Unmatch/Matched A', legType='l',option='hist')
ratB = p.Plot(ratB_h, legName='Unmatch/Matched B', legType='l',option='hist')

#can.addMainPlot(unmatchedA,color=r.kWhite)
can.addMainPlot(ratA)
can.addMainPlot(ratB)
can.firstPlot.SetMaximum(1e7)
can.firstPlot.SetMinimum(0.1)


 
#pt = p.Plot(pt_h,'', legType='',option='hist')

#can.addMainPlot(pt, color=r.kBlue)
#pt.scaleTitleOffsets(1.2,'Y')
can.makeLegend(pos='tl')
   
can.cleanup('matchedUnmatched.pdf', mode='BOB')#