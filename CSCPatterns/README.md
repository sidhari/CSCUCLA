# CSCPatterns

Framework for analyzing CSC Patterns use in the level one trigger for the CMS Detector. C++ code in ``/src`` requires a CSCDigiTree root file create using the framework a directory above this one. Given you have a CSCDigiTree, the pattern tree can be create with

```
$ make //makes shared libraries and executables
$./src/PatternFinder <inputfile> <outputfile> [<nevents>]
```
