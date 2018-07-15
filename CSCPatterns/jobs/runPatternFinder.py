import glob
import os

EOS = '/uscms/home/wnash/eos/'
SAMPLE_DIR = 'Charmonium/charmonium2017F/'
TOP_DIR = EOS+SAMPLE_DIR
SRC_DIR = '../src/'
TEMPLATE_RUN_FOLDER = 'run/'

RUN_ON_CONDOR_SCRIPT = 'runOnCondor.sh' #runs an executable on condor, (loads root, etc)
EXECUTABLE = 'PatternFinder'
OUTPUT_IDENTIFIER = 'CLCTMatch'
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



#writes the submission script for condor
def writeSubmitScript(scriptfile, runFolder, runOnCondor, EXECUTABLE, inputfilePath,outputIdentifier):

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
    scriptfile.write("transfer_input_files   ="+(runFolder+runOnCondor)+","+(runFolder+EXECUTABLE)+","+inputfilePath+"\n")
    scriptfile.write("transfer_output_files  ="+outputfile+"\n")
    scriptfile.write("transfer_output_remaps =\"%s=%s\"\n"%(outputfile, TOP_DIR+OUTPUT_FOLDER+outputfile))
    scriptfile.write("whentotransferoutput   =ON_EXIT\n")
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

    
#
# Compile the root macro
#
print "Compiling Macro:"
os.system('pushd '+SRC_DIR+'; make '+EXECUTABLE+';popd')
if os.path.isfile(SRC_DIR+EXECUTABLE):
    #copy executable to running directory
    os.system('mv '+SRC_DIR+EXECUTABLE+' '+runFolder)
else:
    print "Failed to compile executable"
    exit()

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
    print "Making condor script for %s"%inputfilePath
    
    #pulls out the number associated with the input file
    num = inputfilePath.split('_')[2].split('.')[0]
    
    thisOutputIdentifier = OUTPUT_IDENTIFIER+'-'+num
    
    # create a condor script to run for each .root file in the directory
    
    scriptfileName = shellFolder+'/'+thisOutputIdentifier+'.sub'
    scriptfile = open(scriptfileName, "w")
    writeSubmitScript(scriptfile, runFolder, RUN_ON_CONDOR_SCRIPT, EXECUTABLE, inputfilePath,thisOutputIdentifier)
    scriptfile.close()
    
    os.system("condor_submit "+scriptfileName)

