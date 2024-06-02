#pragma once

#include <array>
#include <stdexcept>

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
