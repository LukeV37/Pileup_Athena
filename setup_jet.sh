#!/bin/bash
setupATLAS --quiet
mkdir JetAnalysis && cd JetAnalysis
mkdir source build run
cd build
acmSetup --sourcedir=../source AthAnalysis,25.2.35
acm new_skeleton MyPackage

# Copy over files
cp ../../DAODjets/MyPackageAlg.* ../source/MyPackage/src
cp ../../DAODjets/CMakeLists.txt ../source/MyPackage/
cp ../../DAODjets/myJobOptions.py ../run/

acm compile

cd ..
