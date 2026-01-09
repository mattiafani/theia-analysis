#include <TDatabasePDG.h>
#include <TMath.h>

#include <iostream>

#include "../include/Config.h"
#include "../include/EventProcessor.h"

EventProcessor::InputBranches::InputBranches() {
    Reset();
}

void EventProcessor::InputBranches::Reset() {
    EvtNum = -1;
    StdHepN = 0;
    for (int i = 0; i < 100; i++) {
        StdHepPdg[i] = 0;
        StdHepStatus[i] = 0;
        for (int j = 0; j < 4; j++) {
            StdHepP4[i][j] = 0.0;
            StdHepX4[i][j] = 0.0;
        }
    }
}

EventProcessor::OutputBranches::OutputBranches() {
    evid = subev = mcid = mcparticlecount = mcpdg = nhits = mcpecount = -1;
    mcx = mcy = mcz = mcu = mcv = mcw = mct = mcke = -1;
    scintPhotons = remPhotons = cherPhotons = -1;
    digitNhits = 0;

    mcpdgs = nullptr;
    mcxs = mcys = mczs = nullptr;
    mcus = mcvs = mcws = nullptr;
    mcts = mckes = nullptr;
    hitPMTID = nullptr;
    hitPMTTime = hitPMTCharge = nullptr;
    hitPMTDigitizedTime = hitPMTDigitizedCharge = nullptr;
    digitPMTID = nullptr;
    digitTime = digitCharge = nullptr;
    digitNCrossings = nullptr;
    digitTimeOverThreshold = digitVoltageOverThreshold = nullptr;
    digitPeak = digitLocalTriggerTime = nullptr;
    digitReconNPEs = nullptr;
    mcPMTID = mcPMTNPE = nullptr;
    mcPMTCharge = nullptr;
    mcPEPMTID = nullptr;
    mcPEHitTime = mcPEFrontEndTime = nullptr;
    mcPEProcess = nullptr;
    mcPEx = mcPEy = mcPEz = nullptr;
    mcPECharge = mcPEWavelength = nullptr;
}

EventProcessor::OutputBranches::~OutputBranches() {
    // Vectors are owned by ROOT; don't delete them
}

EventProcessor::EventProcessor(FileManager& fm, HistogramManager& hm, Statistics& stats)
    : file_manager(fm), hist_manager(hm), statistics(stats), event_display(nullptr) {
}

EventProcessor::~EventProcessor() {
}

double EventProcessor::CalculateKE(double px, double py, double pz, double E) const {
    // KE = E - sqrt(p^2 + m^2) where m is rest mass
    // But GENIE gives us E (total energy), so KE = E - m = E - sqrt(E^2 - p^2)
    double p2 = px*px + py*py + pz*pz;
    double mass = TMath::Sqrt(E*E - p2);
    double KE = E - mass;
    return KE * 1000.0; // Convert GeV to MeV (input is in GeV)
}

void EventProcessor::SetupInputBranches(TTree* tree) {
    // Setup GENIE gRooTracker branches
    tree->SetBranchAddress("EvtNum", &input_branches.EvtNum);
    tree->SetBranchAddress("StdHepN", &input_branches.StdHepN);
    tree->SetBranchAddress("StdHepPdg", input_branches.StdHepPdg);
    tree->SetBranchAddress("StdHepStatus", input_branches.StdHepStatus);
    tree->SetBranchAddress("StdHepP4", input_branches.StdHepP4);
    tree->SetBranchAddress("StdHepX4", input_branches.StdHepX4);
}

void EventProcessor::EnableOnlyRequiredBranches(TTree* tree) {
    Config& cfg = Config::Instance();

    if (!cfg.GetDebugMode()) {
        // Disable all branches first
        tree->SetBranchStatus("*", 0);

        // Enable only required branches for production
        tree->SetBranchStatus("evid", 1);
        tree->SetBranchStatus("subev", 1);
        tree->SetBranchStatus("mcparticlecount", 1);
        tree->SetBranchStatus("mckes", 1);
        tree->SetBranchStatus("scintPhotons", 1);
        tree->SetBranchStatus("cherPhotons", 1);
        tree->SetBranchStatus("remPhotons", 1);
        tree->SetBranchStatus("mcPMTNPE", 1);

        // For event display in debug mode, we need additional branches
        if (cfg.GetEventDisplayMode()) {
            tree->SetBranchStatus("mcpdgs", 1);
            tree->SetBranchStatus("mcxs", 1);
            tree->SetBranchStatus("mcys", 1);
            tree->SetBranchStatus("mczs", 1);
            tree->SetBranchStatus("mcus", 1);
            tree->SetBranchStatus("mcvs", 1);
            tree->SetBranchStatus("mcws", 1);
            tree->SetBranchStatus("mcts", 1);
            tree->SetBranchStatus("mcPMTID", 1);
        }
    }
}

void EventProcessor::SetupOutputBranches(TTree* tree) {
    // Set branches - basics
    if (tree->GetBranch("evid")) {
        tree->SetBranchAddress("evid", &output_branches.evid);
    }
    if (tree->GetBranch("subev")) {
        tree->SetBranchAddress("subev", &output_branches.subev);
    }
    if (tree->GetBranch("nhits")) {
        tree->SetBranchAddress("nhits", &output_branches.nhits);
    }
    if (tree->GetBranch("mcpdg")) {
        tree->SetBranchAddress("mcpdg", &output_branches.mcpdg);
    }
    if (tree->GetBranch("mcx")) {
        tree->SetBranchAddress("mcx", &output_branches.mcx);
    }
    if (tree->GetBranch("mcy")) {
        tree->SetBranchAddress("mcy", &output_branches.mcy);
    }
    if (tree->GetBranch("mcz")) {
        tree->SetBranchAddress("mcz", &output_branches.mcz);
    }
    if (tree->GetBranch("mcu")) {
        tree->SetBranchAddress("mcu", &output_branches.mcu);
    }
    if (tree->GetBranch("mcv")) {
        tree->SetBranchAddress("mcv", &output_branches.mcv);
    }
    if (tree->GetBranch("mcw")) {
        tree->SetBranchAddress("mcw", &output_branches.mcw);
    }
    if (tree->GetBranch("mct")) {
        tree->SetBranchAddress("mct", &output_branches.mct);
    }
    if (tree->GetBranch("mcke")) {
        tree->SetBranchAddress("mcke", &output_branches.mcke);
    }
    if (tree->GetBranch("mcparticlecount")) {
        tree->SetBranchAddress("mcparticlecount", &output_branches.mcparticlecount);
    }
    if (tree->GetBranch("scintPhotons")) {
        tree->SetBranchAddress("scintPhotons", &output_branches.scintPhotons);
    }
    if (tree->GetBranch("remPhotons")) {
        tree->SetBranchAddress("remPhotons", &output_branches.remPhotons);
    }
    if (tree->GetBranch("cherPhotons")) {
        tree->SetBranchAddress("cherPhotons", &output_branches.cherPhotons);
    }
    if (tree->GetBranch("mcpecount")) {
        tree->SetBranchAddress("mcpecount", &output_branches.mcpecount);
    }

    // Set branches - include_mcparticles
    if (tree->GetBranch("mcpdgs")) {
        tree->SetBranchAddress("mcpdgs", &output_branches.mcpdgs);
    }
    if (tree->GetBranch("mcxs")) {
        tree->SetBranchAddress("mcxs", &output_branches.mcxs);
    }
    if (tree->GetBranch("mcys")) {
        tree->SetBranchAddress("mcys", &output_branches.mcys);
    }
    if (tree->GetBranch("mczs")) {
        tree->SetBranchAddress("mczs", &output_branches.mczs);
    }
    if (tree->GetBranch("mcus")) {
        tree->SetBranchAddress("mcus", &output_branches.mcus);
    }
    if (tree->GetBranch("mcvs")) {
        tree->SetBranchAddress("mcvs", &output_branches.mcvs);
    }
    if (tree->GetBranch("mcws")) {
        tree->SetBranchAddress("mcws", &output_branches.mcws);
    }
    if (tree->GetBranch("mcts")) {
        tree->SetBranchAddress("mcts", &output_branches.mcts);
    }
    if (tree->GetBranch("mckes")) {
        tree->SetBranchAddress("mckes", &output_branches.mckes);
    }

    // Set branches - include_pmthits
    if (tree->GetBranch("hitPMTID")) {
        tree->SetBranchAddress("hitPMTID", &output_branches.hitPMTID);
    }
    if (tree->GetBranch("hitPMTTime")) {
        tree->SetBranchAddress("hitPMTTime", &output_branches.hitPMTTime);
    }
    if (tree->GetBranch("hitPMTCharge")) {
        tree->SetBranchAddress("hitPMTCharge", &output_branches.hitPMTCharge);
    }

    // Set branches - include_digitizerhits
    if (tree->GetBranch("digitNhits")) {
        tree->SetBranchAddress("digitNhits", &output_branches.digitNhits);
    }
    if (tree->GetBranch("digitPMTID")) {
        tree->SetBranchAddress("digitPMTID", &output_branches.digitPMTID);
    }
    if (tree->GetBranch("digitTime")) {
        tree->SetBranchAddress("digitTime", &output_branches.digitTime);
    }
    if (tree->GetBranch("digitCharge")) {
        tree->SetBranchAddress("digitCharge", &output_branches.digitCharge);
    }
    if (tree->GetBranch("digitNCrossings")) {
        tree->SetBranchAddress("digitNCrossings", &output_branches.digitNCrossings);
    }
    if (tree->GetBranch("digitTimeOverThreshold")) {
        tree->SetBranchAddress("digitTimeOverThreshold", &output_branches.digitTimeOverThreshold);
    }
    if (tree->GetBranch("digitVoltageOverThreshold")) {
        tree->SetBranchAddress("digitVoltageOverThreshold", &output_branches.digitVoltageOverThreshold);
    }
    if (tree->GetBranch("digitPeak")) {
        tree->SetBranchAddress("digitPeak", &output_branches.digitPeak);
    }
    if (tree->GetBranch("digitLocalTriggerTime")) {
        tree->SetBranchAddress("digitLocalTriggerTime", &output_branches.digitLocalTriggerTime);
    }
    if (tree->GetBranch("digitReconNPEs")) {
        tree->SetBranchAddress("digitReconNPEs", &output_branches.digitReconNPEs);
    }

    // Set branches - others
    if (tree->GetBranch("hitPMTDigitizedTime")) {
        tree->SetBranchAddress("hitPMTDigitizedTime", &output_branches.hitPMTDigitizedTime);
    }
    if (tree->GetBranch("hitPMTDigitizedCharge")) {
        tree->SetBranchAddress("hitPMTDigitizedCharge", &output_branches.hitPMTDigitizedCharge);
    }
    if (tree->GetBranch("mcPEWavelength")) {
        tree->SetBranchAddress("mcPEWavelength", &output_branches.mcPEWavelength);
    }

    // Set branches - include_mchits
    if (tree->GetBranch("mcPMTID")) {
        tree->SetBranchAddress("mcPMTID", &output_branches.mcPMTID);
    }
    if (tree->GetBranch("mcPMTNPE")) {
        tree->SetBranchAddress("mcPMTNPE", &output_branches.mcPMTNPE);
    }
    if (tree->GetBranch("mcPMTCharge")) {
        tree->SetBranchAddress("mcPMTCharge", &output_branches.mcPMTCharge);
    }
    if (tree->GetBranch("mcPEPMTID")) {
        tree->SetBranchAddress("mcPEPMTID", &output_branches.mcPEPMTID);
    }
    if (tree->GetBranch("mcPEHitTime")) {
        tree->SetBranchAddress("mcPEHitTime", &output_branches.mcPEHitTime);
    }
    if (tree->GetBranch("mcPEFrontEndTime")) {
        tree->SetBranchAddress("mcPEFrontEndTime", &output_branches.mcPEFrontEndTime);
    }
    if (tree->GetBranch("mcPEProcess")) {
        tree->SetBranchAddress("mcPEProcess", &output_branches.mcPEProcess);
    }
    if (tree->GetBranch("mcPEx")) {
        tree->SetBranchAddress("mcPEx", &output_branches.mcPEx);
    }
    if (tree->GetBranch("mcPEy")) {
        tree->SetBranchAddress("mcPEy", &output_branches.mcPEy);
    }
    if (tree->GetBranch("mcPEz")) {
        tree->SetBranchAddress("mcPEz", &output_branches.mcPEz);
    }
    if (tree->GetBranch("mcPECharge")) {
        tree->SetBranchAddress("mcPECharge", &output_branches.mcPECharge);
    }
}

void EventProcessor::ProcessFile(int file_nr, int dataset) {
    Config& cfg = Config::Instance();
    bool debug = cfg.GetDebugMode();

    // Open files
    if (!file_manager.OpenFiles(file_nr)) {
        if (!file_manager.IsValid()) {
            statistics.n_input_file_not_readable++;
            statistics.n_output_file_not_readable++;
        }
        return;
    }

    statistics.n_valid_file_pairs++;

    TTree* input_tree = file_manager.GetInputTree();
    TTree* output_tree = file_manager.GetOutputTree();

    if (!input_tree || !output_tree) {
        std::cerr << "ERROR: Could not get trees\n";
        return;
    }

    // Setup input branches (GENIE format)
    SetupInputBranches(input_tree);

    // Setup output branches (RATPAC format)
    EnableOnlyRequiredBranches(output_tree);
    SetupOutputBranches(output_tree);

    // Load geometry for event display if needed
    if (event_display && cfg.GetEventDisplayMode()) {
        if (!event_display->IsGeometryLoaded()) {
            event_display->LoadGeometry(file_manager.GetOutputFile());
        }
    }

    Int_t n_entries_out = output_tree->GetEntries();
    Int_t n_entries_in = input_tree->GetEntries();

    if (debug) {
        std::cout << "Entries:\n";
        std::cout << " Input entries: " << n_entries_in << std::endl;
        std::cout << " Output entries: " << n_entries_out << std::endl;
    }

    // Process events
    int evt_nr = 0;
    for (Int_t i_entry_out = 0; i_entry_out < n_entries_out; i_entry_out++) {
        output_tree->GetEntry(i_entry_out);

        if (output_branches.subev != 0) continue;

        // Calculate corresponding input entry
        // Output file contains events starting at file_nr * 100 + 1
        // So first output event corresponds to input event file_nr * 100
        Int_t i_entry_in = file_nr * 100 + i_entry_out + 1;; // +1 for 1-based EvtNum
        
        if (i_entry_in >= n_entries_in) {
            if (debug) {
                std::cout << " ! WARNING: Input entry " << i_entry_in 
                          << " exceeds input entries (" << n_entries_in << ")\n";
            }
            break;
        }
        
        input_tree->GetEntry(i_entry_in);

        evt_nr++;
        statistics.n_total_entries++;

        ProcessEvent(evt_nr, dataset, i_entry_in);
    }
}

void EventProcessor::ProcessEvent(int evt_nr, int dataset, int entry_index) {
    Config& cfg = Config::Instance();
    bool debug = cfg.GetDebugMode();

    if (debug) {
        PrintEventInfo(evt_nr, entry_index);
    }

    std::vector<double> input_true_KEs;
    std::vector<double> output_true_KEs;
    
    // Count final state particles from GENIE (Status == 1)
    int n_final_state = 0;
    for (int k = 0; k < input_branches.StdHepN; ++k) {
        if (input_branches.StdHepStatus[k] == 1) {
            n_final_state++;
        }
    }
    
    input_true_KEs.reserve(n_final_state);
    output_true_KEs.reserve(output_branches.mcparticlecount);

    // Process input particles (GENIE format)
    int final_state_counter = 1;
    for (int k = 0; k < input_branches.StdHepN; ++k) {
        // Only consider final state particles (Status == 1)
        if (input_branches.StdHepStatus[k] != 1) continue;

        int pdg = input_branches.StdHepPdg[k];
        double px = input_branches.StdHepP4[k][0];
        double py = input_branches.StdHepP4[k][1];
        double pz = input_branches.StdHepP4[k][2];
        double E = input_branches.StdHepP4[k][3];
        double x = input_branches.StdHepX4[k][0];
        double y = input_branches.StdHepX4[k][1];
        double z = input_branches.StdHepX4[k][2];

        double KE = CalculateKE(px, py, pz, E);

        if (debug) {
            TDatabasePDG* pdgDB = TDatabasePDG::Instance();
            TParticlePDG* pinfo = pdgDB->GetParticle(pdg);

            const char* pdg_name = "unknown";
            double pdg_charge = NAN;
            double pdg_mass = NAN;

            if (pinfo) {
                pdg_name = pinfo->GetName();
                pdg_charge = pinfo->Charge();
                pdg_mass = pinfo->Mass();
            }

            std::cout << "   Particle " << final_state_counter
                      << " (GENIE index " << k << ")"
                      << " | PDG = " << pdg
                      << " (" << pdg_name << ")"
                      << ", Q = " << pdg_charge
                      << ", mass = " << pdg_mass << " GeV"
                      << ", Pos = (" << x << ", " << y << ", " << z << ")"
                      << ", KE = " << KE << " MeV"
                      << std::endl;
        }

        input_true_KEs.push_back(KE);
        final_state_counter++;
    }

    // Process output particles (RATPAC format)
    if (debug && output_branches.mcpdgs && output_branches.mckes) {
        std::cout << " Output File entry"
                  << " (evid=" << output_branches.evid
                  << ", subev=" << output_branches.subev << ")" << std::endl;

        if (output_branches.mcxs && output_branches.mcxs->size() > 0) {
            std::cout << "  Pos = (" << (*output_branches.mcxs)[0] << ", "
                      << (*output_branches.mcys)[0] << ", "
                      << (*output_branches.mczs)[0] << ")"
                      << ", t = " << (*output_branches.mcts)[0]
                      << "; Total particles = " << output_branches.mcparticlecount
                      << "; scintPhotons = " << output_branches.scintPhotons
                      << ", cherPhotons = " << output_branches.cherPhotons
                      << ", remPhotons = " << output_branches.remPhotons
                      << std::endl;
        }
    }

    if (output_branches.mckes) {
        for (Int_t j = 0; j < output_branches.mcparticlecount; j++) {
            if (debug && output_branches.mcpdgs) {
                TDatabasePDG* pdgDB = TDatabasePDG::Instance();
                TParticlePDG* pinfo = pdgDB->GetParticle((*output_branches.mcpdgs)[j]);

                const char* pdg_name = "unknown";
                double pdg_charge = NAN;
                double pdg_mass = NAN;

                if (pinfo) {
                    pdg_name = pinfo->GetName();
                    pdg_charge = pinfo->Charge();
                    pdg_mass = pinfo->Mass();
                }

                std::cout << "   Particle " << j
                          << " | PDG = " << (*output_branches.mcpdgs)[j]
                          << " (AKA " << pdg_name << ")"
                          << ", Q = " << pdg_charge
                          << ", mass = " << pdg_mass << " GeV"
                          << ", Pos = (" << (*output_branches.mcxs)[j] << ", "
                          << (*output_branches.mcys)[j] << ", "
                          << (*output_branches.mczs)[j] << ")"
                          << ", t = " << (*output_branches.mcts)[j]
                          << ", Dir = (" << (*output_branches.mcus)[j] << ", "
                          << (*output_branches.mcvs)[j] << ", "
                          << (*output_branches.mcws)[j] << ")"
                          << ", KE = " << (*output_branches.mckes)[j] << " MeV"
                          << std::endl;
            }

            output_true_KEs.push_back((*output_branches.mckes)[j]);
        }
    }

    // Compare input and output energies
    if (input_true_KEs.size() == output_true_KEs.size()) {
        for (size_t k = 0; k < input_true_KEs.size(); ++k) {
            hist_manager.FillSingleEnergies(dataset, input_true_KEs[k], output_true_KEs[k]);
        }
    } else {
        if (debug) {
            std::cout << " ! WARNING: number of true KE entries do not match! "
                      << "Input size: " << input_true_KEs.size()
                      << ", Output size: " << output_true_KEs.size() << std::endl;
        }
        statistics.n_events_with_KE_size_mismatch++;
    }

    double input_total_true_KE = 0.0;
    double output_total_true_KE = 0.0;
    double this_event_true_nr_PE = 0.0;

    if (!output_true_KEs.empty() && !input_true_KEs.empty()) {
        for (auto ke : input_true_KEs) input_total_true_KE += ke;
        for (auto ke : output_true_KEs) output_total_true_KE += ke;
    }

    if (output_branches.mcPMTNPE && output_branches.hitPMTID) {
        for (size_t ihit = 0; ihit < output_branches.mcPMTNPE->size(); ++ihit) {
            this_event_true_nr_PE += (*output_branches.mcPMTNPE)[ihit];
        }
    }

    hist_manager.FillTotalEnergy(dataset, input_total_true_KE, output_total_true_KE);
    hist_manager.FillPhotonsVsKE(dataset, output_total_true_KE, output_branches.scintPhotons, 
                                  output_branches.cherPhotons, output_branches.remPhotons);
    hist_manager.FillPEsVsKE(dataset, output_total_true_KE, this_event_true_nr_PE);
    hist_manager.FillEdiff(dataset, input_total_true_KE - output_total_true_KE);

    if (event_display && cfg.GetEventDisplayMode()) {
        // Note: EventDisplay might need updating too if it uses MC object
        // For now, passing nullptr for mc since we don't have DS::MC anymore
        event_display->CreateDisplay(
            evt_nr, nullptr, output_branches.mcpdgs, output_branches.mcxs, 
            output_branches.mcys, output_branches.mczs, output_branches.mcus, 
            output_branches.mcvs, output_branches.mcws, output_branches.mckes, 
            output_branches.mcparticlecount, output_branches.scintPhotons, 
            output_branches.cherPhotons, output_branches.remPhotons, 
            output_branches.mcPMTID, output_branches.mcPMTNPE);
    }

    if (debug) std::cout << std::endl;
}

void EventProcessor::PrintEventInfo(int evt_nr, int entry_index) const {
    std::cout << "Event number: " << evt_nr << std::endl;
    std::cout << " Input File entry nr: " << entry_index + 1 << std::endl;
    std::cout << "  Total particles = " << input_branches.StdHepN
              << " (GENIE format)"
              << std::endl;
}