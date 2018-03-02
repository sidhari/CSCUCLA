#!/usr/bin/env python


import ROOT as r

useCompHits = 1 # 0 means use recHits
if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"


chambers = ["All-Chambers", "ME11A", "ME11B", "ME12", "ME13",\
            "ME21","ME22","ME31", "ME32", "ME41", "ME42"]

types = ["Slope", "Pos"]


for type in types:
    print("== %s =="% type)
    # loop through all the files
    for chamber in chambers:
        #print("== Looking at Chamber: %s =="%(chamber))
        #print(chamber, end='')
        inF = r.TFile("../data/%s/%s_resolutionPlots.root"%(folder, chamber))
        if not (inF.IsOpen()) :
            print("Missed File")
            continue
    
    

        # for each file, takes the mean of the proper histogram
        pat = inF.Get("cumPat%sRes"%(type))
        cc = inF.Get("cumCC%sRes"%(type))
        leg = inF.Get("cumLeg%sRes"%(type))
    
        
        patStd = pat.GetStdDev()
        ccStd = cc.GetStdDev()
        legStd = leg.GetStdDev()
        
        print("%s & %0.3f & %0.3f & %0.3f\\\\"%(chamber, legStd, patStd, ccStd))
        #print("patStd = %0.3f"%(patStd))
        
        # put that mean in a text file, properly formatted 
