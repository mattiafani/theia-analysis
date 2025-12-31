#ifndef EVENTDISPLAY_H
#define EVENTDISPLAY_H

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>

#include <map>
#include <vector>

#include "RAT/DS/MC.hh"
#include "RAT/DS/MCParticle.hh"
#include "RAT/DS/Root.hh"

class EventDisplay {
   public:
    EventDisplay();
    ~EventDisplay();

    bool LoadGeometry(TFile* output_file);

    void CreateDisplay(
        int evt_nr,
        RAT::DS::MC* mc,
        std::vector<int>* mcpdgs,
        std::vector<double>* mcxs,
        std::vector<double>* mcys,
        std::vector<double>* mczs,
        std::vector<double>* mcus,
        std::vector<double>* mcvs,
        std::vector<double>* mcws,
        std::vector<double>* mckes,
        int mcparticlecount,
        double scintPhotons,
        double cherPhotons,
        double remPhotons,
        std::vector<int>* mcPMTID,
        std::vector<int>* mcPMTNPE);

    bool IsGeometryLoaded() const { return geometry_loaded; }

   private:
    // PMT geometry
    std::map<int, TVector3> pmtPos;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    bool geometry_loaded;

    enum Face { ZP = 0,
                ZM = 1,
                XP = 2,
                XM = 3,
                YP = 4,
                YM = 5 };

    void AssignPMTsToFaces(std::vector<std::vector<int>>& face_pmts);
    void DrawFace(int face, int evt_nr, const std::vector<int>& face_pmts, const std::map<int, int>& npe_map);
};

#endif  // EVENTDISPLAY_H
