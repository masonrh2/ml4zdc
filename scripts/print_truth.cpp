#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "../include/ZDCModule.h"

unsigned int constexpr MAX_EVENTS = 10;
std::string const SIM_FILE_PATH = "../data/ZDC_sim_1n_100k.root";

inline std::string getString(TTreeReaderArray<float> const& array, unsigned int const precision = 2) {
  std::string tmp = "";
  for (unsigned int i = 0; i < array.GetSize(); i++) {
    tmp += Form("%.*f", precision, array.At(i));
    if (i != array.GetSize() - 1) tmp += ", ";
  }
  return tmp;
}

inline std::string getSideString(TTreeReaderArray<float> const& array, unsigned int const side, unsigned int const precision = 2) {
  std::string tmp = "";
  unsigned int const sideSize = array.GetSize()/2;
  for (unsigned int i = 0; i < sideSize; i++) {
    tmp += Form("%.*f", precision, array.At(getModuleIndex(side, i, sideSize)));
    if (i != sideSize - 1) tmp += ", ";
  }
  return tmp;
}

inline void print_truth() {
  TFile* file = TFile::Open(SIM_FILE_PATH.c_str());

  TTreeReader reader("zdcTree", file);
  TTreeReaderArray<float> zdc_ZdcModuleTruthTotal(reader, "zdc_ZdcModuleTruthTotal");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePosx(reader, "zdc_ZdcTruthParticlePosx");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePosy(reader, "zdc_ZdcTruthParticlePosy");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePosz(reader, "zdc_ZdcTruthParticlePosz");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePx(reader, "zdc_ZdcTruthParticlePx");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePy(reader, "zdc_ZdcTruthParticlePy");
  TTreeReaderArray<float> zdc_ZdcTruthParticlePz(reader, "zdc_ZdcTruthParticlePz");
  TTreeReaderArray<float> zdc_ZdcTruthParticleEnergy(reader, "zdc_ZdcTruthParticleEnergy");

  unsigned int event = 0;
  while (reader.Next() && event < MAX_EVENTS) {
    std::cout << "side C module truth: " << getSideString(zdc_ZdcModuleTruthTotal, Side::C) << std::endl;
    std::cout << "side A module truth: " << getSideString(zdc_ZdcModuleTruthTotal, Side::A) << std::endl;
    std::cout << "\tparticle x: " << getString(zdc_ZdcTruthParticlePosx) << std::endl;
    std::cout << "\tparticle y: " << getString(zdc_ZdcTruthParticlePosy) << std::endl;
    std::cout << "\tparticle z: " << getString(zdc_ZdcTruthParticlePosz) << std::endl;
    std::cout << "\tparticle x momentum: " << getString(zdc_ZdcTruthParticlePx) << std::endl;
    std::cout << "\tparticle y momentum: " << getString(zdc_ZdcTruthParticlePy) << std::endl;
    std::cout << "\tparticle z momentum: " << getString(zdc_ZdcTruthParticlePz) << std::endl;
    std::cout << "\tparticle energy: " << getString(zdc_ZdcTruthParticleEnergy) << std::endl;
    event++;
  }

  file->Close();
}
