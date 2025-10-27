#!/bin/bash
#setupATLAS -c el9
mkdir AthAnalysis && cd AthAnalysis
mkdir source build run
cd build
acmSetup --sourcedir=../source AthAnalysis,25.2.35

# Copy over files
cp ../../AODtracks/MyPackageAlg.* ../source/MyPackage/src
cp ../../AODtracks/CMakeLists.txt ../source/MyPackage/
cp ../../AODtracks/myJobOptions.py ../run/
