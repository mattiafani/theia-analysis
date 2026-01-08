#include "../include/Config.h"

#include <TDatime.h>

Config::Config()
    : E_tolerance(0.001),
      distance_cut(50.),
      cut_nuFinal(true),
      cut_toWall(true),
      cut_Ematch(true),
      mode_debug(false),
      mode_event_display(false),
      histo_half_range(1000.0),
      n_bins_h1d_Ediff(1000),
      diff_tolerance(1e-3) {}

TString Config::GetTimestamp() const {
    TDatime now;
    return TString::Format("%d%02d%02d_%02d%02d%02d",
                           now.GetYear(), now.GetMonth(), now.GetDay(),
                           now.GetHour(), now.GetMinute(), now.GetSecond());
}

TString Config::GetFlavorName(int dataset) {
    switch (dataset) {
        case 1:
            return "nue";
        case 2:
            return "numu";
        case 3:
            return "anue";
        case 4:
            return "anumu";
        default:
            return "";
    }
}