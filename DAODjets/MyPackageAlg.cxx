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

  	m_myTree = new TTree("myTree","myTree");

	m_myTree->Branch("runnb",   &runnumber  );
	m_myTree->Branch("eventnb", &eventnumber);

	m_jet_pt         = new std::vector<float>();
	m_jet_eta        = new std::vector<float>();
	m_jet_phi        = new std::vector<float>();
	m_jet_m          = new std::vector<float>();
	m_jet_pu         = new std::vector<float>();
	m_jet_pc         = new std::vector<float>();
	m_jet_pb         = new std::vector<float>();

	m_myTree->Branch("jet_pt",         &m_jet_pt        );
	m_myTree->Branch("jet_eta",        &m_jet_eta       );
	m_myTree->Branch("jet_phi",        &m_jet_phi       );
	m_myTree->Branch("jet_m",          &m_jet_m       );
	m_myTree->Branch("jet_pu",         &m_jet_pu       );
	m_myTree->Branch("jet_pc",         &m_jet_pc       );
	m_myTree->Branch("jet_pb",         &m_jet_pb       );

	CHECK( histSvc()->regTree("/MYSTREAM/myTree", m_myTree) );


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

  eventnumber = ei->eventNumber();

  const xAOD::JetContainer* jets = 0;
  CHECK(evtStore()->retrieve(jets, "AntiKt4EMPFlowJets"));
  //CHECK(evtStore()->retrieve(jets, "AntiKt4EMTopoJets"));
  //CHECK(evtStore()->retrieve(jets, "AntiKt4TruthJets"));
  ATH_MSG_INFO("number of jets: " << jets->size());

  for (const xAOD::Jet* jet : *jets) {
    m_jet_pt->push_back(jet->pt());
    m_jet_eta->push_back(jet->eta());
    m_jet_phi->push_back(jet->phi());
    m_jet_m->push_back(jet->m());
    const xAOD::BTagging* bjet = xAOD::BTaggingUtilities::getBTagging(*jet);
    double pu, pc, pb;
    m_jet_pu->push_back(bjet->pu("GN2v01",pu));
    m_jet_pc->push_back(bjet->pc("GN2v01",pc));
    m_jet_pb->push_back(bjet->pb("GN2v01",pb));
  }

  m_myTree->Fill();
  m_jet_pt->clear();
  m_jet_eta->clear();
  m_jet_phi->clear();
  m_jet_m->clear();
  m_jet_pu->clear();
  m_jet_pc->clear();
  m_jet_pb->clear();

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


