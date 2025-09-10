* Convert AOD to FTAG1:
Origin: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DerivationFramework

setupATLAS --quiet
asetup Athena,25.0.22
Derivation_tf.py --CA --inputAODFile input.AOD.pool.root --outputDAODFile output.pool.root --formats FTAG1

* Set up AthAnalysis:
Origin: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysis

setupATLAS --quiet
mkdir source build run
cd build
acmSetup --sourcedir=../source AthAnalysis,25.2.35
acm new_skeleton MyPackage
.. edit MyPackageAlg.cxx, CMakeLists.txt, myJobOptions.py ..
acm compile
cd ../run
athena myJobOptions.py
