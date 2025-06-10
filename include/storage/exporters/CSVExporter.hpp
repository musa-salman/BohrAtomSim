#pragma once

#include "SimulationExporter.hpp"
#include "storage/dataset/Dataset.hpp"

namespace storage::exporters {

class CSVExporter : public SimulationExporter {
  public:
    void exportData(const std::string &path,
                    const dataset::Dataset &datasets) override;
};
} // namespace storage::exporters
