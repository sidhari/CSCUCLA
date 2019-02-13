#!/usr/bin/env python
import ROOT as r
import sys
from glob import glob



#old LUTs are offset by 1 half strip, instead of recreating them, 
# just edit the files themselves
def convert_to_newLUT(filename):
    print "Opening file: " + filename
    
    file = open(filename,'r')
    
    newfile = open("new/"+filename,"w")
    for line in file:
        if len(line) < 5: break #hopefully no one will use this
        [key, entry] = line.split('~')
        #print "key: " + key + " entry: " + entry
        #[nothing, strpos, remainingentry] = entry.split
        pos = float(entry.split(' ')[1])
        #print "pos = " + str(pos)
        pos -= 1 #SHIFT BY 1 STRIP
        
        newentry = str(pos)
        for i,ent in enumerate(entry.split(' ')):
            if i < 2: continue
            newentry += " " +ent
        
        newline = key + "~ " + newentry
        #print "newline: " + newline
        newfile.write(newline)
        
    newfile.close()
    
    
filelist = glob(sys.argv[1])
print filelist
for file in filelist:
    convert_to_newLUT(file)