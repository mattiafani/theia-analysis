#include <iostream>
#include <TMath.h>
#include <TLatex.h>

#include "../include/Config.h"
#include "../include/HistogramManager.h"

const char* HistogramManager::GetDatasetLabel(int dataset) {
    switch (dataset) {
        case 1:
            return "#nu_{e}";
        case 2:
            return "#nu_{#mu}";
        case 3:
            return "#bar{#nu}_{e}";
        case 4:
            return "#bar{#nu}_{#mu}";
        default:
            return "";
    }
}

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

        // Position (input and output)
        h1d_posX[i] = nullptr;
        h1d_posY[i] = nullptr;
        h1d_posZ[i] = nullptr;
        h1d_posR[i] = nullptr;
        h1d_dirU[i] = nullptr;
        h1d_dirV[i] = nullptr;
        h1d_dirW[i] = nullptr;
        h2d_posXY[i] = nullptr;
        h2d_posXZ[i] = nullptr;
        h2d_posYZ[i] = nullptr;
        h1d_posX_input[i] = nullptr;
        h1d_posY_input[i] = nullptr;
        h1d_posZ_input[i] = nullptr;
        h1d_posR_input[i] = nullptr;
        h1d_dirU_input[i] = nullptr;
        h1d_dirV_input[i] = nullptr;
        h1d_dirW_input[i] = nullptr;
        h2d_posXY_input[i] = nullptr;
        h2d_posXZ_input[i] = nullptr;
        h2d_posYZ_input[i] = nullptr;

        // Particle type distributions
        h1d_particle_pdg[i] = nullptr;
        h1d_particle_pdg_input[i] = nullptr;
        
        // Multiplicity by particle type
        h1d_mult_electrons[i] = nullptr;
        h1d_mult_muons[i] = nullptr;
        h1d_mult_pions[i] = nullptr;
        h1d_mult_protons[i] = nullptr;
        h1d_mult_neutrons[i] = nullptr;
        h1d_mult_other[i] = nullptr;
        
        // Angular distributions
        h1d_theta[i] = nullptr;
        h1d_phi[i] = nullptr;
        h1d_costheta[i] = nullptr;
        h2d_theta_phi[i] = nullptr;
        h2d_theta_vs_energy[i] = nullptr;
        h2d_phi_vs_energy[i] = nullptr;
        
        // Angular distributions by particle type
        h1d_theta_electrons[i] = nullptr;
        h1d_theta_muons[i] = nullptr;
        h1d_theta_pions[i] = nullptr;
        h1d_theta_protons[i] = nullptr;
    }
}

HistogramManager::~HistogramManager() {
    CloseOutputFile();
}

void HistogramManager::Initialize() {
    Config& cfg = Config::Instance();

    // Basic TH1s
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_Ediff[l] = new TH1D(
            Form("h1d_Ediff_0%d", l),
            Form("Energy difference 0%d (%s);E_{in} - E_{out} [MeV];Entries", l, GetDatasetLabel(l)),
            cfg.n_bins_h1d_Ediff,
            -2, 2);
    }

    // Basic TH2s
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_ioTotalEnergy[l] = new TH2D(
            Form("h2d_ioTotalEnergy_0%d", l),
            Form("Total KE 0%d (%s);GENIE Total KE [MeV];RATPAC Total KE [MeV]", l, GetDatasetLabel(l)),
            1000, 0, 10000,
            1000, 0, 10000);

        h2d_ioSingleEnergies[l] = new TH2D(
            Form("h2d_ioSingleEnergies_0%d", l),
            Form("Single Particle KE 0%d (%s);GENIE KE [MeV];RATPAC KE [MeV]", l, GetDatasetLabel(l)),
            1000, 0, 10000,
            1000, 0, 10000);

        h2d_oPhotonsVsKE[l] = new TH2D(
            Form("h2d_oPhotonsVsKE_0%d", l),
            Form("Total Photons vs KE 0%d (%s);KE [MeV];Photons", l, GetDatasetLabel(l)),
            100, 0, 10000,
            5000, 0, 1E07);

        h2d_oCherenkovPhotonsVsKE[l] = new TH2D(
            Form("h2d_oCherenkovPhotonsVsKE_0%d", l),
            Form("Cherenkov Photons vs KE 0%d (%s);KE [MeV];Cherenkov Photons", l, GetDatasetLabel(l)),
            100, 0, 10000,
            5000, 0, 1E07);

        h2d_oScintPhotonsVsKE[l] = new TH2D(
            Form("h2d_oScintPhotonsVsKE_0%d", l),
            Form("Scintillation Photons vs KE 0%d (%s);KE [MeV];Scint Photons", l, GetDatasetLabel(l)),
            100, 0, 10000,
            5000, 0, 1E07);

        h2d_oRemPhotonsVsKE[l] = new TH2D(
            Form("h2d_oRemPhotonsVsKE_0%d", l),
            Form("Reemitted Photons vs KE 0%d (%s);KE [MeV];Reemitted Photons", l, GetDatasetLabel(l)),
            100, 0, 10000,
            100, 0, 100);

        h2d_oPEsVsKE[l] = new TH2D(
            Form("h2d_oPEsVsKE_0%d", l),
            Form("PEs vs KE 0%d (%s);KE [MeV];PEs", l, GetDatasetLabel(l)),
            100, 0, 1E4,
            100, 0, 1E6);
    }

    // Position Resolution
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_posResX[l] = new TH1D(
            Form("h1d_posResX_0%d", l),
            Form("X Position Resolution 0%d (%s);#Delta X [mm];Entries", l, GetDatasetLabel(l)),
            200, -100, 100);
            
        h1d_posResY[l] = new TH1D(
            Form("h1d_posResY_0%d", l),
            Form("Y Position Resolution 0%d (%s);#Delta Y [mm];Entries", l, GetDatasetLabel(l)),
            200, -100, 100);
            
        h1d_posResZ[l] = new TH1D(
            Form("h1d_posResZ_0%d", l),
            Form("Z Position Resolution 0%d (%s);#Delta Z [mm];Entries", l, GetDatasetLabel(l)),
            200, -10, 10);
            
        h1d_posResR[l] = new TH1D(
            Form("h1d_posResR_0%d", l),
            Form("Radial Position Resolution 0%d (%s);#Delta R [mm];Entries", l, GetDatasetLabel(l)),
            200, 0, 2000);
    }

    // Particle Matching
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_nParticles_input[l] = new TH1D(
            Form("h1d_nParticles_input_0%d", l),
            Form("Input Particle Multiplicity 0%d (%s);N_{particles};Entries", l, GetDatasetLabel(l)),
            50, 0, 50);
            
        h1d_nParticles_output[l] = new TH1D(
            Form("h1d_nParticles_output_0%d", l),
            Form("Output Particle Multiplicity 0%d (%s);N_{particles};Entries", l, GetDatasetLabel(l)),
            50, 0, 50);
            
        h1d_nParticles_matched[l] = new TH1D(
            Form("h1d_nParticles_matched_0%d", l),
            Form("Matched Particles 0%d (%s);N_{matched};Entries", l, GetDatasetLabel(l)),
            50, 0, 50);
    }

    // Light Yield
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_PEperMeV_vs_Energy[l] = new TH2D(
            Form("h2d_PEperMeV_vs_Energy_0%d", l),
            Form("Light Yield vs Energy 0%d (%s);Total KE [MeV];PE/MeV", l, GetDatasetLabel(l)),
            100, 0, 20000,
            200, 0, 200);
            
        h1d_PEperMeV_electrons[l] = new TH1D(
            Form("h1d_PEperMeV_electrons_0%d", l),
            Form("Light Yield for Electrons 0%d (%s);PE/MeV;Entries", l, GetDatasetLabel(l)),
            200, 0, 200);
            
        h1d_PEperMeV_muons[l] = new TH1D(
            Form("h1d_PEperMeV_muons_0%d", l),
            Form("Light Yield for Muons 0%d (%s);PE/MeV;Entries", l, GetDatasetLabel(l)),
            200, 0, 200);
            
        h1d_PEperMeV_hadrons[l] = new TH1D(
            Form("h1d_PEperMeV_hadrons_0%d", l),
            Form("Light Yield for Hadrons 0%d (%s);PE/MeV;Entries", l, GetDatasetLabel(l)),
            200, 0, 200);
    }

    // Energy Resolution
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_EnergyResolution_vs_E[l] = new TH2D(
            Form("h2d_EnergyResolution_vs_E_0%d", l),
            Form("Energy Resolution vs Energy 0%d (%s);E_{true} [MeV];(E_{in}-E_{out})/E_{in}", l, GetDatasetLabel(l)),
            100, 0, 10000,
            200, -1, 1);
            
        h1d_EnergyResolution[l] = new TH1D(
            Form("h1d_EnergyResolution_0%d", l),
            Form("Energy Resolution 0%d (%s);(E_{in}-E_{out})/E_{in};Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
    }

    // Position Distributions (output)
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_posX[l] = new TH1D(
            Form("h1d_posX_0%d", l),
            Form("X Position Distribution 0%d (%s);X [mm];Entries", l, GetDatasetLabel(l)),
            220, -11000, 11000);
            
        h1d_posY[l] = new TH1D(
            Form("h1d_posY_0%d", l),
            Form("Y Position Distribution 0%d (%s);Y [mm];Entries", l, GetDatasetLabel(l)),
            700, -35000, 35000);
            
        h1d_posZ[l] = new TH1D(
            Form("h1d_posZ_0%d", l),
            Form("Z Position Distribution 0%d (%s);Z [mm];Entries", l, GetDatasetLabel(l)),
            180, -9000, 9000);
            
        h1d_posR[l] = new TH1D(
            Form("h1d_posR_0%d", l),
            Form("Radial Position Distribution 0%d (%s);R [mm];Entries", l, GetDatasetLabel(l)),
            600, 0, 6000);
            
        h2d_posXY[l] = new TH2D(
            Form("h2d_posXY_0%d", l),
            Form("XY Position Distribution 0%d (%s);X [mm];Y [mm]", l, GetDatasetLabel(l)),
            220, -11000, 11000,
            700, -35000, 35000);
            
        h2d_posXZ[l] = new TH2D(
            Form("h2d_posXZ_0%d", l),
            Form("XZ Position Distribution 0%d (%s);X [mm];Z [mm]", l, GetDatasetLabel(l)),
            220, -11000, 11000,
            180, -9000, 9000);
            
        h2d_posYZ[l] = new TH2D(
            Form("h2d_posYZ_0%d", l),
            Form("YZ Position Distribution 0%d (%s);Y [mm];Z [mm]", l, GetDatasetLabel(l)),
            700, -35000, 35000,
            180, -9000, 9000);
    }

    // Position Distributions (input)
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_posX_input[l] = new TH1D(
            Form("h1d_posX_input_0%d", l),
            Form("X Position Distribution (Input) 0%d (%s);X [mm];Entries", l, GetDatasetLabel(l)),
            220, -11000, 11000);
            
        h1d_posY_input[l] = new TH1D(
            Form("h1d_posY_input_0%d", l),
            Form("Y Position Distribution (Input) 0%d (%s);Y [mm];Entries", l, GetDatasetLabel(l)),
            700, -35000, 35000);
            
        h1d_posZ_input[l] = new TH1D(
            Form("h1d_posZ_input_0%d", l),
            Form("Z Position Distribution (Input) 0%d (%s);Z [mm];Entries", l, GetDatasetLabel(l)),
            180, -9000, 9000);
            
        h1d_posR_input[l] = new TH1D(
            Form("h1d_posR_input_0%d", l),
            Form("Radial Position Distribution (Input) 0%d (%s);R [mm];Entries", l, GetDatasetLabel(l)),
            600, 0, 6000);
            
        h2d_posXY_input[l] = new TH2D(
            Form("h2d_posXY_input_0%d", l),
            Form("XY Position Distribution (Input) 0%d (%s);X [mm];Y [mm]", l, GetDatasetLabel(l)),
            220, -11000, 11000,
            700, -35000, 35000);
            
        h2d_posXZ_input[l] = new TH2D(
            Form("h2d_posXZ_input_0%d", l),
            Form("XZ Position Distribution (Input) 0%d (%s);X [mm];Z [mm]", l, GetDatasetLabel(l)),
            220, -11000, 11000,
            180, -9000, 9000);
            
        h2d_posYZ_input[l] = new TH2D(
            Form("h2d_posYZ_input_0%d", l),
            Form("YZ Position Distribution (Input) 0%d (%s);Y [mm];Z [mm]", l, GetDatasetLabel(l)),
            700, -35000, 35000,
            180, -9000, 9000);
    }

    // Direction Distributions (output)
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_dirU[l] = new TH1D(
            Form("h1d_dirU_0%d", l),
            Form("U Direction Distribution 0%d (%s);Direction U;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
            
        h1d_dirV[l] = new TH1D(
            Form("h1d_dirV_0%d", l),
            Form("V Direction Distribution 0%d (%s);Direction V;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
            
        h1d_dirW[l] = new TH1D(
            Form("h1d_dirW_0%d", l),
            Form("W Direction Distribution 0%d (%s);Direction W;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
    }

    // Direction Distributions (input)
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_dirU_input[l] = new TH1D(
            Form("h1d_dirU_input_0%d", l),
            Form("U Direction Distribution (Input) 0%d (%s);Direction U;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
            
        h1d_dirV_input[l] = new TH1D(
            Form("h1d_dirV_input_0%d", l),
            Form("V Direction Distribution (Input) 0%d (%s);Direction V;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
            
        h1d_dirW_input[l] = new TH1D(
            Form("h1d_dirW_input_0%d", l),
            Form("W Direction Distribution (Input) 0%d (%s);Direction W;Entries", l, GetDatasetLabel(l)),
            200, -1, 1);
    }

    // Particle Type Distributions
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_particle_pdg[l] = new TH1D(
            Form("h1d_particle_pdg_0%d", l),
            Form("Particle PDG Distribution (Output) 0%d (%s);PDG Code;Entries", l, GetDatasetLabel(l)),
            200, -3000, 3000);
            
        h1d_particle_pdg_input[l] = new TH1D(
            Form("h1d_particle_pdg_input_0%d", l),
            Form("Particle PDG Distribution (Input) 0%d (%s);PDG Code;Entries", l, GetDatasetLabel(l)),
            200, -3000, 3000);
    }

    // Multiplicity by Particle Type
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_mult_electrons[l] = new TH1D(
            Form("h1d_mult_electrons_0%d", l),
            Form("Electron/Positron Multiplicity 0%d (%s);N_{e^{#pm}};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
            
        h1d_mult_muons[l] = new TH1D(
            Form("h1d_mult_muons_0%d", l),
            Form("Muon Multiplicity 0%d (%s);N_{#mu^{#pm}};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
            
        h1d_mult_pions[l] = new TH1D(
            Form("h1d_mult_pions_0%d", l),
            Form("Pion Multiplicity 0%d (%s);N_{#pi^{#pm,0}};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
            
        h1d_mult_protons[l] = new TH1D(
            Form("h1d_mult_protons_0%d", l),
            Form("Proton Multiplicity 0%d (%s);N_{p};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
            
        h1d_mult_neutrons[l] = new TH1D(
            Form("h1d_mult_neutrons_0%d", l),
            Form("Neutron Multiplicity 0%d (%s);N_{n};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
            
        h1d_mult_other[l] = new TH1D(
            Form("h1d_mult_other_0%d", l),
            Form("Other Particle Multiplicity 0%d (%s);N_{other};Entries", l, GetDatasetLabel(l)),
            20, 0, 20);
    }

    // Angular Distributions
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_theta[l] = new TH1D(
            Form("h1d_theta_0%d", l),
            Form("Polar Angle Distribution 0%d (%s);#theta [rad];Entries", l, GetDatasetLabel(l)),
            180, 0, TMath::Pi());
            
        h1d_phi[l] = new TH1D(
            Form("h1d_phi_0%d", l),
            Form("Azimuthal Angle Distribution 0%d (%s);#phi [rad];Entries", l, GetDatasetLabel(l)),
            180, -TMath::Pi(), TMath::Pi());
            
        h1d_costheta[l] = new TH1D(
            Form("h1d_costheta_0%d", l),
            Form("cos(#theta) Distribution 0%d (%s);cos(#theta);Entries", l, GetDatasetLabel(l)),
            100, -1, 1);
            
        h2d_theta_phi[l] = new TH2D(
            Form("h2d_theta_phi_0%d", l),
            Form("Theta vs Phi 0%d (%s);#phi [rad];#theta [rad]", l, GetDatasetLabel(l)),
            180, -TMath::Pi(), TMath::Pi(),
            180, 0, TMath::Pi());
            
        h2d_theta_vs_energy[l] = new TH2D(
            Form("h2d_theta_vs_energy_0%d", l),
            Form("Theta vs Energy 0%d (%s);KE [MeV];#theta [rad]", l, GetDatasetLabel(l)),
            100, 0, 10000,
            180, 0, TMath::Pi());
            
        h2d_phi_vs_energy[l] = new TH2D(
            Form("h2d_phi_vs_energy_0%d", l),
            Form("Phi vs Energy 0%d (%s);KE [MeV];#phi [rad]", l, GetDatasetLabel(l)),
            100, 0, 10000,
            180, -TMath::Pi(), TMath::Pi());
    }

    // Angular Distributions by Particle Type
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h1d_theta_electrons[l] = new TH1D(
            Form("h1d_theta_electrons_0%d", l),
            Form("Theta Distribution - Electrons 0%d (%s);#theta [rad];Entries", l, GetDatasetLabel(l)),
            180, 0, TMath::Pi());
            
        h1d_theta_muons[l] = new TH1D(
            Form("h1d_theta_muons_0%d", l),
            Form("Theta Distribution - Muons 0%d (%s);#theta [rad];Entries", l, GetDatasetLabel(l)),
            180, 0, TMath::Pi());
            
        h1d_theta_pions[l] = new TH1D(
            Form("h1d_theta_pions_0%d", l),
            Form("Theta Distribution - Pions 0%d (%s);#theta [rad];Entries", l, GetDatasetLabel(l)),
            180, 0, TMath::Pi());
            
        h1d_theta_protons[l] = new TH1D(
            Form("h1d_theta_protons_0%d", l),
            Form("Theta Distribution - Protons 0%d (%s);#theta [rad];Entries", l, GetDatasetLabel(l)),
            180, 0, TMath::Pi());
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

        // Classify by particle type
        int abs_pdg = abs(particle_pdg);
        if (abs_pdg == 11 && h1d_PEperMeV_electrons[dataset]) {  // electron/positron
            h1d_PEperMeV_electrons[dataset]->Fill(PE_per_MeV);
        } else if (abs_pdg == 13 && h1d_PEperMeV_muons[dataset]) {  // muon
            h1d_PEperMeV_muons[dataset]->Fill(PE_per_MeV);
        } else if (h1d_PEperMeV_hadrons[dataset]) {  // hadrons
            h1d_PEperMeV_hadrons[dataset]->Fill(PE_per_MeV);
        }
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

void HistogramManager::FillPositionDistributions(int dataset, double x, double y, double z, bool is_input) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        double r = TMath::Sqrt(x*x + y*y + z*z);
        
        if (is_input) {
            // Fill input histograms
            if (h1d_posX_input[dataset]) h1d_posX_input[dataset]->Fill(x);
            if (h1d_posY_input[dataset]) h1d_posY_input[dataset]->Fill(y);
            if (h1d_posZ_input[dataset]) h1d_posZ_input[dataset]->Fill(z);
            if (h1d_posR_input[dataset]) h1d_posR_input[dataset]->Fill(r);
            if (h2d_posXY_input[dataset]) h2d_posXY_input[dataset]->Fill(x, y);
            if (h2d_posXZ_input[dataset]) h2d_posXZ_input[dataset]->Fill(x, z);
            if (h2d_posYZ_input[dataset]) h2d_posYZ_input[dataset]->Fill(y, z);
        } else {
            // Fill output histograms
            if (h1d_posX[dataset]) h1d_posX[dataset]->Fill(x);
            if (h1d_posY[dataset]) h1d_posY[dataset]->Fill(y);
            if (h1d_posZ[dataset]) h1d_posZ[dataset]->Fill(z);
            if (h1d_posR[dataset]) h1d_posR[dataset]->Fill(r);
            if (h2d_posXY[dataset]) h2d_posXY[dataset]->Fill(x, y);
            if (h2d_posXZ[dataset]) h2d_posXZ[dataset]->Fill(x, z);
            if (h2d_posYZ[dataset]) h2d_posYZ[dataset]->Fill(y, z);
        }
    }
}

void HistogramManager::FillDirectionDistributions(int dataset, double u, double v, double w, bool is_input) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (is_input) {
            // Fill input histograms
            if (h1d_dirU_input[dataset]) h1d_dirU_input[dataset]->Fill(u);
            if (h1d_dirV_input[dataset]) h1d_dirV_input[dataset]->Fill(v);
            if (h1d_dirW_input[dataset]) h1d_dirW_input[dataset]->Fill(w);
        } else {
            // Fill output histograms
            if (h1d_dirU[dataset]) h1d_dirU[dataset]->Fill(u);
            if (h1d_dirV[dataset]) h1d_dirV[dataset]->Fill(v);
            if (h1d_dirW[dataset]) h1d_dirW[dataset]->Fill(w);
        }
    }
}

void HistogramManager::FillParticlePDG(int dataset, int pdg, bool is_input) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (is_input && h1d_particle_pdg_input[dataset]) {
            h1d_particle_pdg_input[dataset]->Fill(pdg);
        } else if (!is_input && h1d_particle_pdg[dataset]) {
            h1d_particle_pdg[dataset]->Fill(pdg);
        }
    }
}

void HistogramManager::FillMultiplicity(int dataset, int n_electrons, int n_muons, 
                                       int n_pions, int n_protons, int n_neutrons, 
                                       int n_other) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        if (h1d_mult_electrons[dataset]) h1d_mult_electrons[dataset]->Fill(n_electrons);
        if (h1d_mult_muons[dataset]) h1d_mult_muons[dataset]->Fill(n_muons);
        if (h1d_mult_pions[dataset]) h1d_mult_pions[dataset]->Fill(n_pions);
        if (h1d_mult_protons[dataset]) h1d_mult_protons[dataset]->Fill(n_protons);
        if (h1d_mult_neutrons[dataset]) h1d_mult_neutrons[dataset]->Fill(n_neutrons);
        if (h1d_mult_other[dataset]) h1d_mult_other[dataset]->Fill(n_other);
    }
}

void HistogramManager::FillAngularDistributions(int dataset, double theta, double phi, 
                                                double energy, int pdg) {
    if (dataset >= 1 && dataset < Config::NSAMPLES) {
        // Fill general angular distributions
        if (h1d_theta[dataset]) h1d_theta[dataset]->Fill(theta);
        if (h1d_phi[dataset]) h1d_phi[dataset]->Fill(phi);
        if (h1d_costheta[dataset]) h1d_costheta[dataset]->Fill(TMath::Cos(theta));
        if (h2d_theta_phi[dataset]) h2d_theta_phi[dataset]->Fill(phi, theta);
        if (h2d_theta_vs_energy[dataset]) h2d_theta_vs_energy[dataset]->Fill(energy, theta);
        if (h2d_phi_vs_energy[dataset]) h2d_phi_vs_energy[dataset]->Fill(energy, phi);
        
        // Fill particle-type specific angular distributions
        int abs_pdg = abs(pdg);
        if (abs_pdg == 11 && h1d_theta_electrons[dataset]) {
            h1d_theta_electrons[dataset]->Fill(theta);
        } else if (abs_pdg == 13 && h1d_theta_muons[dataset]) {
            h1d_theta_muons[dataset]->Fill(theta);
        } else if ((abs_pdg == 211 || pdg == 111) && h1d_theta_pions[dataset]) {
            h1d_theta_pions[dataset]->Fill(theta);
        } else if (pdg == 2212 && h1d_theta_protons[dataset]) {
            h1d_theta_protons[dataset]->Fill(theta);
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
        if (h1d_PEperMeV_electrons[dataset])
            outFile->WriteObject(h1d_PEperMeV_electrons[dataset], Form("h1d_PEperMeV_electrons_0%d", dataset));
        if (h1d_PEperMeV_muons[dataset])
            outFile->WriteObject(h1d_PEperMeV_muons[dataset], Form("h1d_PEperMeV_muons_0%d", dataset));
        if (h1d_PEperMeV_hadrons[dataset])
            outFile->WriteObject(h1d_PEperMeV_hadrons[dataset], Form("h1d_PEperMeV_hadrons_0%d", dataset));
        if (h2d_EnergyResolution_vs_E[dataset])
            outFile->WriteObject(h2d_EnergyResolution_vs_E[dataset], Form("h2d_EnergyResolution_vs_E_0%d", dataset));
        if (h1d_EnergyResolution[dataset])
            outFile->WriteObject(h1d_EnergyResolution[dataset], Form("h1d_EnergyResolution_0%d", dataset));
        if (h1d_EnergyResolution[dataset])
            outFile->WriteObject(h1d_EnergyResolution[dataset], Form("h1d_EnergyResolution_0%d", dataset));
            
        // Position distributions (output)
        if (h1d_posX[dataset])
            outFile->WriteObject(h1d_posX[dataset], Form("h1d_posX_0%d", dataset));
        if (h1d_posY[dataset])
            outFile->WriteObject(h1d_posY[dataset], Form("h1d_posY_0%d", dataset));
        if (h1d_posZ[dataset])
            outFile->WriteObject(h1d_posZ[dataset], Form("h1d_posZ_0%d", dataset));
        if (h1d_posR[dataset])
            outFile->WriteObject(h1d_posR[dataset], Form("h1d_posR_0%d", dataset));
        if (h2d_posXY[dataset])
            outFile->WriteObject(h2d_posXY[dataset], Form("h2d_posXY_0%d", dataset));
        if (h2d_posXZ[dataset])
            outFile->WriteObject(h2d_posXZ[dataset], Form("h2d_posXZ_0%d", dataset));
        if (h2d_posYZ[dataset])
            outFile->WriteObject(h2d_posYZ[dataset], Form("h2d_posYZ_0%d", dataset));

        // Position distributions (input)
        if (h1d_posX_input[dataset])
            outFile->WriteObject(h1d_posX_input[dataset], Form("h1d_posX_input_0%d", dataset));
        if (h1d_posY_input[dataset])
            outFile->WriteObject(h1d_posY_input[dataset], Form("h1d_posY_input_0%d", dataset));
        if (h1d_posZ_input[dataset])
            outFile->WriteObject(h1d_posZ_input[dataset], Form("h1d_posZ_input_0%d", dataset));
        if (h1d_posR_input[dataset])
            outFile->WriteObject(h1d_posR_input[dataset], Form("h1d_posR_input_0%d", dataset));
        if (h2d_posXY_input[dataset])
            outFile->WriteObject(h2d_posXY_input[dataset], Form("h2d_posXY_input_0%d", dataset));
        if (h2d_posXZ_input[dataset])
            outFile->WriteObject(h2d_posXZ_input[dataset], Form("h2d_posXZ_input_0%d", dataset));
        if (h2d_posYZ_input[dataset])
            outFile->WriteObject(h2d_posYZ_input[dataset], Form("h2d_posYZ_input_0%d", dataset));
        
            
        // Direction distributions (output)
        if (h1d_dirU[dataset])
            outFile->WriteObject(h1d_dirU[dataset], Form("h1d_dirU_0%d", dataset));
        if (h1d_dirV[dataset])
            outFile->WriteObject(h1d_dirV[dataset], Form("h1d_dirV_0%d", dataset));
        if (h1d_dirW[dataset])
            outFile->WriteObject(h1d_dirW[dataset], Form("h1d_dirW_0%d", dataset));

        // Direction distributions (input)
        if (h1d_dirU_input[dataset])
            outFile->WriteObject(h1d_dirU_input[dataset], Form("h1d_dirU_input_0%d", dataset));
        if (h1d_dirV_input[dataset])
            outFile->WriteObject(h1d_dirV_input[dataset], Form("h1d_dirV_input_0%d", dataset));
        if (h1d_dirW_input[dataset])
            outFile->WriteObject(h1d_dirW_input[dataset], Form("h1d_dirW_input_0%d", dataset));
        

        // Particle type distributions
        if (h1d_particle_pdg[dataset])
            outFile->WriteObject(h1d_particle_pdg[dataset], Form("h1d_particle_pdg_0%d", dataset));
        if (h1d_particle_pdg_input[dataset])
            outFile->WriteObject(h1d_particle_pdg_input[dataset], Form("h1d_particle_pdg_input_0%d", dataset));
            
        // Multiplicity by particle type
        if (h1d_mult_electrons[dataset])
            outFile->WriteObject(h1d_mult_electrons[dataset], Form("h1d_mult_electrons_0%d", dataset));
        if (h1d_mult_muons[dataset])
            outFile->WriteObject(h1d_mult_muons[dataset], Form("h1d_mult_muons_0%d", dataset));
        if (h1d_mult_pions[dataset])
            outFile->WriteObject(h1d_mult_pions[dataset], Form("h1d_mult_pions_0%d", dataset));
        if (h1d_mult_protons[dataset])
            outFile->WriteObject(h1d_mult_protons[dataset], Form("h1d_mult_protons_0%d", dataset));
        if (h1d_mult_neutrons[dataset])
            outFile->WriteObject(h1d_mult_neutrons[dataset], Form("h1d_mult_neutrons_0%d", dataset));
        if (h1d_mult_other[dataset])
            outFile->WriteObject(h1d_mult_other[dataset], Form("h1d_mult_other_0%d", dataset));

        // Angular distributions
        if (h1d_theta[dataset])
            outFile->WriteObject(h1d_theta[dataset], Form("h1d_theta_0%d", dataset));
        if (h1d_phi[dataset])
            outFile->WriteObject(h1d_phi[dataset], Form("h1d_phi_0%d", dataset));
        if (h1d_costheta[dataset])
            outFile->WriteObject(h1d_costheta[dataset], Form("h1d_costheta_0%d", dataset));
        if (h2d_theta_phi[dataset])
            outFile->WriteObject(h2d_theta_phi[dataset], Form("h2d_theta_phi_0%d", dataset));
        if (h2d_theta_vs_energy[dataset])
            outFile->WriteObject(h2d_theta_vs_energy[dataset], Form("h2d_theta_vs_energy_0%d", dataset));
        if (h2d_phi_vs_energy[dataset])
            outFile->WriteObject(h2d_phi_vs_energy[dataset], Form("h2d_phi_vs_energy_0%d", dataset));
        
        // Angular distributions by particle type
        if (h1d_theta_electrons[dataset])
            outFile->WriteObject(h1d_theta_electrons[dataset], Form("h1d_theta_electrons_0%d", dataset));
        if (h1d_theta_muons[dataset])
            outFile->WriteObject(h1d_theta_muons[dataset], Form("h1d_theta_muons_0%d", dataset));
        if (h1d_theta_pions[dataset])
            outFile->WriteObject(h1d_theta_pions[dataset], Form("h1d_theta_pions_0%d", dataset));
        if (h1d_theta_protons[dataset])
            outFile->WriteObject(h1d_theta_protons[dataset], Form("h1d_theta_protons_0%d", dataset));
    
            
    }
}

void HistogramManager::CloseOutputFile() {
    if (outFile) {
        outFile->Close();
        delete outFile;
        outFile = nullptr;
    }
}