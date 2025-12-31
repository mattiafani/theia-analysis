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

extern "C" void validate(int dataset = 1, bool debug = true, bool event_display = true, int start_file = 4) {
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
        n_files = 10;
        n_loop_start = 0;
        loop_end = n_files;
    }

    TString chunk = Config::GetChunkName(dataset);
    if (chunk == "") {
        std::cerr << "Error: Invalid dataset number!" << std::endl;
        return;
    }

    Statistics stats;
    FileManager file_mgr;
    HistogramManager hist_mgr;
    EventDisplay* evt_display = nullptr;

    file_mgr.SetupPaths(chunk.Data());

    hist_mgr.Initialize();
    TString output_name = TString::Format("validate_%s_%s.root", chunk.Data(), config.GetTimestamp().Data());
    hist_mgr.InitializeOutputFile(output_name.Data());

    if (config.GetEventDisplayMode()) {
        evt_display = new EventDisplay();
    }

    EventProcessor processor(file_mgr, hist_mgr, stats);
    if (evt_display) {
        processor.SetEventDisplay(evt_display);
    }

    for (int i = n_loop_start; i < loop_end; i++) {
        if (!debug && i % 1000 == 0) {
            std::cout << "\n: Reading " << chunk << " I/O files nr " << i << " out of 10000..." << std::endl;
        }

        processor.ProcessFile(i, dataset);
    }

    hist_mgr.Write(dataset);
    hist_mgr.CloseOutputFile();

    std::cout << std::endl;
    stats.PrintSummary(chunk.Data());

    if (evt_display) delete evt_display;
}
