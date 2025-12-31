#include <iomanip>

#include "../include/Statistics.h"

Statistics::Statistics() {
    Reset();
}

void Statistics::Reset() {
    n_output_zombies = 0;
    n_input_zombies = 0;
    n_input_file_not_readable = 0;
    n_output_file_not_readable = 0;
    n_valid_file_pairs = 0;
    n_entries_mismatch = 0;
    n_total_entries = 0;
    n_events_with_KE_size_mismatch = 0;
    n_io_vtx_mismatch = 0;
    n_evts_w_nu_in_final_state = 0;
    n_toWall = 0;
    n_entries_energy_do_not_match = 0;
    n_gone_wrong = 0;
    n_diff_invalid = 0;
    n_diff_out_range = 0;
    n_diff_out_5xrange = 0;
    n_diff_1_10_MeV = 0;
}

void Statistics::PrintSummary(const char* chunk_name) const {
    std::cout << "   Summary " << chunk_name << ":" << std::endl;
    PrintLine("Number of non-existing or corrupted input (genie) files",
              n_input_file_not_readable);
    PrintLine("Number of non-existing or corrupted output (Ratpac) files",
              n_output_file_not_readable);
    PrintLine("Number of I/O file pairs with number of entry mismatch",
              n_entries_mismatch);
    PrintLine("Number of primary events with true KE entries number mismatch",
              n_events_with_KE_size_mismatch,
              n_total_entries);
    PrintLine("Number of events with I/O vtx position mismatch",
              n_io_vtx_mismatch);
    PrintLine("Number of I/O file pairs analyzed",
              n_valid_file_pairs);
}

void Statistics::PrintLine(const char* label, int value) const {
    std::cout << " - " << label << " = " << value << std::endl;
}

void Statistics::PrintLine(const char* label, int value, int total, bool show_percent) const {
    std::cout << " - " << label << " = " << value;
    if (show_percent && total > 0) {
        double percent = 100.0 * value / total;
        std::cout << " over " << total << " total events ("
                  << std::fixed << std::setprecision(2) << percent << "%)";
    }
    std::cout << std::endl;
}
