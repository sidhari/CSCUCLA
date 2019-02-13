#!/usr/bin/env python
import ROOT as r
import sys
from glob import glob



#old LUTs are offset by 1 half strip, instead of recreating them, 
# just edit the files themselves
def convert_to_newLUT(filename):
    print "Opening file: " + filename
    
    file = open(filename,'r')
    
    newfile = open("flipped/"+filename,"w")
    for line in file:
        if len(line) < 5: break #hopefully no one will use this
        [key, entry] = line.split('~')
        
        if key == "9 ": key = "8 "
        elif key == "8 ": key = "9 "
        elif key == "7 ": key = "6 "
        elif key == "6 ": key = "7 "
        elif key == "5 ": key = "4 "
        elif key == "4 ": key = "5 "
        elif key == "3 ": key = "2 "
        elif key == "2 ": key = "3 "
        
        newline = key + "~" + entry
        newfile.write(newline)
        

    newfile.close()
    
#run with python flip_legacy_patterns.py "*Legacy.lut"
filelist = glob(sys.argv[1])
print filelist
for file in filelist:
    convert_to_newLUT(file)