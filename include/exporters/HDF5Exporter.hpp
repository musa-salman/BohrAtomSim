#ifndef HDF5_EXPORTER_HPP
#define HDF5_EXPORTER_HPP

#include "SimulationExporter.hpp"

class HDF5Exporter : public SimulationExporter {
  public:
    void exportData(const std::string &path,
                    const std::unordered_map<std::string, std::vector<double>>
                        &datasets) override;
};

#endif // HDF5_EXPORTER_HPP
