#include <iostream>
#include <vector>
#include <map>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"

int main(int argc, char *argv[])
{
    // Unpack parameters
    char *jet_input = argv[1];
    char *trk_input = argv[2];
    int algo=std::stoi(argv[3]);

    // Get input jet file
    TFile *jet_file = new TFile(TString(jet_input), "read");
    TTree *jetTree = (TTree*)jet_file->Get("myTree");

    int EventNum_calo_jet;
    std::vector<float> *calo_jet_pt=0, *calo_jet_eta=0, *calo_jet_phi=0, *calo_jet_m=0, *calo_jet_algo=0;
    jetTree->SetBranchAddress("eventnb", &EventNum_calo_jet);
    jetTree->SetBranchAddress("jet_algo", &calo_jet_algo);
    jetTree->SetBranchAddress("jet_pt", &calo_jet_pt);
    jetTree->SetBranchAddress("jet_eta", &calo_jet_eta);
    jetTree->SetBranchAddress("jet_phi", &calo_jet_phi);
    jetTree->SetBranchAddress("jet_m", &calo_jet_m);

    // Get input trk file
    TFile *trk_file = new TFile(TString(trk_input), "read");
    TTree *trkTree = (TTree*)trk_file->Get("fastjet");

    // Initialize input branches
    int EventNum_trk_jet;
    std::vector<float> *trk_jet_pt=0, *trk_jet_eta=0, *trk_jet_phi=0, *trk_jet_m=0, *trk_jet_Efrac=0, *trk_jet_Mfrac=0, *trk_jet_Wfrac=0;
    std::vector<std::vector<float> > *constituent_pt=0, *constituent_eta=0, *constituent_phi=0, *constituent_isW=0;

    trkTree->SetBranchAddress("eventnb", &EventNum_trk_jet);
    trkTree->SetBranchAddress("jet_pt", &trk_jet_pt);
    trkTree->SetBranchAddress("jet_eta", &trk_jet_eta);
    trkTree->SetBranchAddress("jet_phi", &trk_jet_phi);
    trkTree->SetBranchAddress("jet_m", &trk_jet_m);
    trkTree->SetBranchAddress("jet_Efrac", &trk_jet_Efrac);
    trkTree->SetBranchAddress("jet_Mfrac", &trk_jet_Mfrac);
    trkTree->SetBranchAddress("jet_Wfrac", &trk_jet_Wfrac);
    trkTree->SetBranchAddress("constituent_pt", &constituent_pt);
    trkTree->SetBranchAddress("constituent_eta", &constituent_eta);
    trkTree->SetBranchAddress("constituent_phi", &constituent_phi);
    trkTree->SetBranchAddress("constituent_isW", &constituent_isW);

    // Define output file
    TFile *outfile = new TFile("out"+TString(argv[3])+".root","recreate");
    TTree *outTree = new TTree("myAnalysis", "myAnalysis");
    float W_mass;
    float deltaR_leading;
    float deltaR_subleading;
    outTree->Branch("W_mass", &W_mass);
    outTree->Branch("deltaR_leading", &deltaR_leading);
    outTree->Branch("deltaR_subleading", &deltaR_subleading);

    //std::cout << "Generating Map Between Calo Events and Trk Jet Events..." << std::endl;
    std::map<int, int> calo_jet_map;
    int nEntries = jetTree->GetEntries();
    for (int event=0; event<nEntries; event++){
        //if (event%100==0) std::cout << event << " / " << nEntries << "\r" << std::flush;
        jetTree->GetEntry(event);
        calo_jet_map[EventNum_calo_jet] = event;
    }
    //std::cout << nEntries << " / " << nEntries << std::endl;
    //std::cout << std::endl;

    std::cout << "Reconstructing W Mass by Associating Trk Jets to Calo Jets..." << std::endl;
    for (int event=0; event<nEntries; event++){
        if (event%100==0){
            std::cout << event << " / " << nEntries << "\r" << std::flush;
        }
        trkTree->GetEntry(event);
        jetTree->GetEntry(calo_jet_map[EventNum_trk_jet]);

        std::vector<TLorentzVector> W_candidates;

        int num_jets = trk_jet_pt->size();
        for (int jet=0; jet<num_jets; jet++){
            int num_trks = constituent_pt->at(jet).size();
            int num_trks_fromW=0;
            for (int trk=0; trk<num_trks; trk++){
                if (std::abs(constituent_isW->at(jet).at(trk))==24) num_trks_fromW++;
            }
            if (num_trks_fromW>=2){
                TLorentzVector W_jet; W_jet.SetPtEtaPhiM(trk_jet_pt->at(jet), trk_jet_eta->at(jet), trk_jet_phi->at(jet), trk_jet_m->at(jet));
                W_candidates.push_back(W_jet);
            }
        }


        TLorentzVector W1;
        TLorentzVector W2;
        float current_max_pt=0;
        int max_idx=-99;
        int num_candidates = W_candidates.size();
        if (num_candidates<2) continue;
        for (int jet=0; jet<W_candidates.size(); jet++){
            if (W_candidates[jet].Pt()>current_max_pt){
                current_max_pt=W_candidates[jet].Pt();
                max_idx=jet;
            }
        }
        W1 = W_candidates[max_idx];
        W_candidates.erase(W_candidates.begin()+max_idx);
        current_max_pt=0;
        max_idx=-99;
        for (int jet=0; jet<W_candidates.size(); jet++){
            if (W_candidates[jet].Pt()>current_max_pt){
                current_max_pt=W_candidates[jet].Pt();
                max_idx=jet;
            }
        }
        W2 = W_candidates[max_idx];

        std::vector<TLorentzVector> calo_jets;
        int num_calo_jets = calo_jet_pt->size();
        for (int jet=0; jet<num_calo_jets; jet++){
            if (calo_jet_algo->at(jet)!=algo) continue;
            TLorentzVector calo_jet; calo_jet.SetPtEtaPhiM(calo_jet_pt->at(jet), calo_jet_eta->at(jet), calo_jet_phi->at(jet), calo_jet_m->at(jet));
            calo_jets.push_back(calo_jet);
        }

        TLorentzVector W1_calo;
        TLorentzVector W2_calo;
        float current_min_deltaR=99;
        int min_idx=-99;
        int num_calo= calo_jets.size();
        for (int jet=0; jet<calo_jets.size(); jet++){
            float deltaR = W1.DeltaR(calo_jets[jet]);
            if (deltaR<current_min_deltaR){
                current_min_deltaR=deltaR;
                min_idx=jet;
            }
        }
        W1_calo = calo_jets[min_idx];
        deltaR_leading = current_min_deltaR;
        calo_jets.erase(calo_jets.begin()+min_idx);
        current_min_deltaR=99;
        min_idx=-99;
        num_calo= calo_jets.size();
        for (int jet=0; jet<calo_jets.size(); jet++){
            float deltaR = W2.DeltaR(calo_jets[jet]);
            if (deltaR<current_min_deltaR){
                current_min_deltaR=deltaR;
                min_idx=jet;
            }
        }
        W2_calo = calo_jets[min_idx];
        deltaR_subleading = current_min_deltaR;

        W_mass = (W1_calo+W2_calo).M();
        outTree->Fill();
    }
    std::cout << nEntries << " / " << nEntries << std::endl;
    std::cout << std::endl;

    outfile->Write();
    outfile->Close();
    jet_file->Close();
    trk_file->Close();

    std::cout << "Done!" << std::endl;
    std::cout << std::endl;

    return 0;
}
