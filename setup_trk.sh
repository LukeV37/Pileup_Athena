#!/bin/bash
setupATLAS --quiet
mkdir TrkAnalysis && cd TrkAnalysis
mkdir source build run
cd build
acmSetup --sourcedir=../source AthAnalysis,25.2.35
acm new_skeleton MyPackage

# Copy over files
cp ../../AODtracks/MyPackageAlg.* ../source/MyPackage/src
cp ../../AODtracks/CMakeLists.txt ../source/MyPackage/
cp ../../AODtracks/myJobOptions.py ../run/

acm compile

cd ..
