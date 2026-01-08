#ifdef __CLING__
R__ADD_LIBRARY_PATH($RATROOT / lib)
R__LOAD_LIBRARY(libRATEvent)
R__LOAD_LIBRARY(libPhysics)
#endif

#include <iostream>

#include "RAT/DS/MC.hh"
#include "RAT/DS/MCParticle.hh"
#include "RAT/DS/Root.hh"
#include "include/Config.h"
#include "include/EventDisplay.h"
#include "include/EventProcessor.h"
#include "include/FileManager.h"
#include "include/HistogramManager.h"
#include "include/Statistics.h"

extern "C" void validate(int flavor = 1, bool debug = true, bool event_display = true, int start_file = 0) {
    Config& config = Config::Instance();
    config.SetDebugMode(debug);
    config.SetEventDisplayMode(event_display && debug);  // Event display only works in debug mode

    int n_files, n_loop_start, loop_end;
    if (debug) {
        n_files = 1;
        n_loop_start = start_file;
        loop_end = n_loop_start + n_files;
        std::cout << "\n - Debug mode: processing " << n_files
                  << " file(s), starting from file nr: " << n_loop_start << std::endl;
    } else {
        // Production mode: process all 1000 files per flavor
        // (100,000 events ÷ 100 events per file = 1000 files)
        n_files = 1000;
        n_loop_start = 0;
        loop_end = n_files;
    }

    TString flavor_name = Config::GetFlavorName(flavor);
    if (flavor_name == "") {
        std::cerr << "Error: Invalid flavor number! Use 1-4 (1=nue, 2=numu, 3=anue, 4=anumu)" << std::endl;
        return;
    }

    std::cout << "\nProcessing flavor: " << flavor_name << " (flavor " << flavor << ")" << std::endl;

    Statistics stats;
    FileManager file_mgr;
    HistogramManager hist_mgr;
    EventDisplay* evt_display = nullptr;

    file_mgr.SetupPaths(flavor_name.Data());

    hist_mgr.Initialize();
    TString output_name = TString::Format("validate_%s_%s.root", 
                                          flavor_name.Data(), 
                                          config.GetTimestamp().Data());
    hist_mgr.InitializeOutputFile(output_name.Data());

    if (config.GetEventDisplayMode()) {
        evt_display = new EventDisplay();
    }

    EventProcessor processor(file_mgr, hist_mgr, stats);
    if (evt_display) {
        processor.SetEventDisplay(evt_display);
    }

    for (int i = n_loop_start; i < loop_end; i++) {
        if (!debug && i % 100 == 0) {
            std::cout << "\n: Reading " << flavor_name << " I/O files nr " << i << " out of " << n_files << "..." << std::endl;
        }

        processor.ProcessFile(i, flavor);
    }

    hist_mgr.Write(flavor);
    hist_mgr.CloseOutputFile();

    std::cout << std::endl;
    stats.PrintSummary(flavor_name.Data());

    if (evt_display) delete evt_display;
}