## CSCPatterns

Framework used to manipulate and study tuples produced by `CSCDigiTuples` using C++ analyzer scripts. From within the directory, run

```bash
make
```

Which will compile a set of libraries which use ROOT objects which can be used by both C++ or Python code. 
Workflow goes as:
  * Make a `CSCDigiTree.root` via the `CSCDigiTree/` directory for whatever sample you are considering
  * After running make here, select an analyzer and run
```bash
./src/<analyzer> <input tuple> <outputfile> [<nevents.] 
```
This will create an output file associated with whichever analyzer you ran. The number of events can be specified, and is by default the entire file.
Quick python scripts which use the same classes described in the `include/` directory, as well as plotting scripts, are in the `python/` directory


 




