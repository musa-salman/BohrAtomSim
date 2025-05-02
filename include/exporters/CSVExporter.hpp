#ifndef CSVEXPORTER_HPP
#define CSVEXPORTER_HPP

#include "SimulationExporter.hpp"

class CSVExporter : public SimulationExporter {
  public:
    void exportData(const std::string &path,
                    const std::unordered_map<std::string, std::vector<double>>
                        &datasets) override;
};

#endif // CSVEXPORTER_HPP