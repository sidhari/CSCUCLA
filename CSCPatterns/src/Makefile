CXX=`root-config --cxx`
CXXFLAGS=`root-config --cflags`
LDFLAGS=`root-config --ldflags`
LDLIBS=`root-config --glibs`
ROOTLIBS='-lRooFit -lHtml -lMinuit -lRooFitCore -lRooStats -lHistFactory'


# Assume it contains a main() function from https://gist.github.com/ghl3/3975167

all: PatternFinder

clean:
	rm *.so *.d *.pcm *.cxx*

#Makes whatever script you name, add $(ROOTLIBS) after -L$(ROOTSYS)/lib if you need it
% : %.cpp
	g++ -std=c++0x -g -Wall `root-config --cflags --libs` -L$(ROOTSYS)/lib $? -o $@
