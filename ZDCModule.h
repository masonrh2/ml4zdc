enum struct Side : unsigned int {
  C = 0,
  A = 1
};

unsigned int constexpr N_RECO_MODULES = 4;
enum struct RecoModule : unsigned int {
  EM = 0, HAD1 = 1, HAD2 = 2, HAD3 = 3
};

unsigned int constexpr N_SIM_MODULES = 7;
enum struct SimModule : unsigned int {
  EM = 0, HAD1 = 1, HAD2 = 2, HAD3 = 3, RPD = 4, BRAN = 5
};

inline unsigned int getModuleIndex(Side const side, unsigned int const index, unsigned int const sideSize) {
  return static_cast<unsigned int>(side)*sideSize + index;
};

inline unsigned int getModuleIndex(Side const side, RecoModule const mod) {
  return getModuleIndex(side, static_cast<unsigned int>(mod), N_RECO_MODULES);
};

inline unsigned int getModuleIndex(Side const side, SimModule const mod) {
  return getModuleIndex(side, static_cast<unsigned int>(mod), N_SIM_MODULES);
};
