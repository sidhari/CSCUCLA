FLAGS=-std=c++11 -g -Wall -fPIC 
#flags necessary to compile an executable which contains access to root libraries
ROOTFLAGS=`root-config --cflags --libs` -L$(ROOTSYS)/lib
CSCLIBDIR=../CSCPatterns/lib

LIBDIR=lib
SRCDIR=src
INCDIR=include
PROJLIBS=$(LIBDIR)/BremClasses_cpp.so
#TODO: surely a simpler way to handle this...
CSCPROJLIBS=$(CSCLIBDIR)/PatternFinderClasses_cpp.so $(CSCLIBDIR)/PatternFinderHelperFunctions_cpp.so $(CSCLIBDIR)/LUTClasses_cpp.so $(CSCLIBDIR)/StlCollectionProxy_cpp.so


# Assume it contains a main() function from https://gist.github.com/ghl3/3975167
all: sub-make $(PROJLIBS) $(SRCDIR)/BremGenPEstimator $(SRCDIR)/GenStudy $(SRCDIR)/SimVsRecSanityCheck


# Make shared libraries to minimize code compilation, but primarily to
# allow access of C++ classes / functions through python using
#
#  >> ROOT.gSystem.Load(<lib>)  //without .so
#
# then access with
#
#  >> ROOT.<function>(<args>)
#
$(LIBDIR)/%_cpp.so : $(SRCDIR)/%.cpp $(INCDIR)/%.h
	##### Making shared library #####
	echo ".L $<++" | root -b -l #make the shared library using ACLiC
	mv $(SRCDIR)/*_cpp* $(LIBDIR) #move it to the libs dir
	$(shell export PROJLIBS="$(PROJLIBS)")
	
clean:
	$(MAKE) -C ../CSCPatterns/ clean
	rm $(LIBDIR)/*.so $(LIBDIR)/*.pcm $(LIBDIR)/*.d
	
sub-make:
	$(MAKE) -C ../CSCPatterns/

#Makes whatever script you name, add $(ROOTLIBS) after -L$(ROOTSYS)/lib if you need it
$(SRCDIR)/%: $(SRCDIR)/%.cpp $(PROJLIBS)
	##### Making executable #####
	$(shell export LD_LIBRARY_PATH=$(CSCLIBDIR):${LD_LIBRARY_PATH})
	$(shell export LD_LIBRARY_PATH=$(LIBDIR):${LD_LIBRARY_PATH})
	g++ $(FLAGS) $(ROOTFLAGS) -L$(CSCLIBDIR) -L$(LIBDIR) $(PROJLIBS) $(CSCPROJLIBS) $< -o $@ 


	