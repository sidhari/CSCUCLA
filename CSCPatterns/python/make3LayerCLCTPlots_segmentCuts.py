import ROOT as r
import Plotter as p
 
writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'

f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
can = p.Canvas(lumi='',logy=True)
 
mep11a ='me_p11a_11'
mep11b ='me_p11b_11'
mem11a ='me_m11a_11'
mem11b ='me_m11b_11'

chambers =[mep11a,mep11b,mem11a,mem11b]

for chamber in chambers:
    me11 = p.Plot('h_clctEff_cuts_'+chamber,f,'',legType = 'l',option='hist') 
    me11.setTitles(X='')
#     if chamber is mep11a: me11.legName = '#splitline{#bf{ME+11A}}{Entries:%i}'%me11.GetEntries()
#     if chamber is mep11b: me11.legName = '#splitline{#bf{ME+11B}}{Entries:%i}'%me11.GetEntries()
#     if chamber is mem11a: me11.legName = '#splitline{#bf{ME-11A}}{Entries:%i}'%me11.GetEntries()
#     if chamber is mem11b: me11.legName = '#splitline{#bf{ME-11B}}{Entries:%i}'%me11.GetEntries()
#         

    if chamber is mep11a: me11.legName = '#bf{ME+11A}'
    if chamber is mep11b: me11.legName = '#bf{ME+11B}'
    if chamber is mem11a: me11.legName = '#bf{ME-11A}'
    if chamber is mem11b: me11.legName = '#bf{ME-11B}'
    if chamber is mep11a or chamber is mem11a:
        can.addMainPlot(me11, color=r.kBlack)
    else:
        can.addMainPlot(me11,color=r.kRed)
        
    if chamber is mem11a or chamber is mem11b:
        me11.plot.SetLineStyle(r.kDashed)
        
can.setMaximum(2.)

leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.1)
#leg.resizeHeight(2.)

   
can.cleanup(writePath+'segmentCuts.pdf', mode='BOB')#