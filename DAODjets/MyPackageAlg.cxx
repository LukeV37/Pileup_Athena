// MyPackage includes
#include "MyPackageAlg.h"

#include "xAODEventInfo/EventInfo.h"
#include <xAODTruth/TruthEventContainer.h>
#include <xAODTruth/TruthPileupEventContainer.h>
#include "xAODJet/JetContainer.h"
#include <xAODTracking/Vertex.h>
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include <map>

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

  m_jet_algo       = new std::vector<int>();
  m_jet_pt         = new std::vector<float>();
  m_jet_eta        = new std::vector<float>();
  m_jet_phi        = new std::vector<float>();
  m_jet_e          = new std::vector<float>();
  m_jet_m          = new std::vector<float>();

  m_myTree->Branch("jet_algo",       &m_jet_algo      );
  m_myTree->Branch("jet_pt",         &m_jet_pt        );
  m_myTree->Branch("jet_eta",        &m_jet_eta       );
  m_myTree->Branch("jet_phi",        &m_jet_phi       );
  m_myTree->Branch("jet_e",          &m_jet_e         );
  m_myTree->Branch("jet_m",          &m_jet_m         );

  CHECK( histSvc()->regTree("/MYSTREAM/myTree", m_myTree) );


  return StatusCode::SUCCESS;
}

StatusCode MyPackageAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

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

  typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
  typedef std::vector<TrackLink> TrackLinks;
  typedef ElementLink<xAOD::TruthParticleContainer> TruthLink;
  typedef std::vector<TruthLink> TruthLinks;

  const xAOD::EventInfo* ei = 0;
  CHECK(evtStore()->retrieve(ei, "EventInfo"));
  std::cout << "eventNumber=" << ei->eventNumber() << std::endl;

  runnumber   = ei->runNumber();
  eventnumber = ei->eventNumber();

  // collect various jet types

  const char* chjetcols[] = {
    "AntiKt4EMPFlowJets",
    "AntiKt4EMTopoJets",
    "AntiKt4UFOCSSKJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthDressedWZJets"
  };
  const int njetcol = sizeof(chjetcols)/sizeof(const char*);

  int jetalgo = 0;
  for (int ijetcol = 0; ijetcol<njetcol; ++ijetcol) {

    const xAOD::JetContainer* jets = 0;
    CHECK(evtStore()->retrieve(jets, chjetcols[ijetcol]));
    ATH_MSG_INFO("number of " << chjetcols[ijetcol] << ": " << jets->size());

    for (const xAOD::Jet* jet : *jets) {
      m_jet_algo->push_back(jetalgo);
      m_jet_pt->push_back(jet->pt());
      m_jet_eta->push_back(jet->eta());
      m_jet_phi->push_back(jet->phi());
      m_jet_e->push_back(jet->e());
      m_jet_m->push_back(jet->m());
    }
    ++jetalgo;

    if (ijetcol<=2) {
      for (const xAOD::Jet* jet : *jets) {
	m_jet_algo->push_back(jetalgo);
	float pt  = jet->auxdata<float>("JetEMScaleMomentum_pt");
	float eta = jet->auxdata<float>("JetEMScaleMomentum_eta");
	float phi = jet->auxdata<float>("JetEMScaleMomentum_phi");
	float m   = jet->auxdata<float>("JetEMScaleMomentum_m");
	m_jet_pt->push_back(pt);
	m_jet_eta->push_back(eta);
	m_jet_phi->push_back(phi);
	m_jet_e->push_back(sqrt(pow(m,2) + pow(pt*cosh(eta),2)));
	m_jet_m->push_back(m);
      }
      ++jetalgo;

      for (const xAOD::Jet* jet : *jets) {
	m_jet_algo->push_back(jetalgo);
	float pt  = jet->auxdata<float>("JetConstitScaleMomentum_pt");
	float eta = jet->auxdata<float>("JetConstitScaleMomentum_eta");
	float phi = jet->auxdata<float>("JetConstitScaleMomentum_phi");
	float m   = jet->auxdata<float>("JetConstitScaleMomentum_m");
	m_jet_pt->push_back(pt);
	m_jet_eta->push_back(eta);
	m_jet_phi->push_back(phi);
	m_jet_e->push_back(sqrt(pow(m,2) + pow(pt*cosh(eta),2)));
	m_jet_m->push_back(m);
      }
      ++jetalgo;
    }
  }

  m_myTree->Fill();
  m_jet_algo->clear();
  m_jet_pt->clear();
  m_jet_eta->clear();
  m_jet_phi->clear();
  m_jet_e->clear();
  m_jet_m->clear();

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


