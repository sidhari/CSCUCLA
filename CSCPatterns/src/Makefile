CXX=`root-config --cxx`
CXXFLAGS=`root-config --cflags`
LDFLAGS=`root-config --ldflags`
LDLIBS=`root-config --glibs`
ROOTLIBS='-lRooFit -lHtml -lMinuit -lRooFitCore -lRooStats -lHistFactory'


# Assume it contains a main() function from https://gist.github.com/ghl3/3975167

all: PatternFinder

clean:
	rm *.so *.d *.pcm *dict.cxx*


#Makes whatever script you name
% : %.cpp
	g++ -g -Wall `root-config --cflags --libs` -L$(ROOTSYS)/lib $(ROOTLIBS) $? -o $@
