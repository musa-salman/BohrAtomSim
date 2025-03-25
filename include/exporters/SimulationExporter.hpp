#ifndef SIMULATION_EXPORTER_HPP
#define SIMULATION_EXPORTER_HPP

#include <string>
#include <unordered_map>
#include <vector>

class SimulationExporter {
  public:
    virtual void
    exportData(const std::string &path,
               const std::unordered_map<std::string, std::vector<double>>
                   &datasets) = 0;

    virtual ~SimulationExporter() = default;
};

#endif // SIMULATION_EXPORTER_HPP
