#pragma once

#include "SimulationExporter.hpp"
#include "storage/dataset/Dataset.hpp"

namespace storage::exporters {
class HDF5Exporter : public SimulationExporter {
  public:
    void exportData(const std::string &path,
                    const dataset::Dataset &datasets) override;
};
} // namespace storage::exporters