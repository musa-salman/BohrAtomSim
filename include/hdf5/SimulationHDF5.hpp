#ifndef SIMULATIONHDF5_H
#define SIMULATIONHDF5_H

#include <H5Cpp.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/types.h"

const std::string BASE_GROUP = "/simulations";

class SimulationDataWriter {
  private:
    H5::H5File file;
    std::mutex hdf5_mutex;

  public:
    SimulationDataWriter() = delete;

    explicit SimulationDataWriter(const std::string &filename);

    ~SimulationDataWriter();

    void createSimulationGroup(
        const std::string &group_name,
        const std::unordered_map<std::string, std::string> &meta_data);

    void storeSimulationRecords(
        const std::string &group_name,
        const std::unordered_map<std::string, std::vector<scalar>> &records);

    std::unordered_map<std::string, std::string>
    queryMetadata(const std::string &group_name);
};

#endif // SIMULATIONHDF5_H
