import ROOT as r
import Plotter as p
import StatsTools as s

#writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'
writePath ='~/Documents/Presentations/2018/181026-3LayerEff/pt10/'


#f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
f = r.TFile('../data/SingleMuon/zskim2018D/CLCTLayerAnalysis-Pt10.root')
can = p.Canvas(True,lumi='',ratioFactor=1./3)

# pt_a_h = f.Get("h_mep11a_11_Pt")
# pt_a_3_h = f.Get("h_mep11a_3Lay_11_Pt")
# pt_b_h = f.Get("h_mep11b_11_Pt")
# pt_b_3_h = f.Get("h_mep11b_3Lay_11_Pt")
# pt_a_h.GetXaxis().SetTitle("Pt [GeV]")
pt_a_h = f.Get("h_matchedPt_me_p11a_11")
pt_a_3_h = f.Get("h_matchedPt_3Lay_me_p11a_11")
pt_b_h = f.Get("h_matchedPt_me_p11b_11")
pt_b_3_h = f.Get("h_matchedPt_3Lay_me_p11b_11")
pt_a_h.GetXaxis().SetTitle("Pt [GeV]")


rat_a_h = s.binomial_divide(pt_a_3_h, pt_a_h)[0]
rat_b_h = s.binomial_divide(pt_b_3_h, pt_b_h)[0]


pt_a = p.Plot(pt_a_h,legName='ME+1/1/11A', legType='l')
pt_a_3 = p.Plot(pt_a_3_h,legName='ME+1/1/11A 3Lay', legType='l')
pt_b = p.Plot(pt_b_h,legName='ME+1/1/11B', legType='l')
pt_b_3 = p.Plot(pt_b_3_h,legName='ME+1/1/11B 3Lay', legType='l')

rat_a = p.Plot(rat_a_h, legName='ME+11A', legType='l',option='pe')
rat_b = p.Plot(rat_b_h, legName='ME+11B', legType='l',option='pe')


can.addMainPlot(pt_a,color=r.kBlack)
can.addMainPlot(pt_a_3,color=r.kBlack)
can.addMainPlot(pt_b,color=r.kRed)
can.addMainPlot(pt_b_3,color=r.kRed)


pt_a_3.SetLineStyle(5)
pt_b_3.SetLineStyle(5)

can.setMaximum(factor=1.5)
can.firstPlot.SetMinimum(0.01)

##hack to get axes to draw correctly
can.addAndDivideRatioPlot(pt_a_3,pt_a, color=r.kWhite, option='pe',ytit='(3 lay) / (3-6 lay)', xtit='Pt [GeV]', yrange=[-0.01,0.05])
#can.addRatioPlot(pt_a_3,pt_a, color=r.kRed,ytit='ME11 (3 lay) / (3-6 lay)', xtit='Pt[GeV]', plusminus=2)
##can.addRatioPlot(pt_b_3,pt_b, color=r.kBlue, option='pe', ytit='ME11 (3 lay) / (3-6 lay)', xtit='Pt[GeV]', yrange=[-0.1,0.2])
#can.addRatioPlot(pt_b_3,pt_b, color=r.kBlue,ytit='ME11 (3 lay) / (3-6 lay)', xtit='Pt[GeV]', plusminus=2)
#can.addRatioPlot(pt_b_3, pt_b, ytit='ME11B (3 lay) / (3-6 lay)', xtit='Pt[GeV]')

can.addRatioPlot(rat_a,color=r.kBlack, option='pe',ytit='(3 lay) / (3-6 lay)', xtit='Pt [GeV]', yrange=[-0.01,0.05] )
can.addRatioPlot(rat_b,color=r.kRed, option='pe',ytit=' (3 lay) / (3-6 lay)', xtit='Pt [GeV]', yrange=[-0.01,0.05] )


can.makeLegend(pos='tl')


can.cleanup(writePath+'clctPtDistribution.pdf', mode='BOB')