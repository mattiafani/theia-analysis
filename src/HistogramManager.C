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
            Form("Energy difference 0%d", l),
            cfg.n_bins_h1d_Ediff,
            -1 * cfg.histo_half_range,
            cfg.histo_half_range);
        h1d_Ediff[l]->GetXaxis()->SetTitle("[MeV]");
        h1d_Ediff[l]->GetYaxis()->SetTitle("Entries [#]");
    }

    // TH2s
    for (int l = 1; l < Config::NSAMPLES; l++) {
        h2d_ioTotalEnergy[l] = new TH2D(
            Form("h2d_ioTotalEnergy_0%d", l),
            Form("Primary KE 0%d; Genie Primary KE [MeV]; Ratpac Primary KE [MeV]", l),
            1000,
            0,
            100000,
            1000,
            0,
            100000);

        h2d_ioSingleEnergies[l] = new TH2D(
            Form("h2d_ioSingleEnergies_0%d", l),
            Form("All Particles Energy 0%d; Genie KE [MeV]; Ratpac KE [MeV]", l),
            1000,
            0,
            100000,
            1000,
            0,
            100000);

        h2d_oPhotonsVsKE[l] = new TH2D(
            Form("h2d_oPhotonsVsKE_0%d", l),
            Form(" Number of photons Vs KE 0%d; KE [MeV]; Photons [#]", l),
            100,
            0,
            100000,
            5000,
            0,
            5E07);

        h2d_oCherenkovPhotonsVsKE[l] = new TH2D(
            Form("h2d_oCherenkovPhotonsVsKE_0%d", l),
            Form("Number of Cherenkov photons Vs KE 0%d; KE [MeV]; Cherenkov photons [#]", l),
            100,
            0,
            100000,
            5000,
            0,
            5E07);

        h2d_oScintPhotonsVsKE[l] = new TH2D(
            Form("h2d_oScintPhotonsVsKE_0%d", l),
            Form(" Number of scintillation photons Vs KE 0%d; KE [MeV]; Scintillation Photons [#]", l),
            100,
            0,
            100000,
            5000,
            0,
            5E07);

        h2d_oRemPhotonsVsKE[l] = new TH2D(
            Form("h2d_oRemPhotonsVsKE_0%d", l),
            Form(" Number of reemitted photons Vs KE 0%d; KE [MeV]; Reemitted Photons [#]", l),
            100,
            0,
            100000,
            100,
            0,
            1000);

        h2d_oPMTChargeVsKE[l] = new TH2D(
            Form("h2d_oPMTChargeVsKE_0%d", l),
            Form(" Number of PMT Charge Vs KE 0%d; KE [MeV]; PMT Charge [#]", l),
            100,
            0,
            100000,
            100,
            0,
            1000);

        h2d_oPEsVsKE[l] = new TH2D(
            Form("h2d_oPEsVsKE_0%d", l),
            Form(" Number of PMT PEs Vs Primary KE 0%d; KE [MeV]; PMT PEs [#]", l),
            100,
            0,
            100000,
            1000,
            0,
            10000);
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
    }
}

void HistogramManager::CloseOutputFile() {
    if (outFile) {
        outFile->Close();
        delete outFile;
        outFile = nullptr;
    }
}
