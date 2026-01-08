#ifndef EVENTPROCESSOR_H
#define EVENTPROCESSOR_H

#include <vector>

#include "EventDisplay.h"
#include "FileManager.h"
#include "HistogramManager.h"
#include "Statistics.h"

class EventProcessor {
   public:
    EventProcessor(FileManager& fm, HistogramManager& hm, Statistics& stats);
    ~EventProcessor();

    void ProcessFile(int file_nr, int dataset);

    void SetEventDisplay(EventDisplay* ed) { event_display = ed; }

   private:
    FileManager& file_manager;
    HistogramManager& hist_manager;
    Statistics& statistics;
    EventDisplay* event_display;

    // Input tree branches (GENIE gRooTracker format)
    struct InputBranches {
        Int_t EvtNum;
        Int_t StdHepN;
        Int_t StdHepPdg[100];      // Max particles
        Int_t StdHepStatus[100];
        Double_t StdHepP4[100][4]; // [particle][px,py,pz,E]
        Double_t StdHepX4[100][4]; // [particle][x,y,z,t]
        
        InputBranches();
        void Reset();
    };

    // Output tree branches (RATPAC output format)
    struct OutputBranches {
        Int_t evid, subev, mcid, mcparticlecount, mcpdg, nhits, mcpecount;
        Double_t mcx, mcy, mcz, mcu, mcv, mcw, mct, mcke;
        Double_t scintPhotons, remPhotons, cherPhotons;

        std::vector<int>* mcpdgs;
        std::vector<double>* mcxs;
        std::vector<double>* mcys;
        std::vector<double>* mczs;
        std::vector<double>* mcus;
        std::vector<double>* mcvs;
        std::vector<double>* mcws;
        std::vector<double>* mcts;
        std::vector<double>* mckes;

        std::vector<int>* hitPMTID;
        std::vector<double>* hitPMTTime;
        std::vector<double>* hitPMTCharge;
        std::vector<double>* hitPMTDigitizedTime;
        std::vector<double>* hitPMTDigitizedCharge;

        Int_t digitNhits;
        std::vector<int>* digitPMTID;
        std::vector<double>* digitTime;
        std::vector<double>* digitCharge;
        std::vector<int>* digitNCrossings;
        std::vector<double>* digitTimeOverThreshold;
        std::vector<double>* digitVoltageOverThreshold;
        std::vector<double>* digitPeak;
        std::vector<double>* digitLocalTriggerTime;
        std::vector<int>* digitReconNPEs;

        std::vector<int>* mcPMTID;
        std::vector<int>* mcPMTNPE;
        std::vector<double>* mcPMTCharge;
        std::vector<int>* mcPEPMTID;
        std::vector<double>* mcPEHitTime;
        std::vector<double>* mcPEFrontEndTime;
        std::vector<int>* mcPEProcess;
        std::vector<double>* mcPEx;
        std::vector<double>* mcPEy;
        std::vector<double>* mcPEz;
        std::vector<double>* mcPECharge;
        std::vector<double>* mcPEWavelength;

        OutputBranches();
        ~OutputBranches();
    };

    InputBranches input_branches;
    OutputBranches output_branches;

    void SetupInputBranches(TTree* tree);
    void SetupOutputBranches(TTree* tree);
    void EnableOnlyRequiredBranches(TTree* tree);

    void ProcessEvent(int evt_nr, int dataset, int entry_index);

    void PrintEventInfo(int evt_nr, int entry_index) const;
    
    // Helper to calculate kinetic energy from 4-momentum
    double CalculateKE(double px, double py, double pz, double E) const;
};

#endif