#ifndef SIMULATION_RESULT_LOADER_HPP
#define SIMULATION_RESULT_LOADER_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class SimulationResultLoader {
  public:
    static size_t loadSimulation(
        const std::string &filepath,
        std::unordered_map<std::string, std::vector<double>> &datasets,
        size_t start_row = 0, size_t rows_per_chunk = 1000);
};

#endif // SIMULATION_RESULT_LOADER_HPP
