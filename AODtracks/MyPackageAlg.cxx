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

  m_track_pt         = new std::vector<float>();
  m_track_eta        = new std::vector<float>();
  m_track_phi        = new std::vector<float>();
  m_track_e          = new std::vector<float>();
  m_track_q          = new std::vector<float>();
  m_track_d0         = new std::vector<float>();
  m_track_z0         = new std::vector<float>();
  m_track_pid        = new std::vector<int>();
  m_track_isPU       = new std::vector<int>();
  m_track_isW        = new std::vector<int>();

  m_myTree->Branch("track_pt",         &m_track_pt        );
  m_myTree->Branch("track_eta",        &m_track_eta       );
  m_myTree->Branch("track_phi",        &m_track_phi       );
  m_myTree->Branch("track_e",          &m_track_e         );
  m_myTree->Branch("track_q",          &m_track_q         );
  m_myTree->Branch("track_d0",         &m_track_d0        );
  m_myTree->Branch("track_z0",         &m_track_z0        );
  m_myTree->Branch("track_pid",        &m_track_pid       );
  m_myTree->Branch("track_isPU",       &m_track_isPU      );
  m_myTree->Branch("track_isW",        &m_track_isW       );

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

  /*const xAOD::TruthParticleContainer* truthParticles = 0;
  CHECK(evtStore()->retrieve(truthParticles, "TruthParticles"));
  std::cout << "number of truth particles: " << truthParticles->size() << std::endl;*/

  std::map<const xAOD::TruthParticle*, int> truthMap;

  const xAOD::TruthEventContainer* truthEvents = 0;
  CHECK(evtStore()->retrieve(truthEvents, "TruthEvents"));
  std::cout << "number of truth events: " << truthEvents->size() << std::endl;
  for (const xAOD::TruthEvent* event : *truthEvents) {
    TruthLinks links = event->truthParticleLinks();
    unsigned int nlinks = links.size();
    for (unsigned int il = 0; il<nlinks; ++il) {
      if (!links.at(il).isValid()) continue;
      const xAOD::TruthParticle* truth = (*links.at(il));
      truthMap[truth] = -1;
    }
  }

  const xAOD::TruthPileupEventContainer* truthPileupEvents = 0;
  CHECK(evtStore()->retrieve(truthPileupEvents, "TruthPileupEvents"));
  std::cout << "number of truth pileup events: " << truthPileupEvents->size() << std::endl;
  int ipu = 0;
  for (const xAOD::TruthPileupEvent* event : *truthPileupEvents) {
    TruthLinks links = event->truthParticleLinks();
    unsigned int nlinks = links.size();
    for (unsigned int il = 0; il<nlinks; ++il) {
      if (!links.at(il).isValid()) continue;
      const xAOD::TruthParticle* truth = (*links.at(il));
      truthMap[truth] = ipu;
    }
    ++ipu;
  }

  const xAOD::TrackParticleContainer* tracks = 0;
  CHECK(evtStore()->retrieve(tracks, "InDetTrackParticles"));
  std::cout << "number of tracks: " << tracks->size() << std::endl;
  for (const xAOD::TrackParticle* track : *tracks) {
    int pdgId = -999, isPU = -999, isW = 0;
    TruthLink link = track->auxdata<TruthLink>("truthParticleLink");
    if (link.isValid()) {
      const xAOD::TruthParticle* truth = *link;
      pdgId = truth->pdgId();
      std::map<const xAOD::TruthParticle*, int>::iterator it = truthMap.find(truth);
      if (it!=truthMap.end()) {
	isPU = it->second;
      }
      // trace particle origin
      if (isPU<0) { // look at HS only
	//std::cout << "Particle " << truth->pdgId();
	std::vector<const xAOD::TruthParticle*> origin;
	origin.push_back(truth);
	std::vector<const xAOD::TruthParticle*> new_origin;
	while (!origin.empty()) {
	  new_origin.clear();
	  for (unsigned int ip = 0; ip<origin.size(); ++ip) {
	    truth = origin[ip];
	    if (truth==0 || !truth->hasProdVtx()) continue;
	    int pid = truth->pdgId();
	    if (abs(pid)==24) isW = pid;
	    const xAOD::TruthVertex* vertex = truth->production_vertex();
	    if (vertex==0) continue;
	    int nin = vertex->nIncomingParticles();
	    if (nin==0) continue;
	    //std::cout << " <- "; if (nin>1) std::cout << "(";
	    for (int in = 0; in<nin; ++in) {
	      truth = vertex->incomingParticle(in);
	      if (truth==0) continue;
	      new_origin.push_back(truth);
	      //if (in>0) std::cout << " "; std::cout << truth->pdgId();
	    }
	    //if (nin>1) std::cout << ")";
	  }
	  origin.swap(new_origin);
	}
	//std::cout << std::endl;
      }
    }
    m_track_pt->push_back(track->pt());
    m_track_eta->push_back(track->eta());
    m_track_phi->push_back(track->phi());
    m_track_e->push_back(track->e());
    m_track_q->push_back(track->charge());
    m_track_d0->push_back(track->d0());
    m_track_z0->push_back(track->z0());
    m_track_pid->push_back(pdgId);
    m_track_isPU->push_back(isPU);
    m_track_isW->push_back(isW);
  }

  m_myTree->Fill();
  m_track_pt->clear();
  m_track_eta->clear();
  m_track_phi->clear();
  m_track_e->clear();
  m_track_q->clear();
  m_track_d0->clear();
  m_track_z0->clear();
  m_track_pid->clear();
  m_track_isPU->clear();
  m_track_isW->clear();

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


