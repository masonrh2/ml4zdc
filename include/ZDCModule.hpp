#pragma once

#include <array>
#include <stdexcept>
#include <functional>

#include "ROOT/RDataFrame.hxx"

namespace Side {
  unsigned int constexpr C = 0;
  unsigned int constexpr A = 1;
}
std::array<unsigned int, 2> constexpr SIDES = {Side::C, Side::A};
std::array<char, 2> constexpr SIDE_LABELS = {'C', 'A'};

inline void checkSide(unsigned int const side) {
  if (side != 0 && side != 1) {
    throw std::runtime_error("invalid side: " + std::to_string(side));
  }
}

inline char getSideLabel(unsigned int const side) {
  checkSide(side);
  return SIDE_LABELS.at(side);
}

unsigned int constexpr N_RECO_MODULES = 4;
enum struct RecoModule : unsigned int {
  EM = 0, HAD1 = 1, HAD2 = 2, HAD3 = 3
};

unsigned int constexpr N_SIM_MODULES = 7;
unsigned int constexpr N_SIM_MODULES_USED = 6;
enum struct SimModule : unsigned int {
  EM = 0, HAD1 = 1, HAD2 = 2, HAD3 = 3, RPD = 4, BRAN = 5
};

std::string const MODULE_TRUTH_ENERGIES_BRANCH = "zdc_ZdcModuleTruthTotal";

std::array<std::string, N_SIM_MODULES_USED> const MODULE_NAMES = {"EM", "HAD1" ,"HAD2" ,"HAD3" ,"RPD" ,"BRAN"};

// plot in order of location in detector, closest to beam first
std::array<unsigned int, N_SIM_MODULES_USED> constexpr MODULE_PLOT_ORDER = {0, 4, 5, 1, 2, 3};

inline unsigned int getModuleIndex(unsigned int const side, unsigned int const index, unsigned int const sideSize) {
  checkSide(side);
  return side*sideSize + index;
};

inline unsigned int getModuleIndex(unsigned int const side, RecoModule const mod) {
  return getModuleIndex(side, static_cast<unsigned int>(mod), N_RECO_MODULES);
};

inline unsigned int getModuleIndex(unsigned int const side, SimModule const mod) {
  return getModuleIndex(side, static_cast<unsigned int>(mod), N_SIM_MODULES);
};

/**
 * @brief get a function that picks an element at a specified index given a vector.
 */
inline std::function<float(ROOT::VecOps::RVec<float> const& vec)> getVectorUnpackerFunc(unsigned int const index) {
  return [index] (ROOT::VecOps::RVec<float> const& vec) {
    return vec.at(index);
  };
}

/**
 * @brief get a function that picks an element at a specified index given a vector, multiplied by scaleFactor.
 */
inline std::function<float(ROOT::VecOps::RVec<float> const& vec)> getVectorUnpackerScalerFunc(unsigned int const index, float const scaleFactor) {
  return [index, scaleFactor] (ROOT::VecOps::RVec<float> const& vec) {
    return vec.at(index)*scaleFactor;
  };
}
