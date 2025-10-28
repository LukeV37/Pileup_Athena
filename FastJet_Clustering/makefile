SPECIALFLAGS=-O2
CFLAGS = $(SPECIALFLAGS) -I.
LFLAGS = -L.
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)
FJCFLAGS=$(shell fastjet-3.5.1/fastjet-install/bin/fastjet-config --cxxflags)
FJLIBS=$(shell fastjet-3.5.1/fastjet-install/bin/fastjet-config --libs --plugins)

RCXX=$(CFLAGS) $(ROOTCFLAGS) $(FJCFLAGS)
RLXX=$(LFLAGS) $(ROOTLIBS) $(FJLIBS)

SRC=cluster.cxx cluster_dict.cxx

%.o: %.cxx
	g++ $(RCXX) -c $< -w $(RLXX)

cluster: $(SRC:.cxx=.o)
	g++ $(SRC:.cxx=.o)  -o run_clustering $(RLXX) $(RLXX)

cluster_dict.cxx: ./LinkDef.h
	rootcling cluster_dict.cxx ./LinkDef.h

clean:
	@rm -f run_clustering
	@rm -f cluster*dict*.*
	@rm -f cluster*.o
