#ifndef CONFIG_H
#define CONFIG_H

#include <TString.h>

class Config {
   public:
    static Config& Instance() {
        static Config instance;
        return instance;
    }

    double E_tolerance;
    double distance_cut;
    bool cut_nuFinal;
    bool cut_toWall;
    bool cut_Ematch;

    bool mode_debug;
    bool mode_event_display;

    double histo_half_range;
    int n_bins_h1d_Ediff;
    double diff_tolerance;

    static const int NSAMPLES = 9;  // 8 + 1

    void SetDebugMode(bool debug) { mode_debug = debug; }
    void SetEventDisplayMode(bool ed) { mode_event_display = ed; }
    void SetCutNuFinal(bool cut) { cut_nuFinal = cut; }
    void SetCutToWall(bool cut) { cut_toWall = cut; }
    void SetCutEmatch(bool cut) { cut_Ematch = cut; }
    void SetETolerance(double tol) { E_tolerance = tol; }
    void SetDistanceCut(double dist) { distance_cut = dist; }

    bool GetDebugMode() const { return mode_debug; }
    bool GetEventDisplayMode() const { return mode_event_display; }

    TString GetTimestamp() const;

    static TString GetChunkName(int dataset);

   private:
    Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif
