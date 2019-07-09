import glob
import os

EOS = '/uscms/home/wnash/eos/'
#SAMPLE_DIR = 'Charmonium/charmonium2017C/'
#SAMPLE_DIR = 'SingleMuon/zskim2018D/'
SAMPLE_DIR = 'SingleMuon/zskim2018D-redo/'
TOP_DIR = EOS+SAMPLE_DIR
#SRC_DIR = '../src/'
#LIB_DIR = '../lib/'
#LUT_DIR = '../dat/' #TODO: clean this up...
MAKE_DIR = '..'
TEMPLATE_RUN_FOLDER = 'run/'

RUN_ON_CONDOR_SCRIPT = 'runOnCondor.sh' #runs an executable on condor, (loads root, etc)
EXECUTABLE = 'MultiplicityStudy'
OUTPUT_IDENTIFIER = 'Multiplicity'
OUTPUT_FOLDER = OUTPUT_IDENTIFIER+'/'
 
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


def makeTarball(tarFile, runFolder):
    #this is poor design, but its hot and I can't think of how to do this more gracefully
    srcDir = 'src/'
    libDir = 'lib/'
    incDir = 'include/'
    lutDir = 'dat/'
    
    makefileToTar = 'Makefile'
    #executableToTar = srcDir+ EXECUTABLE
    libsToTar = libDir + '*.so ' + libDir + '*.d ' + libDir+ '*.pcm'
    srcsToTar = srcDir + '*.cpp'
    incsToTar = incDir + '*.h'
    lutsToTar = lutDir +'*.lut'
    
    # executable, libs, cpp, h
    tarLines ='%s %s %s %s %s'%(makefileToTar, libsToTar, srcsToTar, incsToTar,lutsToTar)
    os.system('pushd %s; tar -hczf %s %s;popd; mv %s/%s %s'%(MAKE_DIR,tarFile, tarLines, MAKE_DIR,tarFile, runFolder)) #tar and change back
    return runFolder+tarFile #return the tarFile path


#writes the submission script for condor
def writeSubmitScript(scriptfile, runFolder, runOnCondor, tarPath, inputfilePath, outputIdentifier):

    inputfile = inputfilePath.split('/')[-1]
    
    #outputfile = outputFolder + '/' + num + '.root'
    outputfile = outputIdentifier + '.root'
    
    scriptfile.write("universe               =vanilla\n")
    scriptfile.write("executable             ="+(runFolder+runOnCondor)+"\n")
    scriptfile.write("arguments              ="+EXECUTABLE+" "+inputfile+" "+outputfile+"\n")
    scriptfile.write("output                 ="+SAMPLE_DIR+outputIdentifier+".out\n")
    scriptfile.write("error                  ="+SAMPLE_DIR+outputIdentifier+".err\n")
    scriptfile.write("log                    ="+SAMPLE_DIR+outputIdentifier+".log\n")
    scriptfile.write("should_transfer_files  =YES\n")
    scriptfile.write("transfer_input_files   ="+(runFolder+runOnCondor)+","+tarPath+","+inputfilePath+"\n")
    scriptfile.write("transfer_output_files  ="+outputfile+"\n")
    scriptfile.write("transfer_output_remaps =\"%s=%s\"\n"%(outputfile, TOP_DIR+OUTPUT_FOLDER+outputfile))
    scriptfile.write("whentotransferoutput   =ON_EXIT\n")
    scriptfile.write("getenv                 =True\n")
    scriptfile.write("queue                  1\n")


#
# Make folders for holding all the things we will make
#

#make output directory
outputFolder = makeDir(TOP_DIR, OUTPUT_FOLDER)
#make condor shell script directory
shellFolder  = makeDir('',SAMPLE_DIR)
#where we will keep all the executables
runFolder    = makeDir(SAMPLE_DIR, TEMPLATE_RUN_FOLDER)

'''    
#
# Compile the root macro
#
print "\nCompiling Macro:\n"
os.system('pushd '+MAKE_DIR+'; make ;popd')
if not os.path.isfile(SRC_DIR+EXECUTABLE):
    #copy executable to running directory
    #os.system('mv '+SRC_DIR+EXECUTABLE+' '+runFolder)
#else:
    print "Failed to compile executable"
    exit()
'''  
    
print "\nMaking tarball:\n"
tarFile = 'package.tar'
tarPath = makeTarball(tarFile, runFolder)

print "Copying Condor "
if os.path.isfile(TEMPLATE_RUN_FOLDER+RUN_ON_CONDOR_SCRIPT):
    os.system('cp '+(TEMPLATE_RUN_FOLDER+RUN_ON_CONDOR_SCRIPT)+' '+runFolder)
else:
    print "Can't find "+TEMPLATE_RUN_FOLDER+RUN_ON_CONDOR_SCRIPT
    exit

#
# Run over all the files in the directory we want and send them to condor
#

for inputfilePath in glob.glob(TOP_DIR + '/*/*/CSCDigiTree_*.root'):
#for inputfilePath in glob.glob(TOP_DIR + '/*/*/CSCDigiTree_*_1.root'):
#for inputfilePath in [ TOP_DIR+ '180518_214403/0000/CSCDigiTree_420.root']:
#for inputfilePath in [TOP_DIR+'*/*/CSCDigiTree_*_1.root']:
    print "Making condor script for %s"%inputfilePath
    
    #pulls out the number associated with the input file
    num = inputfilePath.split('_')[-1].split('.')[0]
    
    thisOutputIdentifier = OUTPUT_IDENTIFIER+'-'+num
    
    # create a condor script to run for each .root file in the directory
    
    scriptfileName = shellFolder+'/'+thisOutputIdentifier+'.sub'
    scriptfile = open(scriptfileName, "w")
    #writeSubmitScript(scriptfile, runFolder, RUN_ON_CONDOR_SCRIPT, EXECUTABLE, inputfilePath, thisOutputIdentifier)
    writeSubmitScript(scriptfile, runFolder, RUN_ON_CONDOR_SCRIPT, tarPath, inputfilePath, thisOutputIdentifier)
    scriptfile.close()
    
    os.system("condor_submit "+scriptfileName)

