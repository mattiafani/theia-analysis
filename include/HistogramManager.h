#ifndef HISTOGRAMMANAGER_H
#define HISTOGRAMMANAGER_H

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>

#include "Config.h"

class HistogramManager {
   public:
    HistogramManager();
    ~HistogramManager();

    void Initialize();
    void InitializeOutputFile(const char* filename);

    void FillSingleEnergies(int dataset, double input_KE, double output_KE);
    void FillTotalEnergy(int dataset, double input_total, double output_total);
    void FillPhotonsVsKE(int dataset, double KE, double scint, double cher, double rem);
    void FillPEsVsKE(int dataset, double KE, double nPE);
    void FillEdiff(int dataset, double Ediff);

    // New methods
    void FillPositionResolution(int dataset, double dx, double dy, double dz, double dr);
    void FillParticleMatching(int dataset, int n_input, int n_output, int n_matched);
    void FillLightYield(int dataset, double total_KE, double nPE, int particle_pdg);
    void FillEnergyResolution(int dataset, double E_input, double E_output, double rel_diff);

    void Write(int dataset);
    void CloseOutputFile();

   private:
    TFile* outFile;

    TH1D* h1d_Ediff[Config::NSAMPLES];
    TH2D* h2d_ioTotalEnergy[Config::NSAMPLES];
    TH2D* h2d_ioSingleEnergies[Config::NSAMPLES];
    TH2D* h2d_oScintPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oRemPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oCherenkovPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oPMTChargeVsKE[Config::NSAMPLES];
    TH2D* h2d_oPEsVsKE[Config::NSAMPLES];

    // Position resolution
    TH1D* h1d_posResX[Config::NSAMPLES];
    TH1D* h1d_posResY[Config::NSAMPLES];
    TH1D* h1d_posResZ[Config::NSAMPLES];
    TH1D* h1d_posResR[Config::NSAMPLES];
    
    // Particle matching
    TH1D* h1d_nParticles_input[Config::NSAMPLES];
    TH1D* h1d_nParticles_output[Config::NSAMPLES];
    TH1D* h1d_nParticles_matched[Config::NSAMPLES];
    
    // Light yield
    TH2D* h2d_PEperMeV_vs_Energy[Config::NSAMPLES];
    TH1D* h1d_PEperMeV_electrons[Config::NSAMPLES];
    TH1D* h1d_PEperMeV_muons[Config::NSAMPLES];
    TH1D* h1d_PEperMeV_hadrons[Config::NSAMPLES];
    
    // Energy resolution
    TH2D* h2d_EnergyResolution_vs_E[Config::NSAMPLES];
    TH1D* h1d_EnergyResolution[Config::NSAMPLES];

    void CreateParameterTree();
};

#endif