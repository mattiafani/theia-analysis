#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>

class Statistics {
   public:
    int n_output_zombies;
    int n_input_zombies;
    int n_input_file_not_readable;
    int n_output_file_not_readable;
    int n_valid_file_pairs;

    int n_entries_mismatch;
    int n_total_entries;
    int n_events_with_KE_size_mismatch;
    int n_io_vtx_mismatch;
    int n_evts_w_nu_in_final_state;
    int n_toWall;
    int n_entries_energy_do_not_match;
    int n_gone_wrong;
    int n_diff_invalid;
    int n_diff_out_range;
    int n_diff_out_5xrange;
    int n_diff_1_10_MeV;

    Statistics();
    void Reset();
    void PrintSummary(const char* chunk_name) const;

   private:
    void PrintLine(const char* label, int value) const;
    void PrintLine(const char* label, int value, int total, bool show_percent = true) const;
};

#endif
