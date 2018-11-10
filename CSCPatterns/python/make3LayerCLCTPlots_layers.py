import ROOT as r
import Plotter as p


writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'
f = r.TFile('../data/SingleMuon/zskim2018D/CLCTLayerAnalysis-Aug-Sep.root')

#writePath ='~/Documents/Presentations/2018/181026-3LayerEff/secondFirmwareUpdate/'
#f = r.TFile('../data/SingleMuon/zskim2018D/CLCTLayerAnalysis-Sep+.root')

print f
if not hasattr(f, 'IsOpen'):
    print "can't open file"
    exit()

can = p.Canvas(True,lumi='')

mep11a ='me_p11a_11'
mep11b ='me_p11b_11'
mem11a ='me_m11a_11'
mem11b ='me_m11b_11'

chambers =[mem11b,mem11a,mep11b,mep11a]

for chamber in chambers:
    me11 = p.Plot('h_clctLayerCount_'+chamber,f,'',legType = 'l',option='hist') 
    if chamber is mep11a: 
        me11.legName = '#splitline{#bf{ME+1/1/11A}}{Entries:%i}'%me11.GetEntries()
        can.addMainPlot(me11, color=r.kBlack)
    if chamber is mep11b: 
        me11.legName = '#splitline{#bf{ME+1/1/11B}}{Entries:%i}'%me11.GetEntries()
        can.addMainPlot(me11,color=r.kRed)
    if chamber is mem11a: 
        me11.legName = '#splitline{#bf{ME-1/1/11A}}{Entries:%i}'%me11.GetEntries()
        me11.legType = 'lf'
        can.addMainPlot(me11, color=r.kGray)
    if chamber is mem11b: 
        me11.legName = '#splitline{#bf{ME-1/1/11B}}{Entries:%i}'%me11.GetEntries()
        me11.legType = 'lf'
        can.addMainPlot(me11,color=r.kRed-9)
        
    if chamber is mem11a or chamber is mem11b:
        me11.plot.SetLineStyle(r.kDashed)
        if chamber is mem11a: 
            me11.plot.SetLineColor(r.kBlack)
            #me11.plot.SetFillStyle(3002)
            me11.plot.SetFillColor(r.kGray)
        else: 
            #me11.plot.SetFillStyle(3002)
            me11.plot.SetFillColor(r.kRed-9)
            me11.plot.SetLineColor(r.kRed)


can.setMaximum(factor = 2)
can.firstPlot.SetMinimum(10)

leg = can.makeLegend(pos='tl')
#leg.moveLegend(X=-0.1)
leg.resizeHeight(2.)
#r.gStyle.SetOptStat(10)
#can.makeStatsBox(me11a, color = 35)
#can.makeStatsBox(me11b)


can.cleanup(writePath+'layers.pdf',mode='BOB')

# 
# 
# f2 = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTDigiTree-Full.root')
# can = p.Canvas(True,lumi='')
# 
# pt_h = f2.Get('h_premassCutInvMass')
# 
# 
# pt = p.Plot(pt_h,'pt', legType='l',option='hist')
# can.addMainPlot(pt)

#can.cleanup('invMass.pdf', mode='BOB')#