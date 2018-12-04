import ROOT as r
import os

colors = [r.kBlack, r.kRed-4, r.kGreen+1, r.kBlue+1, r.kMagenta-4, r.kYellow-3, r.kCyan-3, r.kBlue+3, r.kRed+2, r.kOrange+7, r.kBlue-8, 30, 50, 20, r.kYellow, 38]


def printProgress(counter, entries):
    if((counter % 10000) == 0) : print("Finished %0.2f%% of events"%(100.*counter/entries))

  
class LUT:
    def __init__(self, isLegacy_=False):
        self.isLegacy = isLegacy_
        self.positions = {}
        self.slopes = {}
        self.nsegments = {}
        self.quality = {}
        self.layers = {}
        self.chi2 = {} 
    def addEntry(self, patt, cc, position, slope, nsegments, quality = None, layers = None, chi2 = None):
        if self.isLegacy: return
        if not patt in self.positions:
            self.positions[patt] = {}
            self.slopes[patt] = {}
            self.nsegments[patt] = {}
            self.quality[patt] = {}
            self.layers[patt] = {}
            self.chi2[patt] = {} 

        self.positions[patt][cc] = position
        self.slopes[patt][cc] = slope
        self.nsegments[patt][cc] = nsegments
        if quality is not None: self.quality[patt][cc] = quality
        if layers is not None: self.layers[patt][cc] = layers
        if chi2 is not None: self.chi2[patt][cc] = chi2

    
    def addLegacyEntry(self, patt, position, slope, nsegments):
        if not self.isLegacy: return
        self.positions[patt] = position
        self.slopes[patt] = slope
        self.nsegments[patt] = nsegments

#TODO: edit contstruction of this file to be identical to the construction of the data LUTs
def createLineFitLUT(filepath):
    print("\033[94m=== Reading Line Fit LUT ===\033[0m")
    
    lut = LUT()
    
    with open(filepath, "r") as f:
        for l,line in enumerate(f):
            if (l == 0): continue #skip first line
            
            #should be pat, cc, offset, slope, chi2, ndf
            elements = line.strip('\n').split('\t')

            #
            # Some funky sign issues, slope is opposite the expected sign,
            # and offset is off by 0.5 strips, and need to convert to strips
            #
            
            patt = int(elements[0])
            cc = int(elements[1])
            position = 0.5*float(elements[2]) + 0.25 #offset between the two
            slope = -0.5*float(elements[3])
            chi2 = float(elements[4])
            ndf = int(elements[5])
            poserr = 0.5*float(elements[6])
            slopeerr = 0.5*float(elements[7]) 
            
            nsegments = 0 #placeholder
            quality = 0
            layers = ndf + 2
            
            lut.addEntry(patt, cc, position, slope, nsegments, quality, layers, chi2)        
            
    return lut
       
       

def loadLUT(filepath):
    
    with open(filepath, 'r') as f:
        line = f.readline() #just look at the first like to determine shape of LUT
        [identifiers, lutquantities] = line.split('~')
        identifiers = identifiers.split()
        lutquantities = lutquantities.split()
        print "Loading LUT: %s"%filepath
        print "Sample line: " 
        print  identifiers 
        print  lutquantities
            
        lut = LUT() if len(identifiers) == 2 else LUT(True)
        
        f.seek(0) #reset to top
        for line in f:
            #should be formatted as 
            # pattern (cc) - position slope (quality layers chi2)
            
            [identifiers, lutquantities] = line.split('~')
            identifiers = identifiers.split()
            lutquantities = lutquantities.split()
            
            extras = len(lutquantities) > 3 # if we have the extras elements in our LUT
            
            patt = int(identifiers[0])
                
            if(len(identifiers)) == 1:
                positions = float(lutquantities[0])
                slopes    = float(lutquantities[1])
                nsegments = int(lutquantities[2])
                lut.addLegacyEntry(patt,positions, slopes, nsegments)
                                
            elif len(identifiers) == 2:
                cc = int(identifiers[1])
                position = float(lutquantities[0])
                slope   = float(lutquantities[1])
                nsegments = int(lutquantities[2])
                if extras == True:
                    quality = float(lutquantities[3])
                    layers  = int(lutquantities[4])
                    chi2    = float(lutquantities[5])
                    lut.addEntry(patt, cc, position, slope, nsegments, quality, layers, chi2)
                else:
                    lut.addEntry(patt, cc, position, slope, nsegments)
                       
            else:
                print("Error: lut file format incorrect")
                return 
                
    return lut


#not guaranteed to run for funny paths!
def makeDir(rootpath, folderpath):
    folders = folderpath.split('/')
    
    fcount = 0 
    foldertocreate = folders[fcount]
     
    while(fcount != len(folders)-1): #recursively create folders until we don't need to anymore
        if not os.path.isdir(rootpath+foldertocreate):
            path = rootpath+foldertocreate
            print("Making directory: %s"%path)
            os.system("mkdir %s"%path)
            
        fcount+=1
        foldertocreate += "/" + folders[fcount]
    return rootpath+foldertocreate

         