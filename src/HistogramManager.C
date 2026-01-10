#include <iostream>

#include "../include/Config.h"
#include "../include/HistogramManager.h"

HistogramManager::HistogramManager() : outFile(nullptr) {
    for (int i = 0; i < Config::NSAMPLES; i++) {
        
        h1d_Ediff[i] = nullptr;
        h2d_ioTotalEnergy[i] = nullptr;
        h2d_ioSingleEnergies[i] = nullptr;
        h2d_oScintPhotonsVsKE[i] = nullptr;
        h2d_oRemPhotonsVsKE[i] = nullptr;
        h2d_oPhotonsVsKE[i] = nullptr;
        h2d_oCherenkovPhotonsVsKE[i] = nullptr;
        h2d_oPMTChargeVsKE[i] = nullptr;
        h2d_oPEsVsKE[i] = nullptr;
        
        // New histos
        h1d_posResX[i] = nullptr;
        h1d_posResY[i] = nullptr;
        h1d_posResZ[i] = nullptr;
        h1d_posResR[i] = nullptr;
        h1d_nParticles_input[i] = nullptr;
        h1d_nParticles_output[i] = nullptr;
        h1d_nParticles_matched[i] = nullptr;
        h2d_PEperMeV_vs_Energy[i] = nullptr;
        h1d_PEperMeV_electrons[i] = nullptr;
        h1d_PEperMeV_muons[i] = nullptr;
        h1d_PEperMeV_hadrons[i] = nullptr;
        h2d_EnergyResolution_vs_E[i] = nullptr;
        h1d_EnergyResolution[i] = nullptr;
    }
}

HistogramManager::~HistogramManager() {
    CloseOutputFile();
}

void HistogramManager::Initialize() {
    Config& cfg = Config::Instance();

    // TH1s
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_Ediff[l] = new TH1D(
            Form("h1d_Ediff_0%d", l),
            Form("Energy difference 0%d;(E_{in} - E_{out});Entries", l),
            cfg.n_bins_h1d_Ediff,
            -2, 2);
    }

    // TH2s
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_ioTotalEnergy[l] = new TH2D(
            Form("h2d_ioTotalEnergy_0%d", l),
            Form("Total KE 0%d;GENIE Total KE [MeV];RATPAC Total KE [MeV]", l),
            1000, 0, 10000,
            1000, 0, 10000);

        h2d_ioSingleEnergies[l] = new TH2D(
            Form("h2d_ioSingleEnergies_0%d", l),
            Form("Single Particle KE 0%d;GENIE KE [MeV];RATPAC KE [MeV]", l),
            1000, 0, 10000,
            1000, 0, 10000);

        h2d_oPhotonsVsKE[l] = new TH2D(
            Form("h2d_oPhotonsVsKE_0%d", l),
            Form("Total Photons vs KE 0%d;KE [MeV];Photons", l),
            100, 0, 10000,
            5000, 0, 5E07);

        h2d_oCherenkovPhotonsVsKE[l] = new TH2D(
            Form("h2d_oCherenkovPhotonsVsKE_0%d", l),
            Form("Cherenkov Photons vs KE 0%d;KE [MeV];Cherenkov Photons", l),
            100, 0, 10000,
            5000, 0, 5E07);

        h2d_oScintPhotonsVsKE[l] = new TH2D(
            Form("h2d_oScintPhotonsVsKE_0%d", l),
            Form("Scintillation Photons vs KE 0%d;KE [MeV];Scint Photons", l),
            100, 0, 10000,
            5000, 0, 5E07);

        h2d_oRemPhotonsVsKE[l] = new TH2D(
            Form("h2d_oRemPhotonsVsKE_0%d", l),
            Form("Reemitted Photons vs KE 0%d;KE [MeV];Reemitted Photons", l),
            100, 0, 10000,
            100, 0, 100);

        h2d_oPEsVsKE[l] = new TH2D(
            Form("h2d_oPEsVsKE_0%d", l),
            Form("PEs vs KE 0%d;KE [MeV];PEs", l),
            100, 0, 100000,
            1000, 0, 10000);
    }

    // Position Resolution
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_posResX[l] = new TH1D(
            Form("h1d_posResX_0%d", l),
            Form("X Position Resolution 0%d;#Delta X [mm];Entries", l),
            200, -1000, 1000);
            
        h1d_posResY[l] = new TH1D(
            Form("h1d_posResY_0%d", l),
            Form("Y Position Resolution 0%d;#Delta Y [mm];Entries", l),
            200, -1000, 1000);
            
        h1d_posResZ[l] = new TH1D(
            Form("h1d_posResZ_0%d", l),
            Form("Z Position Resolution 0%d;#Delta Z [mm];Entries", l),
            200, -1000, 1000);
            
        h1d_posResR[l] = new TH1D(
            Form("h1d_posResR_0%d", l),
            Form("Radial Position Resolution 0%d;#Delta R [mm];Entries", l),
            200, 0, 2000);
    }

    // Particle Matching
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_nParticles_input[l] = new TH1D(
            Form("h1d_nParticles_input_0%d", l),
            Form("Input Particle Multiplicity 0%d;N_{particles};Entries", l),
            50, 0, 50);
            
        h1d_nParticles_output[l] = new TH1D(
            Form("h1d_nParticles_output_0%d", l),
            Form("Output Particle Multiplicity 0%d;N_{particles};Entries", l),
            50, 0, 50);
            
        h1d_nParticles_matched[l] = new TH1D(
            Form("h1d_nParticles_matched_0%d", l),
            Form("Matched Particles 0%d;N_{matched};Entries", l),
            50, 0, 50);
    }

    // Light Yield
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_PEperMeV_vs_Energy[l] = new TH2D(
            Form("h2d_PEperMeV_vs_Energy_0%d", l),
            Form("Light Yield vs Energy 0%d;Total KE [MeV];PE/MeV", l),
            100, 0, 10000,
            200, 0, 10);
            
        h1d_PEperMeV_electrons[l] = new TH1D(
            Form("h1d_PEperMeV_electrons_0%d", l),
            Form("Light Yield for Electrons 0%d;PE/MeV;Entries", l),
            200, 0, 10);
            
        h1d_PEperMeV_muons[l] = new TH1D(
            Form("h1d_PEperMeV_muons_0%d", l),
            Form("Light Yield for Muons 0%d;PE/MeV;Entries", l),
            200, 0, 10);
            
        h1d_PEperMeV_hadrons[l] = new TH1D(
            Form("h1d_PEperMeV_hadrons_0%d", l),
            Form("Light Yield for Hadrons 0%d;PE/MeV;Entries", l),
            200, 0, 10);
    }

    // Energy Resolution
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_EnergyResolution_vs_E[l] = new TH2D(
            Form("h2d_EnergyResolution_vs_E_0%d", l),
            Form("Energy Resolution vs Energy 0%d;E_{true} [MeV];(E_{in}-E_{out})/E_{in}", l),
            100, 0, 10000,
            200, -1, 1);
            
        h1d_EnergyResolution[l] = new TH1D(
            Form("h1d_EnergyResolution_0%d", l),
            Form("Energy Resolution 0%d;(E_{in}-E_{out})/E_{in};Entries", l),
            200, -1, 1);
    }
}

void HistogramManager::InitializeOutputFile(const char* filename) {
    outFile = TFile::Open(filename, "UPDATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "Could not open output ROOT file: " << filename << std::endl;
        outFile = new TFile(filename, "RECREATE");
    }

    CreateParameterTree();
}

void HistogramManager::CreateParameterTree() {
    Config& cfg = Config::Instance();

    Bool_t g_mode_debug = cfg.GetDebugMode();
    Bool_t g_mode_event_display = cfg.GetEventDisplayMode();
    Bool_t g_cut_nuFinal = cfg.cut_nuFinal;
    Bool_t g_cut_Ematch = cfg.cut_Ematch;
    Bool_t g_cut_toWall = cfg.cut_toWall;
    Double_t g_E_tolerance = cfg.E_tolerance;
    Double_t g_distance_toWall = cfg.distance_cut;

    TTree* t_parameters = new TTree("t_parameters", "Input parameters applied");
    t_parameters->Branch("mode_debug", &g_mode_debug, "mode_debug/O");
    t_parameters->Branch("mode_event_display", &g_mode_event_display, "mode_event_display/O");
    t_parameters->Branch("cut_nuFinal", &g_cut_nuFinal, "cut_nuFinal/O");
    t_parameters->Branch("cut_Ematch", &g_cut_Ematch, "cut_Ematch/O");
    t_parameters->Branch("cut_toWall", &g_cut_toWall, "cut_toWall/O");
    t_parameters->Branch("E_tolerance", &g_E_tolerance, "E_tolerance/D");
    t_parameters->Branch("distance_toWall", &g_distance_toWall, "distance_toWall/D");

    t_parameters->Fill();
    t_parameters->Write();
}

// Fill methods
void HistogramManager::FillSingleEnergies(int dataset, double input_KE, double output_KE) {
    if (dataset >= 1 && dataset < Config::NSAMPLES && h2d_ioSingleEnergies[dataset]) {
        h2d_ioSingleEnergies[dataset]->Fill(input_KE, output_KE);
    }
}

void HistogramManager::FillTotalEnergy(int dataset, double input_total, double output_total) {
    if (dataset >= 1 && dataset < Config::NSAMPLES && h2d_ioTotalEnergy[dataset]) {
        h2d_ioTotalEnergy[dataset]->Fill(input_total, output_total);
    }
}

void HistogramManager::FillEdiff(int dataset, double Ediff) {
    if (dataset >= 1 && dataset < Config::NSAMPLES && h1d_Ediff[dataset]) {
        h1d_Ediff[dataset]->Fill(Ediff);
    }
}

void HistogramManager::FillPhotonsVsKE(int dataset, double KE, double scint, double cher, double rem) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (h2d_oPhotonsVsKE[dataset]) {
            h2d_oPhotonsVsKE[dataset]->Fill(KE, scint + cher + rem);
        }
        if (h2d_oCherenkovPhotonsVsKE[dataset]) {
            h2d_oCherenkovPhotonsVsKE[dataset]->Fill(KE, cher);
        }
        if (h2d_oRemPhotonsVsKE[dataset]) {
            h2d_oRemPhotonsVsKE[dataset]->Fill(KE, rem);
        }
        if (h2d_oScintPhotonsVsKE[dataset]) {
            h2d_oScintPhotonsVsKE[dataset]->Fill(KE, scint);
        }
    }
}

void HistogramManager::FillPEsVsKE(int dataset, double KE, double nPE) {
    if (dataset >= 1 && dataset < Config::NSAMPLES && h2d_oPEsVsKE[dataset]) {
        h2d_oPEsVsKE[dataset]->Fill(KE, nPE);
    }
}

// New fill methods
void HistogramManager::FillPositionResolution(int dataset, double dx, double dy, double dz, double dr) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (h1d_posResX[dataset]) h1d_posResX[dataset]->Fill(dx);
        if (h1d_posResY[dataset]) h1d_posResY[dataset]->Fill(dy);
        if (h1d_posResZ[dataset]) h1d_posResZ[dataset]->Fill(dz);
        if (h1d_posResR[dataset]) h1d_posResR[dataset]->Fill(dr);
    }
}

void HistogramManager::FillParticleMatching(int dataset, int n_input, int n_output, int n_matched) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (h1d_nParticles_input[dataset]) h1d_nParticles_input[dataset]->Fill(n_input);
        if (h1d_nParticles_output[dataset]) h1d_nParticles_output[dataset]->Fill(n_output);
        if (h1d_nParticles_matched[dataset]) h1d_nParticles_matched[dataset]->Fill(n_matched);
    }
}

void HistogramManager::FillLightYield(int dataset, double total_KE, double nPE, int particle_pdg) {
    if (dataset >= 1 && dataset < Config::NSAMPLES && total_KE > 0) {
        double PE_per_MeV = nPE / total_KE;
        
        if (h2d_PEperMeV_vs_Energy[dataset]) {
            h2d_PEperMeV_vs_Energy[dataset]->Fill(total_KE, PE_per_MeV);
        }
        
        // // Classify by particle type - To be implemented
        // int abs_pdg = abs(particle_pdg);
        // if (abs_pdg == 11 && h1d_PEperMeV_electrons[dataset]) {  // electron/positron
        //     h1d_PEperMeV_electrons[dataset]->Fill(PE_per_MeV);
        // } else if (abs_pdg == 13 && h1d_PEperMeV_muons[dataset]) {  // muon
        //     h1d_PEperMeV_muons[dataset]->Fill(PE_per_MeV);
        // } else if (h1d_PEperMeV_hadrons[dataset]) {  // hadrons
        //     h1d_PEperMeV_hadrons[dataset]->Fill(PE_per_MeV);
        // }
    }
}

void HistogramManager::FillEnergyResolution(int dataset, double E_input, double E_output, double rel_diff) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (h2d_EnergyResolution_vs_E[dataset]) {
            h2d_EnergyResolution_vs_E[dataset]->Fill(E_input, rel_diff);
        }
        if (h1d_EnergyResolution[dataset]) {
            h1d_EnergyResolution[dataset]->Fill(rel_diff);
        }
    }
}

void HistogramManager::Write(int dataset) {
    if (!outFile) return;

    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        
        if (h1d_Ediff[dataset])
            outFile->WriteObject(h1d_Ediff[dataset], Form("h1d_Ediff_0%d", dataset));
        if (h2d_ioTotalEnergy[dataset])
            outFile->WriteObject(h2d_ioTotalEnergy[dataset], Form("h2d_ioTotalEnergy_0%d", dataset));
        if (h2d_ioSingleEnergies[dataset])
            outFile->WriteObject(h2d_ioSingleEnergies[dataset], Form("h2d_ioSingleEnergies_0%d", dataset));
        if (h2d_oPhotonsVsKE[dataset])
            outFile->WriteObject(h2d_oPhotonsVsKE[dataset], Form("h2d_oPhotonsVsKE_0%d", dataset));
        if (h2d_oCherenkovPhotonsVsKE[dataset])
            outFile->WriteObject(h2d_oCherenkovPhotonsVsKE[dataset], Form("h2d_oCherenkovPhotonsVsKE_0%d", dataset));
        if (h2d_oScintPhotonsVsKE[dataset])
            outFile->WriteObject(h2d_oScintPhotonsVsKE[dataset], Form("h2d_oScintPhotonsVsKE_0%d", dataset));
        if (h2d_oRemPhotonsVsKE[dataset])
            outFile->WriteObject(h2d_oRemPhotonsVsKE[dataset], Form("h2d_oRemPhotonsVsKE_0%d", dataset));
        if (h2d_oPEsVsKE[dataset])
            outFile->WriteObject(h2d_oPEsVsKE[dataset], Form("h2d_oPEsVsKE_0%d", dataset));
            
        // New histograms
        if (h1d_posResX[dataset])
            outFile->WriteObject(h1d_posResX[dataset], Form("h1d_posResX_0%d", dataset));
        if (h1d_posResY[dataset])
            outFile->WriteObject(h1d_posResY[dataset], Form("h1d_posResY_0%d", dataset));
        if (h1d_posResZ[dataset])
            outFile->WriteObject(h1d_posResZ[dataset], Form("h1d_posResZ_0%d", dataset));
        if (h1d_posResR[dataset])
            outFile->WriteObject(h1d_posResR[dataset], Form("h1d_posResR_0%d", dataset));
        if (h1d_nParticles_input[dataset])
            outFile->WriteObject(h1d_nParticles_input[dataset], Form("h1d_nParticles_input_0%d", dataset));
        if (h1d_nParticles_output[dataset])
            outFile->WriteObject(h1d_nParticles_output[dataset], Form("h1d_nParticles_output_0%d", dataset));
        if (h1d_nParticles_matched[dataset])
            outFile->WriteObject(h1d_nParticles_matched[dataset], Form("h1d_nParticles_matched_0%d", dataset));
        if (h2d_PEperMeV_vs_Energy[dataset])
            outFile->WriteObject(h2d_PEperMeV_vs_Energy[dataset], Form("h2d_PEperMeV_vs_Energy_0%d", dataset));
        // if (h1d_PEperMeV_electrons[dataset])
        //     outFile->WriteObject(h1d_PEperMeV_electrons[dataset], Form("h1d_PEperMeV_electrons_0%d", dataset));
        // if (h1d_PEperMeV_muons[dataset])
        //     outFile->WriteObject(h1d_PEperMeV_muons[dataset], Form("h1d_PEperMeV_muons_0%d", dataset));
        // if (h1d_PEperMeV_hadrons[dataset])
        //     outFile->WriteObject(h1d_PEperMeV_hadrons[dataset], Form("h1d_PEperMeV_hadrons_0%d", dataset));
        if (h2d_EnergyResolution_vs_E[dataset])
            outFile->WriteObject(h2d_EnergyResolution_vs_E[dataset], Form("h2d_EnergyResolution_vs_E_0%d", dataset));
        if (h1d_EnergyResolution[dataset])
            outFile->WriteObject(h1d_EnergyResolution[dataset], Form("h1d_EnergyResolution_0%d", dataset));
    }
}

void HistogramManager::CloseOutputFile() {
    if (outFile) {
        outFile->Close();
        delete outFile;
        outFile = nullptr;
    }
}