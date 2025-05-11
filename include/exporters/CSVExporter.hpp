#ifndef CSVEXPORTER_HPP
#define CSVEXPORTER_HPP

#include "SimulationExporter.hpp"
#include "dataset/Dataset.hpp"

class CSVExporter : public SimulationExporter {
  public:
    void exportData(const std::string &path, const Dataset &datasets) override;
};

#endif // CSVEXPORTER_HPP