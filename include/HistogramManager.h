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

    void CreateParameterTree();
};

#endif
