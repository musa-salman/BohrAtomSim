#ifndef SIMULATION_RESULT_LOADER_HPP
#define SIMULATION_RESULT_LOADER_HPP

#include "orbital_math.h"
#include <string>
#include <unordered_map>
#include <vector>

class SimulationResultLoader {
  public:
    static void loadSimulation(
        const std::string &filepath,
        const std::vector<std::string> &column_names,
        std::unordered_map<std::string, std::vector<scalar>> &datasets);
};

#endif // SIMULATION_RESULT_LOADER_HPP
