# Quick Start
Setup AthAnalysis, create a skeleton, and build the custom package using:
```
source setup.sh
```
Then run the job using:
```
cd ./run
athena myJobOptions.py
```
Make sure you have modified `myJobOptions.py` to point to the path of your input file.

Use the following command to download a sample file: 
```
rucio get --nrandom 1 mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.merge.AOD.e8481_s4149_r14701_r14702_tid33629038_00
```

# Convert AOD to FTAG1:
Origin: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DerivationFramework

```
setupATLAS --quiet
asetup Athena,25.0.22
Derivation_tf.py --CA --inputAODFile input.AOD.pool.root --outputDAODFile output.pool.root --formats FTAG1
```

# Set up AthAnalysis:
Origin: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysis

```
setupATLAS --quiet
mkdir source build run
cd build
acmSetup --sourcedir=../source AthAnalysis,25.2.35
acm new_skeleton MyPackage
.. edit MyPackageAlg.cxx, CMakeLists.txt, myJobOptions.py ..
acm compile
cd ../run
athena myJobOptions.py
```
