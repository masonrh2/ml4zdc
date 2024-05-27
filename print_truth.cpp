#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "ZDCModule.h"

unsigned int constexpr MAX_EVENTS = 10;
std::string const SIM_FILE_PATH = "ZDC_sim_1n_100k.root";

std::string getSideString(TTreeReaderArray<float> const& array, Side const side, unsigned int const precision = 2) {
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

  unsigned int event = 0;
  while (reader.Next() && event < MAX_EVENTS) {
    std::cout << "module truth: " << getSideString(zdc_ZdcModuleTruthTotal, Side::C) << std::endl;
    event++;
  }

  file->Close();
}
