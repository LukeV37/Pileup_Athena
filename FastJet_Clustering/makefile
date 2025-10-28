SPECIALFLAGS=-O2
CFLAGS = $(SPECIALFLAGS) -I.
LFLAGS = -L.
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)
FJCFLAGS=$(shell fastjet-3.5.1/fastjet-install/bin/fastjet-config --cxxflags)
FJLIBS=$(shell fastjet-3.5.1/fastjet-install/bin/fastjet-config --libs --plugins)

RCXX=$(CFLAGS) $(ROOTCFLAGS) $(FJCFLAGS)
RLXX=$(LFLAGS) $(ROOTLIBS) $(FJLIBS)

SRC=cluster.cxx

%.o: %.cpp
	@$(CPP) $(RCXX) -c $< -w $(RLXX) $(RLXX)

cluster: $(SRC:.cpp=.o)
	g++ $(SRC:.cpp=.o)  -o run_clustering $(RLXX) $(RLXX)

cluster_dict.cpp: ./LinkDef.h
	rootcling cluster_dict.cpp ./LinkDef.h

clean:
	rm -f run_clustering
	rm -f cluster*dict*.*
