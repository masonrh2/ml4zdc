#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1.h" // 2-dimensional histograms, for full docs see https://root.cern.ch/doc/master/classTH2.html
#include "TCanvas.h"
#include "TLegend.h"
#include "TLegendEntry.h"

#include "../include/ZDCModule.h"
#include "../include/Axis.h"

std::string const SIM_FILE_PATH = "./data/SingleNeutronNew_2024-05-19_NTUP.root";
std::string const OUT_FILE_PATH = "./plots/no_rpd_sum_energy_dists.root";

inline void plot_no_rpd_sum_energy_dists() {
  TH1::AddDirectory(false);
  
  TFile* dataFile = TFile::Open(SIM_FILE_PATH.c_str());

  TTreeReader reader("zdcTree", dataFile);
  TTreeReaderArray<float> zdc_ZdcModuleTruthTotal(reader, "zdc_ZdcModuleTruthTotal");

  std::array<TH1D*, 2> EM_HAD123_hists {};
  std::array<TH1D*, 2> BRAN_hists {};
  for (auto const& side : SIDES) {
    EM_HAD123_hists.at(side) = new TH1D(Form("side%c_em_had123_energy", getSideLabel(side)), 
        ";EM + HAD123 Energy [MeV];Count", BINS(axis::BRANTruth.withBins(32)));
    BRAN_hists.at(side) = new TH1D(Form("side%c_bran_energy", getSideLabel(side)), 
        ";BRAN Energy [MeV];Count", BINS(axis::BRANTruth.withBins(32)));
  }

  while (reader.Next()) {
    for (auto const& side : SIDES) {
      EM_HAD123_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::EM)) +
           zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD1)) +
           zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD2)) +
           zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD3))
      );
      BRAN_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::BRAN)));
    }
  }

  dataFile->Close();

//   TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
//   for (auto const& side : SIDES) {
//     EM_hists.at(side)->Write();
//     RPD_hists.at(side)->Write();
//     BRAN_hists.at(side)->Write();
//     HAD1_hists.at(side)->Write();
//     HAD2_hists.at(side)->Write();
//     HAD3_hists.at(side)->Write();
//   }
//   plotFile->Close();

//   for (unsigned int side : SIDES) {
//     delete EM_hists.at(side);
//     delete RPD_hists.at(side);
//     delete BRAN_hists.at(side);
//     delete HAD1_hists.at(side);
//     delete HAD2_hists.at(side);
//     delete HAD3_hists.at(side);
//   }

    std::array<TCanvas*, 2> overlays {};
    TLegend * leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    for (auto const& side : SIDES) {
        overlays.at(side) = new TCanvas(Form("side%c_energies", getSideLabel(side)), 
        Form("Side %c Energy Distributions", getSideLabel(side)), 800, 600);
        
        EM_HAD123_hists.at(side)->Draw();
        BRAN_hists.at(side)->Draw("SAME");

        EM_HAD123_hists.at(side)->SetLineColor(kRed);
        BRAN_hists.at(side)->SetLineColor(kBlack);

        EM_HAD123_hists.at(side)->SetLineWidth(4);
        BRAN_hists.at(side)->SetLineWidth(4);


        TLegend * leg = new TLegend(0.7, 0.7, 0.9, 0.9);
        leg->AddEntry(EM_HAD123_hists.at(side), "EM + HAD123", "l");
        leg->AddEntry(BRAN_hists.at(side), "BRAN", "l");
        leg->Draw();
    }
    TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
    for (auto const& side : SIDES) {
        overlays.at(side)->Write();
    }
    plotFile->Close();

    for (unsigned int side : SIDES) {
        delete EM_HAD123_hists.at(side);
        delete BRAN_hists.at(side);
        delete overlays.at(side);
    }

}