#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1.h" // 2-dimensional histograms, for full docs see https://root.cern.ch/doc/master/classTH2.html
#include "TCanvas.h"
#include "TLegend.h"
#include "TLegendEntry.h"

#include "../include/ZDCModule.hpp"
#include "../include/Axis.hpp"

std::string const SIM_FILE_PATH = "../data/SingleNeutronNew.2024.05.19_NTUP.root";
std::string const OUT_FILE_PATH = "../plots/energy_dists.root";

namespace axisConfig = axis::singleNeutron;

inline void plot_energy_dists() {
  TH1::AddDirectory(false);
  
  TFile* dataFile = TFile::Open(SIM_FILE_PATH.c_str());

  TTreeReader reader("zdcTree", dataFile);
  TTreeReaderArray<float> zdc_ZdcModuleTruthTotal(reader, "zdc_ZdcModuleTruthTotal");

  std::array<TH1D*, 2> EM_hists {};
  std::array<TH1D*, 2> RPD_hists {};
  std::array<TH1D*, 2> BRAN_hists {};
  std::array<TH1D*, 2> HAD1_hists {};
  std::array<TH1D*, 2> HAD2_hists {};
  std::array<TH1D*, 2> HAD3_hists {};
  for (auto const& side : SIDES) {
    EM_hists.at(side) = new TH1D(Form("side%c_em_energy", getSideLabel(side)), 
        ";EM Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
    RPD_hists.at(side) = new TH1D(Form("side%c_rpd_energy", getSideLabel(side)), 
        ";RPD Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
    BRAN_hists.at(side) = new TH1D(Form("side%c_bran_energy", getSideLabel(side)), 
        ";BRAN Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
    HAD1_hists.at(side) = new TH1D(Form("side%c_had1_energy", getSideLabel(side)), 
        ";HAD1 Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
    HAD2_hists.at(side) = new TH1D(Form("side%c_had2_energy", getSideLabel(side)), 
        ";HAD2 Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
    HAD3_hists.at(side) = new TH1D(Form("side%c_had3_energy", getSideLabel(side)), 
        ";HAD3 Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)));
  }

  while (reader.Next()) {
    for (auto const& side : SIDES) {
      EM_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::EM)));
      RPD_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::RPD)));
      BRAN_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::BRAN)));
      HAD1_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD1)));
      HAD2_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD2)));
      HAD3_hists.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD3)));
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
        
        EM_hists.at(side)->Draw();
        RPD_hists.at(side)->Draw("SAME");
        BRAN_hists.at(side)->Draw("SAME");
        HAD1_hists.at(side)->Draw("SAME");
        HAD2_hists.at(side)->Draw("SAME");
        HAD3_hists.at(side)->Draw("SAME");

        EM_hists.at(side)->SetLineColor(kRed);
        RPD_hists.at(side)->SetLineColor(kOrange);
        BRAN_hists.at(side)->SetLineColor(kBlack);
        HAD1_hists.at(side)->SetLineColor(kGreen);
        HAD2_hists.at(side)->SetLineColor(kBlue);
        HAD3_hists.at(side)->SetLineColor(kViolet);

        EM_hists.at(side)->SetLineWidth(4);
        RPD_hists.at(side)->SetLineWidth(4);
        BRAN_hists.at(side)->SetLineWidth(4);
        HAD1_hists.at(side)->SetLineWidth(4);
        HAD2_hists.at(side)->SetLineWidth(4);
        HAD3_hists.at(side)->SetLineWidth(4);


        TLegend * leg = new TLegend(0.7, 0.7, 0.9, 0.9);
        leg->AddEntry(EM_hists.at(side), "EM", "l");
        leg->AddEntry(RPD_hists.at(side), "RPD", "l");
        leg->AddEntry(BRAN_hists.at(side), "BRAN", "l");
        leg->AddEntry(HAD1_hists.at(side), "HAD1", "l");
        leg->AddEntry(HAD2_hists.at(side), "HAD2", "l");
        leg->AddEntry(HAD3_hists.at(side), "HAD3", "l");
        leg->Draw();
    }
    TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
    for (auto const& side : SIDES) {
        overlays.at(side)->Write();
    }
    plotFile->Close();

    for (unsigned int side : SIDES) {
        delete EM_hists.at(side);
        delete RPD_hists.at(side);
        delete BRAN_hists.at(side);
        delete HAD1_hists.at(side);
        delete HAD2_hists.at(side);
        delete HAD3_hists.at(side);
        delete overlays.at(side);
    }

}