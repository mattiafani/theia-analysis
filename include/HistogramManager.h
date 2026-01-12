#ifndef HISTOGRAMMANAGER_H
#define HISTOGRAMMANAGER_H

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TLatex.h>

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

    void FillPositionResolution(int dataset, double dx, double dy, double dz, double dr);
    void FillParticleMatching(int dataset, int n_input, int n_output, int n_matched);
    void FillLightYield(int dataset, double total_KE, double nPE, int particle_pdg);
    void FillEnergyResolution(int dataset, double E_input, double E_output, double rel_diff);

    void FillPositionDistributions(int dataset, double x, double y, double z);
    void FillDirectionDistributions(int dataset, double u, double v, double w);

    void FillParticlePDG(int dataset, int pdg, bool is_input = false);
    void FillMultiplicity(int dataset, int n_electrons, int n_muons, int n_pions, int n_protons, int n_neutrons, int n_other);
    void FillAngularDistributions(int dataset, double theta, double phi, double energy, int pdg);

    void Write(int dataset);
    void CloseOutputFile();

   private:
    TFile* outFile;

    const char* GetDatasetLabel(int dataset);

    TH1D* h1d_Ediff[Config::NSAMPLES];
    TH2D* h2d_ioTotalEnergy[Config::NSAMPLES];
    TH2D* h2d_ioSingleEnergies[Config::NSAMPLES];
    TH2D* h2d_oScintPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oRemPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oCherenkovPhotonsVsKE[Config::NSAMPLES];
    TH2D* h2d_oPMTChargeVsKE[Config::NSAMPLES];
    TH2D* h2d_oPEsVsKE[Config::NSAMPLES];

    // Position distributions
    TH1D* h1d_posX[Config::NSAMPLES];
    TH1D* h1d_posY[Config::NSAMPLES];
    TH1D* h1d_posZ[Config::NSAMPLES];
    TH1D* h1d_posR[Config::NSAMPLES];
    TH1D* h1d_dirU[Config::NSAMPLES];
    TH1D* h1d_dirV[Config::NSAMPLES];
    TH1D* h1d_dirW[Config::NSAMPLES];
    TH2D* h2d_posXY[Config::NSAMPLES];
    TH2D* h2d_posXZ[Config::NSAMPLES];
    TH2D* h2d_posYZ[Config::NSAMPLES];

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

    // Particle type distributions
    TH1D* h1d_particle_pdg[Config::NSAMPLES];
    TH1D* h1d_particle_pdg_input[Config::NSAMPLES];

    // Multiplicity by particle type
    TH1D* h1d_mult_electrons[Config::NSAMPLES];
    TH1D* h1d_mult_muons[Config::NSAMPLES];
    TH1D* h1d_mult_pions[Config::NSAMPLES];
    TH1D* h1d_mult_protons[Config::NSAMPLES];
    TH1D* h1d_mult_neutrons[Config::NSAMPLES];
    TH1D* h1d_mult_other[Config::NSAMPLES];

    // Angular distributions
    TH1D* h1d_theta[Config::NSAMPLES];
    TH1D* h1d_phi[Config::NSAMPLES];
    TH1D* h1d_costheta[Config::NSAMPLES];
    TH2D* h2d_theta_phi[Config::NSAMPLES];
    TH2D* h2d_theta_vs_energy[Config::NSAMPLES];
    TH2D* h2d_phi_vs_energy[Config::NSAMPLES];

    // Angular distributions by particle type
    TH1D* h1d_theta_electrons[Config::NSAMPLES];
    TH1D* h1d_theta_muons[Config::NSAMPLES];
    TH1D* h1d_theta_pions[Config::NSAMPLES];
    TH1D* h1d_theta_protons[Config::NSAMPLES];

    void CreateParameterTree();
};

#endif