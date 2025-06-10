#pragma once

#include <string>

#include "storage/dataset/Dataset.hpp"

namespace storage::exporters {

class SimulationExporter {
  public:
    virtual void exportData(const std::string &path,
                            const dataset::Dataset &datasets) = 0;

    virtual ~SimulationExporter() = default;
};
} // namespace storage::exporters
