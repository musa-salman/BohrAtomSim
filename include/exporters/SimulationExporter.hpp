#ifndef SIMULATION_EXPORTER_HPP
#define SIMULATION_EXPORTER_HPP

#include <string>

#include "dataset/Dataset.hpp"

class SimulationExporter {
  public:
    virtual void exportData(const std::string &path,
                            const Dataset &datasets) = 0;

    virtual ~SimulationExporter() = default;
};

#endif // SIMULATION_EXPORTER_HPP
