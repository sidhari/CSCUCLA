import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
from array import array

writePath ='~/Documents/Presentations/2018/181128-Multiplicity/'
 

f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-Full.root')
#can = p.Canvas(True,lumi='')

PArr, probOneClctArr, likelihoodArr = array('d'), array('d'),array('d')
#dummy sample, assuming 4 chambers got hit by one muon, find likelihood of pt
samples = [1,1,2,1]

n = 0


den_h = r.TH1F('den','; P [GeV]; P(nclcts)', 60, 0, 600)
#normalize each P bin, finding probability for just one clct for each bin
oneClct_h = r.TH1F('oneClct','oneClct; P [GeV]; Probability', 60, 0, 600)

nclcts = []
for i in range(1, 11):
    nclcts.append(r.TH1F('%iClct'%i,'%iClct; P [GeV]; Probability'%i, 60, 0, 600))

mult_h = f.Get('h_multiplicityVsP').Clone('h_multVsP_norm')
for i in range(0, mult_h.GetNbinsX()+1):
    print "=== New P Bin ==="
    norm = 0
    P = mult_h.GetXaxis().GetBinCenter(i)
    
    
    #test dummy sample
    likelihood=1. 
    for j in range(0, mult_h.GetNbinsY()+1):
        norm += mult_h.GetBinContent(i,j)
    if norm != 0.:
        PArr.append(P)
        den_h.SetBinContent(i, norm)
        for j in range(0, mult_h.GetNbinsY()+1):
            content = 1.*mult_h.GetBinContent(i,j)
            if j < len(nclcts)+1 and j > 0:
                print "P = %5.2f ----- Prob %i CLCT = %5.3f"%(P, j-1, content/norm)
                nclcts[j-1].SetBinContent(i, content)
            if j == 1:
                P = mult_h.GetXaxis().GetBinCenter(i)
                #print "P = %5.2f ----- Prob 1 CLCT = %5.3f"%(P, content/norm) 
                probOneClctArr.append(content/norm)
                n+=1
                oneClct_h.SetBinContent(i, content)
            mult_h.SetBinContent(i,j, content/norm)
            for sample in samples:
                if j == sample:
                    print "Multiplying Likelihood by: %f"%(content/norm) 
                    likelihood *= content/norm
        likelihoodArr.append(likelihood)
            

outF = r.TFile('multVsP.root', 'RECREATE')
outF.cd()     
gr = r.TGraph(n, PArr, probOneClctArr)
gr.GetXaxis().SetTitle('P [GeV]')
gr.GetYaxis().SetTitle('Probablity')
gr.SetTitle('Probability of having only 1 CLCT vs P')
gr.Write()



like = r.TGraph(n, PArr, likelihoodArr)
likelihoodTitle =' P Likelihood Given Chamber Hits ['
for sample in samples:
    likelihoodTitle += '%i, '%sample
likelihoodTitle+=']'
like.SetTitle(likelihoodTitle)
like.GetXaxis().SetTitle('P [GeV]')
like.GetYaxis().SetTitle('Likelihood')
#like.Write()
canLike = r.TCanvas()
canLike.cd()
like.Draw()
canLike.Print('likelihood.pdf')
  
mult_h.Write()
outF.Close()
#  
can = p.Canvas(True,lumi='')
den = p.Plot(den_h,legName='',legType = '',option='hist')
can.addMainPlot(den, color=r.kWhite)
can.firstPlot.SetMaximum(0.2)
can.firstPlot.SetMinimum(0.000001)
  
  
for i,hist in enumerate(nclcts):
    print "Finishing plot for %i CLCTS"%(i+1)
    plot = p.Plot(hist, legName='%i CLCTS'%(i+1),option='pe')
    plot.BinomialDivide(den.plot)
    plot.SetMarkerColor(plot.GetLineColor())
    #plot.plot.Fit('pol3')
    can.addMainPlot(plot)
    plot.SetMarkerColor(plot.GetLineColor())
      
leg = can.makeLegend(pos='br')
leg.resizeHeight(0.9)
leg.moveLegend(X=-0.01)
 
# can = p.Canvas(lumi='')
# den = p.Plot(den_h,legName='',legType = '',option='hist')
# can.addMainPlot(den, color=r.kWhite)
# can.firstPlot.SetMaximum(1.)
# can.firstPlot.SetMinimum(0.75)
# oneClct = p.Plot(oneClct_h,legName = '1 CLCTS',option='pe')
# oneClct.BinomialDivide(den.plot)
# can.addMainPlot(oneClct)

#leg = can.makeLegend(pos='bl')
# leg.resizeHeight(0.9)
# leg.moveLegend(Y=-0.)


can.cleanup('clctsVsP.pdf', mode='BOB')