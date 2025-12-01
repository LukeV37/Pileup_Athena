all: W_mass_reconstruction.cxx LinkDef.h
	rootcling -f MyDict.cxx LinkDef.h
	g++ W_mass_reconstruction.cxx MyDict.cxx -o Reconstruction.out $(shell root-config --cflags --libs)

clean:
	rm -f *.out
	rm -f MyDict*


