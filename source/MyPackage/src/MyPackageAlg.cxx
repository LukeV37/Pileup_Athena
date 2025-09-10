// MyPackage includes
#include "MyPackageAlg.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingUtilities.h"

// useful code: https://gitlab.cern.ch/atlas/athena/-/blob/main/PhysicsAnalysis/JetTagging/FlavourTaggingTests/src/PhysicsVariablePlots.cxx?ref_type=heads

MyPackageAlg::MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ){

  //declareProperty( "Property", m_nProperty = 0, "My Example Integer Property" ); //example property declaration

}


MyPackageAlg::~MyPackageAlg() {}


StatusCode MyPackageAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to configure the histsvc stream in the joboptions
  //
  //m_myHist = new TH1D("myHist","myHist",100,0,100);
  //CHECK( histSvc()->regHist("/MYSTREAM/myHist", m_myHist) ); //registers histogram to output stream
  //m_myTree = new TTree("myTree","myTree");
  //CHECK( histSvc()->regTree("/MYSTREAM/SubDirectory/myTree", m_myTree) ); //registers tree to output stream inside a sub-directory


  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed



  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //


  const xAOD::EventInfo* ei = 0;
  CHECK(evtStore()->retrieve(ei, "EventInfo"));
  ATH_MSG_INFO("eventNumber=" << ei->eventNumber());

  const xAOD::JetContainer* jets = 0;
  CHECK(evtStore()->retrieve(jets, "AntiKt4EMPFlowJets"));
  ATH_MSG_INFO("number of jets: " << jets->size());

  for (const xAOD::Jet* jet : *jets) {
    double pt = jet->pt();
    const xAOD::BTagging* bjet = xAOD::BTaggingUtilities::getBTagging(*jet);
    double pu; bjet->pu("GN2v01",pu);
    double pc; bjet->pc("GN2v01",pc);
    double pb; bjet->pb("GN2v01",pb);
    ATH_MSG_INFO("jet pt=" << pt << ", pu=" << pu << ", pc=" << pc << ", pb=" << pb);
  }

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


