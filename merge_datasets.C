#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TObject.h>
#include <TString.h>

#include <iostream>

void merge_datasets(const char* fname) {
    const int NDATASETS = 8;

    TFile* f = TFile::Open(fname, "UPDATE");
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: could not open file " << fname << std::endl;
        return;
    }

    auto trim_title = [&](TH1* h) {
        TString title = h->GetTitle();
        if (title.Length() > 3) {
            title.Resize(title.Length() - 3);
        }
        h->SetTitle(title);
    };

    auto merge_TH2D = [&](const char* prefix, const char* newname) {
        TH2D* hmerged = nullptr;

        for (int i = 1; i <= NDATASETS; i++) {
            TString hname = Form("%s_0%d", prefix, i);
            TH2D* h = dynamic_cast<TH2D*>(f->Get(hname));

            if (!h) {
                std::cerr << "WARNING: histogram " << hname << " not found in file" << std::endl;
                continue;
            }

            if (!hmerged) {
                hmerged = (TH2D*)h->Clone(newname);
                hmerged->SetDirectory(0);
                trim_title(hmerged);
            } else {
                hmerged->Add(h);
            }
        }

        if (!hmerged) {
            std::cerr << "ERROR: no histograms found for prefix " << prefix << std::endl;
            return;
        }

        f->cd();
        hmerged->Write(newname, TObject::kOverwrite);
        std::cout << "  ✓ Merged histogram: " << newname
                  << " (entries: " << hmerged->GetEntries() << ")" << std::endl;

        delete hmerged;
    };

    auto merge_TH1D = [&](const char* prefix, const char* newname) {
        TH1D* hmerged = nullptr;

        for (int i = 1; i <= NDATASETS; i++) {
            TString hname = Form("%s_0%d", prefix, i);
            TH1D* h = dynamic_cast<TH1D*>(f->Get(hname));

            if (!h) {
                std::cerr << "WARNING: histogram " << hname << " not found in file" << std::endl;
                continue;
            }

            if (!hmerged) {
                hmerged = (TH1D*)h->Clone(newname);
                hmerged->SetDirectory(0);
                trim_title(hmerged);
            } else {
                hmerged->Add(h);
            }
        }

        if (!hmerged) {
            std::cerr << "ERROR: no histograms found for prefix " << prefix << std::endl;
            return;
        }

        f->cd();
        hmerged->Write(newname, TObject::kOverwrite);
        std::cout << "  ✓ Merged histogram: " << newname
                  << " (entries: " << hmerged->GetEntries() << ")" << std::endl;

        delete hmerged;
    };

    merge_TH2D("h2d_ioTotalEnergy", "h2d_ioTotalEnergy_combined");
    merge_TH2D("h2d_ioSingleEnergies", "h2d_ioSingleEnergies_combined");
    merge_TH2D("h2d_oPhotonsVsKE", "h2d_oPhotonsVsKE_combined");
    merge_TH2D("h2d_oCherenkovPhotonsVsKE", "h2d_oCherenkovPhotonsVsKE_combined");
    merge_TH2D("h2d_oScintPhotonsVsKE", "h2d_oScintPhotonsVsKE_combined");
    merge_TH2D("h2d_oRemPhotonsVsKE", "h2d_oRemPhotonsVsKE_combined");
    merge_TH2D("h2d_oPEsVsKE", "h2d_oPEsVsKE_combined");
    merge_TH2D("h2d_EnergyResolution_vs_E", "h2d_EnergyResolution_vs_E_combined");

    merge_TH1D("h1d_posResX", "h1d_posResX_combined");
    merge_TH1D("h1d_posResY", "h1d_posResY_combined");
    merge_TH1D("h1d_posResZ", "h1d_posResZ_combined");
    merge_TH1D("h1d_posResR", "h1d_posResR_combined");
    merge_TH1D("h1d_nParticles_input", "h1d_nParticles_input_combined");
    merge_TH1D("h1d_nParticles_output", "h1d_nParticles_output_combined");
    merge_TH1D("h1d_nParticles_matched", "h1d_nParticles_matched_combined");
    merge_TH1D("h1d_PEperMeV_vs_Energy", "h1d_PEperMeV_vs_Energy_combined");
    merge_TH1D("h1d_PEperMeV_electrons", "h1d_PEperMeV_electrons_combined");
    merge_TH1D("h1d_PEperMeV_muons", "h1d_PEperMeV_muons_combined");
    merge_TH1D("h1d_PEperMeV_hadrons", "h1d_PEperMeV_hadrons_combined");
    merge_TH1D("h1d_EnergyResolution", "h1d_EnergyResolution_combined");
    merge_TH1D("h1d_Ediff", "h1d_Ediff_combined");

    f->Close();
    delete f;

    std::cout << "All dataset combinations completed." << std::endl;
}
