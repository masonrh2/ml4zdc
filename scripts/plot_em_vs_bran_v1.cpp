#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH2.h" // 2-dimensional histograms, for full docs see https://root.cern.ch/doc/master/classTH2.html

#include "../include/ZDCModule.h"
#include "../include/Axis.h"

std::string const SIM_FILE_PATH = "../data/SingleNeutronNew.2024.05.19_NTUP.root";
std::string const OUT_FILE_PATH = "../plots/em_vs_bran_v1.root";

/**
 * @brief plot EM module truth vs BRAN truth
 * this version is simpler and plots with an event loop, using TTreeReader
 * run with, for example: root -b -q -l plot_em_vs_bran_v1.cpp
 */
inline void plot_em_vs_bran_v1() {
  // this disables automatic object ownership, which can cause problems when you allocate new histograms
  // while simultaneously opening and closing TFiles
  // see https://root.cern/manual/object_ownership/
  TH1::AddDirectory(false);
  
  TFile* dataFile = TFile::Open(SIM_FILE_PATH.c_str());

  TTreeReader reader("zdcTree", dataFile);
  TTreeReaderArray<float> zdc_ZdcModuleTruthTotal(reader, "zdc_ZdcModuleTruthTotal");

  std::array<TH2D*, 2> hEMVsBRAN {};
  for (auto const& side : SIDES) {
    hEMVsBRAN.at(side) = new TH2D(Form("side%c_EMVsBRAN", getSideLabel(side)), ";EM Truth Energy [MeV];BRAN Truth Energy [MeV];Count", BINS(axis::EMTruth.withBins(32)), BINS(axis::BRANTruth.withBins(32)));
  }

  while (reader.Next()) {
    for (auto const& side : SIDES) {
      hEMVsBRAN.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::EM)), zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::BRAN)));
    }
  }

  dataFile->Close();

  TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& side : SIDES) {
    hEMVsBRAN.at(side)->Write();
  }
  plotFile->Close();

  for (unsigned int side : SIDES) {
    delete hEMVsBRAN.at(side);
  }
}
