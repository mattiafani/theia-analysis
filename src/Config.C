#include <TDatime.h>

#include "../include/Config.h"

Config::Config()
    : E_tolerance(0.01), distance_cut(2000.0), cut_nuFinal(false), cut_toWall(false), cut_Ematch(false), mode_debug(false), mode_event_display(false), histo_half_range(10.0), diff_tolerance(0.01) {
    n_bins_h1d_Ediff = histo_half_range * 2 * 100;
}

TString Config::GetTimestamp() const {
    TDatime now;
    return TString::Format("%04d%02d%02d_%02d%02d",
                           now.GetYear(),
                           now.GetMonth(),
                           now.GetDay(),
                           now.GetHour(),
                           now.GetMinute());
}
// TString Config::GetTimestamp() const {
//     TDatime now;
//     return TString::Format("%04d%02d%02d",
//                            now.GetYear(),
//                            now.GetMonth(),
//                            now.GetDay());
// }

TString Config::GetChunkName(int dataset) {
    switch (dataset) {
        case 1:
            return "01_FHC_NBE_NBM";
        case 2:
            return "02_FHC_NBM_NBM";
        case 3:
            return "03_FHC_NUE_NUM";
        case 4:
            return "04_FHC_NUM_NUM";
        case 5:
            return "05_RHC_NBE_NBM";
        case 6:
            return "06_RHC_NBM_NBM";
        case 7:
            return "07_RHC_NUE_NUM";
        case 8:
            return "08_RHC_NUM_NUM";
        default:
            return "";
    }
}
