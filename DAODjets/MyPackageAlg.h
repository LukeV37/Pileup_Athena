#ifndef MYPACKAGE_MYPACKAGEALG_H
#define MYPACKAGE_MYPACKAGEALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "TTree.h"

#include <vector>

class MyPackageAlg: public ::AthAnalysisAlgorithm { 
 public: 
  MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MyPackageAlg(); 

  ///uncomment and implement methods as required

                                        //IS EXECUTED:
  virtual StatusCode  initialize();     //once, before any input is loaded
  virtual StatusCode  beginInputFile(); //start of each input file, only metadata loaded
  //virtual StatusCode  firstExecute();   //once, after first eventdata is loaded (not per file)
  virtual StatusCode  execute();        //per event
  //virtual StatusCode  endInputFile();   //end of each input file
  //virtual StatusCode  metaDataStop();   //when outputMetaStore is populated by MetaDataTools
  virtual StatusCode  finalize();       //once, after all events processed
  

  ///Other useful methods provided by base class are:
  ///evtStore()        : ServiceHandle to main event data storegate
  ///inputMetaStore()  : ServiceHandle to input metadata storegate
  ///outputMetaStore() : ServiceHandle to output metadata storegate
  ///histSvc()         : ServiceHandle to output ROOT service (writing TObjects)
  ///currentFile()     : TFile* to the currently open input file
  ///retrieveMetadata(...): See twiki.cern.ch/twiki/bin/view/AtlasProtected/AthAnalysisBase#ReadingMetaDataInCpp


 private: 
  TTree* m_myTree = 0;

  int runnumber;
  int eventnumber;

  std::vector<float>* m_jet_pt;
  std::vector<float>* m_jet_eta;
  std::vector<float>* m_jet_phi;
  std::vector<float>* m_jet_m;
  std::vector<float>* m_jet_pu;
  std::vector<float>* m_jet_pc;
  std::vector<float>* m_jet_pb;
}; 

#endif //> !MYPACKAGE_MYPACKAGEALG_H
