#ifndef HDF5_EXPORTER_HPP
#define HDF5_EXPORTER_HPP

#include "SimulationExporter.hpp"
#include "dataset/Dataset.hpp"

class HDF5Exporter : public SimulationExporter {
  public:
    void exportData(const std::string &path, const Dataset &datasets) override;
};

#endif // HDF5_EXPORTER_HPP
