// Usage: root -q 'strip_branches.C+("/path/to/theia_anue_000501_000600.root")'
// Env. variable: THEIA_DATA_PATH
// Example: const char* data_dir = std::getenv("THEIA_DATA_PATH");...

#include <iostream>

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"

void strip_branches(const char* input_file_name) {
    TFile f(input_file_name,"READ");
    TTree* output = (TTree*)f.Get("output");
    TTree* meta = (TTree*)f.Get("meta");
    
    output->SetBranchStatus("triggerTime", 0);
    output->SetBranchStatus("timestamp", 0);
    output->SetBranchStatus("trigger_word", 0);
    output->SetBranchStatus("event_cleaning_word", 0);
    output->SetBranchStatus("timeSinceLastTrigger_us", 0);
    output->SetBranchStatus("hitPMTTime", 0);
    output->SetBranchStatus("digitTime", 0);
    output->SetBranchStatus("digitNCrossings", 0);
    output->SetBranchStatus("digitHitCleaningMask", 0);
    output->SetBranchStatus("digitTimeOverThreshold", 0);
    output->SetBranchStatus("digitVoltageOverThreshold", 0);
    output->SetBranchStatus("digitLocalTriggerTime", 0);
    output->SetBranchStatus("digitReconNPEs", 0);
    output->SetBranchStatus("fit_pmtid_Lognormal", 0);
    output->SetBranchStatus("fit_time_Lognormal", 0);
    output->SetBranchStatus("fit_charge_Lognormal", 0);
    output->SetBranchStatus("fit_FOM_Lognormal_baseline", 0);
    output->SetBranchStatus("fit_FOM_Lognormal_chi2ndf", 0);
    output->SetBranchStatus("fit_pmtid_Gaussian", 0);
    output->SetBranchStatus("fit_time_Gaussian", 0);
    output->SetBranchStatus("fit_charge_Gaussian", 0);
    output->SetBranchStatus("fit_FOM_Gaussian_width", 0);
    output->SetBranchStatus("fit_FOM_Gaussian_chi2ndf", 0);
    output->SetBranchStatus("fit_pmtid_Sinc", 0);
    output->SetBranchStatus("fit_time_Sinc", 0);
    output->SetBranchStatus("fit_charge_Sinc", 0);
    output->SetBranchStatus("fit_FOM_Sinc_peak", 0);
    output->SetBranchStatus("fit_pmtid_LucyDDM", 0);
    output->SetBranchStatus("fit_charge_LucyDDM", 0);
    output->SetBranchStatus("fit_time_LucyDDM", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_time_error", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_charge_error", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_chi2ndf", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_poisson_nll", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_iterations_ran", 0);
    output->SetBranchStatus("fit_FOM_LucyDDM_estimated_npe", 0);
    output->SetBranchStatus("fit_pmtid_rsNNLS", 0);
    output->SetBranchStatus("fit_time_rsNNLS", 0);
    output->SetBranchStatus("fit_charge_rsNNLS", 0);
    output->SetBranchStatus("fit_FOM_rsNNLS_chi2ndf", 0);
    output->SetBranchStatus("fit_FOM_rsNNLS_iterations_ran", 0);
    output->SetBranchStatus("mcPEHitTime", 0);
    output->SetBranchStatus("mcPEFrontEndTime", 0);
    output->SetBranchStatus("mcPEWavelength", 0);

    std::string input_filename_str(input_file_name);
    input_filename_str.erase(input_filename_str.length() - 5);
    TFile fo(Form("%s_reduced.root", input_filename_str.c_str()), "RECREATE");
    TTree* output_new = output->CloneTree(-1);
    TTree* meta_new = meta->CloneTree(-1);
    output_new->Write();
    meta_new->Write();
    fo.Close();
    f.Close();
}
