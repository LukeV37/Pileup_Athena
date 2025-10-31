#include <iostream>
#include <cmath>
#include <vector>

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include <TRandom.h>
#include "TString.h"
#include "TLorentzVector.h"

int main(int argc, char *argv[])
{
    // Get input ROOT file
    char *input = argv[1];
    TFile *infile = new TFile(TString(input), "read");
    TTree *inTree = (TTree*)infile->Get("myTree");

    // Initialize input branches
    int EvntNum;
    std::vector<float> *trk_pt=0,*trk_eta=0,*trk_phi=0,*trk_q=0,*trk_d0=0,*trk_z0=0, *trk_e=0;
    std::vector<int> *trk_pid=0,*trk_isPU=0,*trk_isW=0;
    
    // Set address of input branches
    inTree->SetBranchAddress("eventnb", &EvntNum);
    inTree->SetBranchAddress("track_pt", &trk_pt);
    inTree->SetBranchAddress("track_eta", &trk_eta);
    inTree->SetBranchAddress("track_phi", &trk_phi);
    inTree->SetBranchAddress("track_e", &trk_e);
    inTree->SetBranchAddress("track_q", &trk_q);
    inTree->SetBranchAddress("track_d0", &trk_d0);
    inTree->SetBranchAddress("track_z0", &trk_z0);
    inTree->SetBranchAddress("track_pid", &trk_pid);
    inTree->SetBranchAddress("track_isPU", &trk_isPU);
    inTree->SetBranchAddress("track_isW", &trk_isW);

    // Define output file
    TFile *outfile = new TFile("out.root","recreate");
    TTree *outTree = new TTree("fastjet", "fastjet");

    // Initialize output branches
    std::vector<float> jet_pt, jet_eta, jet_phi, jet_m, jet_Efrac, jet_Mfrac, jet_Wfrac;
    std::vector<std::vector<float> > constituent_pt, constituent_eta, constituent_phi, constituent_q, constituent_d0, constituent_z0;
    std::vector<std::vector<int> > constituent_pid, constituent_isPU, constituent_isW;

    outTree->Branch("eventnb", &EvntNum);
    outTree->Branch("jet_pt", &jet_pt);
    outTree->Branch("jet_eta", &jet_eta);
    outTree->Branch("jet_phi", &jet_phi);
    outTree->Branch("jet_m", &jet_m);
    outTree->Branch("jet_Efrac", &jet_Efrac);
    outTree->Branch("jet_Mfrac", &jet_Mfrac);
    outTree->Branch("jet_Wfrac", &jet_Wfrac);
    outTree->Branch("constituent_pt", &constituent_pt);
    outTree->Branch("constituent_eta", &constituent_eta);
    outTree->Branch("constituent_phi", &constituent_phi);
    outTree->Branch("constituent_q", &constituent_q);
    outTree->Branch("constituent_d0", &constituent_d0);
    outTree->Branch("constituent_z0", &constituent_z0);
    outTree->Branch("constituent_pid", &constituent_pid);
    outTree->Branch("constituent_isPU", &constituent_isPU);
    outTree->Branch("constituent_isW", &constituent_isW);
    outTree->Branch("all_trk_pt", &trk_pt);
    outTree->Branch("all_trk_eta", &trk_eta);
    outTree->Branch("all_trk_phi", &trk_phi);
    outTree->Branch("all_trk_q", &trk_q);
    outTree->Branch("all_trk_d0", &trk_d0);
    outTree->Branch("all_trk_z0", &trk_z0);
    outTree->Branch("all_trk_pid", &trk_pid);
    outTree->Branch("all_trk_isPU", &trk_isPU);
    outTree->Branch("all_trk_isW", &trk_isW);

    int nEntries = inTree->GetEntries();
    for (int event=0; event<nEntries; event++){
        if (event%100==0) std::cout << "Processing Event: " << event << "\r" << std::flush;
        inTree->GetEntry(event);

        std::vector<fastjet::PseudoJet> fastjet_particles;

        int particle=0;

        // Store particles in fasjet::PsuedoJet objects and set the index
        int num_particles = trk_pt->size();
        for (int particle=0; particle<num_particles; particle++){
            TLorentzVector fj_vec; fj_vec.SetPtEtaPhiE(trk_pt->at(particle),trk_eta->at(particle),trk_phi->at(particle),trk_e->at(particle));
            fastjet::PseudoJet fj(fj_vec.Px(), fj_vec.Py(), fj_vec.Pz(), fj_vec.E());
            fj.set_user_index(particle++);
            fastjet_particles.push_back(fj);
        }

        // Cluster particles and pick up hardest largeR jet
        float R_small = 0.4;
        float pTmin = 13000; // GeV
        fastjet::JetDefinition jetDef = fastjet::JetDefinition(fastjet::antikt_algorithm, R_small, fastjet::E_scheme, fastjet::Best);
        fastjet::ClusterSequence clustSeq_large(fastjet_particles, jetDef);
        auto jets = fastjet::sorted_by_pt( clustSeq_large.inclusive_jets(pTmin) );

        for (auto jet:jets){
            jet_pt.push_back(jet.pt());
            jet_eta.push_back(jet.eta());
            jet_phi.push_back(jet.phi());
            jet_m.push_back(jet.m());

            TLorentzVector vtot, vhs, vW;

            std::vector<float> trk_pt_tmp, trk_eta_tmp, trk_phi_tmp, trk_q_tmp,trk_d0_tmp, trk_z0_tmp;
            std::vector<int> trk_pid_tmp, trk_isPU_tmp, trk_isW_tmp;

            for (auto trk:jet.constituents()){
                int idx = trk.user_index();

                trk_pt_tmp.push_back(trk_pt->at(idx));
                trk_eta_tmp.push_back(trk_eta->at(idx));
                trk_phi_tmp.push_back(trk_phi->at(idx));
                trk_q_tmp.push_back(trk_q->at(idx));
                trk_d0_tmp.push_back(trk_d0->at(idx));
                trk_z0_tmp.push_back(trk_z0->at(idx));
                trk_pid_tmp.push_back(trk_pid->at(idx));
                trk_isPU_tmp.push_back(trk_isPU->at(idx));
                trk_isW_tmp.push_back(trk_isW->at(idx));

                TLorentzVector v; v.SetPtEtaPhiE(trk_pt->at(idx),trk_eta->at(idx),trk_phi->at(idx),trk_e->at(idx));
                vtot += v;
                if (trk_isPU->at(idx)<0){
                    vhs += v;
                }
                if (abs(trk_isW->at(idx))==24){
                    vW += v;
                }
            }
            constituent_pt.push_back(trk_pt_tmp);
            constituent_eta.push_back(trk_eta_tmp);
            constituent_phi.push_back(trk_phi_tmp);
            constituent_q.push_back(trk_q_tmp);
            constituent_d0.push_back(trk_d0_tmp);
            constituent_z0.push_back(trk_z0_tmp);
            constituent_pid.push_back(trk_pid_tmp);
            constituent_isPU.push_back(trk_isPU_tmp);
            constituent_isW.push_back(trk_isW_tmp);

            double true_Efrac = vtot.E(); if (true_Efrac>0) true_Efrac = vhs.E()/true_Efrac;
            double true_Mfrac = vtot.M(); if (true_Mfrac>0) true_Mfrac = vhs.M()/true_Mfrac;
            double true_Wfrac = vtot.E(); if (true_Wfrac>0) true_Wfrac = vW.E()/true_Wfrac;
            jet_Efrac.push_back(true_Efrac);
            jet_Mfrac.push_back(true_Mfrac);
            jet_Wfrac.push_back(true_Wfrac);
        }
        outTree->Fill();
        jet_pt.clear(); jet_eta.clear(); jet_phi.clear(); jet_m.clear();
        jet_Efrac.clear(); jet_Mfrac.clear(); jet_Wfrac.clear();
        constituent_pt.clear(); constituent_eta.clear(); constituent_phi.clear(); constituent_q.clear(); constituent_d0.clear(); constituent_z0.clear();
        constituent_pid.clear(); constituent_isPU.clear(); constituent_isW.clear();
    }

    outfile->Write();
    outfile->Close();
    infile->Close();

    return 0;
}
