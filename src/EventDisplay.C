#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <TH2D.h>
#include <TPad.h>
#include <TPaletteAxis.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

#include "../include/Config.h"
#include "../include/EventDisplay.h"

EventDisplay::EventDisplay()
    : xmin(-1), xmax(-1), ymin(-1), ymax(-1), zmin(-1), zmax(-1), geometry_loaded(false) {
}

EventDisplay::~EventDisplay() {
}

bool EventDisplay::LoadGeometry(TFile* output_file) {
    if (!output_file) return false;

    std::cout << " - Event display mode activated. Building geometry...\n";

    TTree* meta = (TTree*)output_file->Get("meta");
    if (!meta) {
        std::cerr << "WARNING: 'meta' tree not found in output file. "
                  << "PMT geometry unavailable; event displays will not be created.\n";
        return false;
    }

    std::vector<int>* meta_pmtId = nullptr;
    std::vector<double>* meta_pmtX = nullptr;
    std::vector<double>* meta_pmtY = nullptr;
    std::vector<double>* meta_pmtZ = nullptr;

    if (meta->GetBranch("pmtId")) meta->SetBranchAddress("pmtId", &meta_pmtId);
    if (meta->GetBranch("pmtX")) meta->SetBranchAddress("pmtX", &meta_pmtX);
    if (meta->GetBranch("pmtY")) meta->SetBranchAddress("pmtY", &meta_pmtY);
    if (meta->GetBranch("pmtZ")) meta->SetBranchAddress("pmtZ", &meta_pmtZ);

    meta->GetEntry(0);

    if (!meta_pmtId || !meta_pmtX || !meta_pmtY || !meta_pmtZ) {
        std::cerr << "WARNING: meta tree present but expected branches "
                  << "pmtId/pmtX/pmtY/pmtZ not found.\n";
        return false;
    }

    if (meta_pmtId->size() != meta_pmtX->size() ||
        meta_pmtId->size() != meta_pmtY->size() ||
        meta_pmtId->size() != meta_pmtZ->size()) {
        std::cerr << "WARNING: meta pmt vectors have inconsistent sizes.\n";
        return false;
    }

    // Load PMT positions
    for (size_t i = 0; i < meta_pmtId->size(); ++i) {
        int id = (*meta_pmtId)[i];
        double x = (*meta_pmtX)[i] * 0.001;  // Convert mm to m
        double y = (*meta_pmtY)[i] * 0.001;
        double z = (*meta_pmtZ)[i] * 0.001;

        pmtPos[id] = TVector3(x, y, z);

        if (i == 0) {
            xmin = xmax = x;
            ymin = ymax = y;
            zmin = zmax = z;
        } else {
            xmin = std::min(xmin, x);
            xmax = std::max(xmax, x);
            ymin = std::min(ymin, y);
            ymax = std::max(ymax, y);
            zmin = std::min(zmin, z);
            zmax = std::max(zmax, z);
        }
    }

    std::cout << "Geometry: Loaded " << pmtPos.size() << " PMT positions from output file\n"
              << " x:[" << xmin << "," << xmax
              << "] y:[" << ymin << "," << ymax
              << "] z:[" << zmin << "," << zmax << "]\n";

    geometry_loaded = true;
    return true;
}

void EventDisplay::AssignPMTsToFaces(std::vector<std::vector<int>>& face_pmts) {
    double tol_x = std::max(1e-3 * (xmax - xmin), 1e-6);
    double tol_y = std::max(1e-3 * (ymax - ymin), 1e-6);
    double tol_z = std::max(1e-3 * (zmax - zmin), 1e-6);

    for (const auto& kv : pmtPos) {
        int id = kv.first;
        TVector3 v = kv.second;
        double x = v.X(), y = v.Y(), z = v.Z();

        if (std::fabs(z - zmax) <= tol_z) {
            face_pmts[ZP].push_back(id);
        } else if (std::fabs(z - zmin) <= tol_z) {
            face_pmts[ZM].push_back(id);
        } else if (std::fabs(x - xmax) <= tol_x) {
            face_pmts[XP].push_back(id);
        } else if (std::fabs(x - xmin) <= tol_x) {
            face_pmts[XM].push_back(id);
        } else if (std::fabs(y - ymax) <= tol_y) {
            face_pmts[YP].push_back(id);
        } else if (std::fabs(y - ymin) <= tol_y) {
            face_pmts[YM].push_back(id);
        }
    }
}

void EventDisplay::CreateDisplay(
    int evt_nr,
    RAT::DS::MC* mc,
    std::vector<int>* mcpdgs,
    std::vector<double>* mcxs,
    std::vector<double>* mcys,
    std::vector<double>* mczs,
    std::vector<double>* mcus,
    std::vector<double>* mcvs,
    std::vector<double>* mcws,
    std::vector<double>* mckes,
    int mcparticlecount,
    double scintPhotons,
    double cherPhotons,
    double remPhotons,
    std::vector<int>* mcPMTID,
    std::vector<int>* mcPMTNPE) {
    if (!geometry_loaded) {
        std::cout << " !!! Skipping event display: PMT geometry not available.\n";
        return;
    }

    std::cout << " - Generating event display for event " << evt_nr << "...\n";

    // Build NPE map
    std::map<int, int> npe_map;
    if (!mcPMTNPE || mcPMTNPE->size() <= 0) {
        std::cerr << " !! No PE have been collected in this event; skipping display.\n\n";
        return;
    }

    if (mcPMTID && mcPMTNPE && mcPMTID->size() == mcPMTNPE->size()) {
        for (size_t i = 0; i < mcPMTID->size(); ++i) {
            npe_map[(*mcPMTID)[i]] = (*mcPMTNPE)[i];
        }
    } else {
        std::cerr << " !!! No PMT hit info available; skipping display.\n";
        return;
    }

    std::vector<std::vector<int>> face_pmts(6);
    AssignPMTsToFaces(face_pmts);

    Bool_t oldBatch = gROOT->IsBatch();
    gROOT->SetBatch(kTRUE);

    TCanvas* c = new TCanvas(Form("c_evt_%d", evt_nr), Form("Event %d", evt_nr), 1600, 900);
    c->cd();
    gStyle->SetOptStat(0);

    double left_x0 = 0.00, left_x1 = 0.78;
    double right_x0 = 0.78, right_x1 = 1.00;
    double left_y0s[5] = {0.00, 0.25, 0.50, 0.75, 1.00};
    int left_faces[4] = {XP, XM, ZP, ZM};

    std::vector<TPad*> pads(6);

    // Left side pads
    for (int i = 0; i < 4; ++i) {
        double y0 = left_y0s[i];
        double y1 = left_y0s[i + 1];
        pads[left_faces[i]] = new TPad(Form("pad_face%d", left_faces[i]), "", left_x0, y0, left_x1, y1);
        pads[left_faces[i]]->SetLeftMargin(0.04);
        pads[left_faces[i]]->SetRightMargin(0.06);
        pads[left_faces[i]]->SetBottomMargin(0.10);
        pads[left_faces[i]]->SetTopMargin(0.10);
        pads[left_faces[i]]->Draw();
    }

    // Right side pads
    pads[YP] = new TPad(Form("pad_YP_evt%d", evt_nr), "+Y side", right_x0, 0.0, right_x1, 0.25);
    pads[YM] = new TPad(Form("pad_YM_evt%d", evt_nr), "-Y side", right_x0, 0.25, right_x1, 0.50);
    TPad* textPad = new TPad(Form("pad_text_evt%d", evt_nr), "Event info", right_x0, 0.50, right_x1, 1.00);

    for (auto p : {pads[YP], pads[YM]}) {
        p->SetLeftMargin(0.08);
        p->SetRightMargin(0.15);
        p->SetBottomMargin(0.10);
        p->SetTopMargin(0.10);
        p->Draw();
    }

    textPad->SetFillColorAlpha(kWhite, 0.9);
    textPad->SetLeftMargin(0.0);
    textPad->SetRightMargin(0.1);
    textPad->SetTopMargin(0.02);
    textPad->SetBottomMargin(0.02);
    textPad->Draw();

    // Draw detector faces
    std::vector<int> draw_order = {ZP, ZM, XP, XM, YP, YM};
    std::vector<std::string> face_names = {
        "+Z (top)", "-Z (bottom)", "+X (right side)", "-X (left side)", "+Y (downstream)", "-Y (upstream)"};

    for (int face : draw_order) {
        if (!pads[face]) continue;
        pads[face]->cd();
        DrawFace(face, evt_nr, face_pmts[face], npe_map);
    }

    // Draw event info
    textPad->cd();
    TPaveText* info = new TPaveText(0.05, 0.05, 0.95, 0.95, "NDC");
    info->SetFillColor(kWhite);
    info->SetLineColor(kBlack);
    info->SetTextAlign(12);
    info->SetTextFont(42);
    info->SetTextSize(0.04);

    TText* header = info->AddText(Form("Event %d", evt_nr));
    header->SetTextAlign(22);
    header->SetTextFont(62);
    header->SetTextSize(0.05);

    // double total_edep = std::accumulate(mckes->begin(), mckes->end(), 0.0);
    double total_edep = 0.0;
    for (double ke : *mckes) {
        if (std::isfinite(ke) && ke > 0) total_edep += ke;
    }
    TText* edep_line = info->AddText(0.0, 0.90, Form("True total deposited energy = %.2f GeV", 0.001 * total_edep));
    edep_line->SetTextFont(62);

    TText* phot_line = info->AddText(0.0, 0.83, Form("True number of photons = %e", scintPhotons + cherPhotons + remPhotons));
    phot_line->SetTextFont(62);
    info->AddText(0.0, 0.78, Form("  Scintillation photons = %e", scintPhotons));
    info->AddText(0.0, 0.73, Form("  Cherenkov photons = %e", cherPhotons));
    info->AddText(0.0, 0.68, Form("  Reemitted photons = %e", remPhotons));

    TText* PE_line = info->AddText(0.0, 0.61, Form("True number of collected PEs = %.0f", (double)mcPMTNPE->size()));
    PE_line->SetTextFont(62);

    TText* part_line = info->AddText(0.0, 0.54, Form("Pos = (%.2f, %.2f, %.2f) m, Total particles = %d:", 0.001 * (*mcxs)[0], 0.001 * (*mcys)[0], 0.001 * (*mczs)[0], mcparticlecount));
    part_line->SetTextFont(62);

    // Show top particles
    TDatabasePDG* pdgDB = TDatabasePDG::Instance();
    int npar_in = mc ? mc->GetMCParticleCount() : 0;
    int max_show = 4;

    std::vector<std::tuple<double, int, TString, double, double, double>> particles;

    for (int ip = 0; ip < std::min(npar_in, max_show); ++ip) {
        RAT::DS::MCParticle* p = mc->GetMCParticle(ip);
        if (!p) continue;

        int pdg = p->GetPDGCode();
        TParticlePDG* pinfo = pdgDB->GetParticle(pdg);
        const char* pname = pinfo ? pinfo->GetName() : "unknown";

        double ke = (*mckes)[ip];
        double u = (*mcus)[ip];
        double v = (*mcvs)[ip];
        double w = (*mcws)[ip];

        particles.emplace_back(ke, pdg, pname, u, v, w);
    }

    std::sort(particles.begin(), particles.end(), [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });

    int idx = 0;
    for (const auto& [ke, pdg, pname, u, v, w] : particles) {
        TString pline = Form(" - %s (PDG=%d), Dir = (%.2f, %.2f, %.2f)",
                             pname.Data(),
                             pdg,
                             u,
                             v,
                             w);
        info->AddText(0.05, 0.49 - 0.10 * idx, pline);

        TString eline = Form("True KE = %.2f MeV", ke);
        info->AddText(0.1, 0.44 - 0.10 * idx, eline);
        ++idx;
    }

    if (npar_in > max_show) {
        info->AddText(0.0, 0.05, Form(" + %d particles not listed", npar_in - max_show));
    }

    info->Draw();
    gPad->Update();

    // Save
    std::ostringstream fname;
    fname << "./Plots/event_" << std::setw(6) << std::setfill('0') << evt_nr << ".pdf";
    c->SaveAs(fname.str().c_str());
    gROOT->SetBatch(oldBatch);

    // Cleanup
    // for (auto p : pads) {
    //     if (p) delete p;
    // }
    // delete textPad;
    // delete info;
    // delete c;
    c->Close();

    std::cout << std::endl;
}

void EventDisplay::DrawFace(int face, int evt_nr, const std::vector<int>& face_pmts, const std::map<int, int>& npe_map) {
    std::vector<std::string> face_names = {
        "+Z (top)", "-Z (bottom)", "+X (right side)", "-X (left side)", "+Y (downstream)", "-Y (upstream)"};

    double axmin, axmax, aymin, aymax;
    int nbx = 40, nby = 40;
    std::string axname, ayname;

    if (face == YP || face == YM) {
        axmin = xmin;
        axmax = xmax;
        aymin = zmin;
        aymax = zmax;
        axname = "x [m]";
        ayname = "z [m]";
        nbx = 56;
        nby = 50;
    } else if (face == XP || face == XM) {
        axmin = ymin;
        axmax = ymax;
        aymin = zmin;
        aymax = zmax;
        axname = "y [m]";
        ayname = "z [m]";
        nbx = 194;
        nby = 50;
    } else {
        axmin = ymin;
        axmax = ymax;
        aymin = xmin;
        aymax = xmax;
        axname = "y [m]";
        ayname = "x [m]";
        nbx = 194;
        nby = 56;
    }

    TH2D* h = new TH2D(Form("h_evt%d_face%d", evt_nr, face),
                       Form("%s;%s;%s", face_names[face].c_str(), axname.c_str(), ayname.c_str()),
                       nbx,
                       axmin,
                       axmax,
                       nby,
                       aymin,
                       aymax);
    h->SetStats(kFALSE);
    gStyle->SetPalette(kCividis);

    // Fill histogram
    for (int pid : face_pmts) {
        auto itpos = pmtPos.find(pid);
        if (itpos == pmtPos.end()) continue;

        TVector3 pos = itpos->second;
        double ax, ay;

        if (face == ZP || face == ZM) {
            ax = pos.Y();
            ay = pos.X();
        } else if (face == XP || face == XM) {
            ax = pos.Y();
            ay = pos.Z();
        } else {
            ax = pos.X();
            ay = pos.Z();
        }

        int npe = 0;
        auto it = npe_map.find(pid);
        if (it != npe_map.end()) npe = it->second;

        h->Fill(ax, ay, npe);
    }

    if (face == ZP || face == ZM || face == XP || face == XM) {
        h->GetYaxis()->SetTitleOffset(0.3);
        h->GetZaxis()->SetTickSize(0.01);
        h->GetYaxis()->SetTickSize(0.01);
    }

    h->Draw("COLZ");

    if (face == ZP || face == ZM || face == XP || face == XM) {
        gPad->Update();
        TPaletteAxis* palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");
        if (palette) {
            palette->SetY2NDC(0.9);
            palette->SetY1NDC(0.1);
            palette->SetX1NDC(0.942);
            palette->SetX2NDC(0.955);
        }
    }
}
