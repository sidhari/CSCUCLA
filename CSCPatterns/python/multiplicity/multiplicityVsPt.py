import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
from array import array

writePath ='~/Documents/Presentations/2018/181128-Multiplicity/'
 

f = r.TFile('../../dat/SingleMuon/zskim2018D/Multiplicity-Full.root')
#can = p.Canvas(True,lumi='')

ptArr, probOneClctArr, likelihoodArr = array('d'), array('d'),array('d')
#dummy sample, assuming 4 chambers got hit by one muon, find likelihood of pt
samples = [1,1,2,1]

n = 0


den_h = r.TH1F('den','; Pt [GeV]; P(nclcts)', 40, 0, 400)
#normalize each pt bin, finding probability for just one clct for each bin
#oneClct_h = r.TH1F('oneClct','oneClct; Pt [GeV]; Probability', 40, 0, 400)

nclcts = []
for i in range(1, 11):
    nclcts.append(r.TH1F('%iClct'%i,'%iClct; Pt [GeV]; Probability'%i, 40, 0, 400))

mult_h = f.Get('h_multiplicityVsPt').Clone('h_multVsPt_norm')
for i in range(0, mult_h.GetNbinsX()+1):
    print "=== New Pt Bin ==="
    norm = 0
    pt = mult_h.GetXaxis().GetBinCenter(i)
    
    
    #test dummy sample
    likelihood=1. 
    for j in range(0, mult_h.GetNbinsY()+1):
        norm += mult_h.GetBinContent(i,j)
    if norm != 0.:
        ptArr.append(pt)
        den_h.SetBinContent(i, norm)
        for j in range(0, mult_h.GetNbinsY()+1):
            content = 1.*mult_h.GetBinContent(i,j)
            if j < len(nclcts)+1 and j > 0:
                print "Pt = %5.2f ----- Prob %i CLCT = %5.3f"%(pt, j-1, content/norm)
                nclcts[j-1].SetBinContent(i, content)
            if j == 1:
                pt = mult_h.GetXaxis().GetBinCenter(i)
                #print "Pt = %5.2f ----- Prob 1 CLCT = %5.3f"%(pt, content/norm) 
                probOneClctArr.append(content/norm)
                n+=1
                #oneClct_h.SetBinContent(i, content)
            mult_h.SetBinContent(i,j, content/norm)
            for sample in samples:
                if j == sample:
                    print "Multiplying Likelihood by: %f"%(content/norm) 
                    likelihood *= content/norm
        likelihoodArr.append(likelihood)
            

outF = r.TFile('multVsPt.root', 'RECREATE')
outF.cd()     
gr = r.TGraph(n, ptArr, probOneClctArr)
gr.GetXaxis().SetTitle('Pt [GeV]')
gr.GetYaxis().SetTitle('Probablity')
gr.SetTitle('Probability of having only 1 CLCT vs Pt')
gr.Write()



like = r.TGraph(n, ptArr, likelihoodArr)
likelihoodTitle =' Pt Likelihood Given Chamber Hits ['
for sample in samples:
    likelihoodTitle += '%i, '%sample
likelihoodTitle+=']'
like.SetTitle(likelihoodTitle)
like.GetXaxis().SetTitle('Pt [GeV]')
like.GetYaxis().SetTitle('Likelihood')
#like.Write()
canLike = r.TCanvas()
canLike.cd()
like.Draw()
canLike.Print('likelihood.pdf')
  
mult_h.Write()
outF.Close()

can = p.Canvas(True,lumi='')
den = p.Plot(den_h,legName='',legType = '',option='hist')
can.addMainPlot(den, color=r.kWhite)
can.firstPlot.SetMaximum(1.)
can.firstPlot.SetMinimum(0.00001)
#oneClct = p.Plot(oneClct_h,option='pe')

for i,hist in enumerate(nclcts):
    print "Finishing plot for %i CLCTS"%(i+1)
    plot = p.Plot(hist, legName='%i CLCTS'%(i+1),option='pe')
    plot.BinomialDivide(den.plot)
    plot.SetMarkerColor(plot.GetLineColor())
    #plot.plot.Fit('pol3')
    can.addMainPlot(plot)
    
leg = can.makeLegend(pos='br')
leg.resizeHeight(0.9)
leg.moveLegend(Y=-0.)
#oneClct.BinomialDivide(den.plot)

#can.addMainPlot(oneClct)


can.cleanup('clctsVsPt.pdf', mode='BOB')