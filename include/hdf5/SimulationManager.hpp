#ifndef SIMULATION_MANAGER_HPP
#define SIMULATION_MANAGER_HPP

#include <functional>
#include <string>
#include <unordered_map>

#include "SimulationBuffer.hpp"
#include "SimulationHDF5.hpp"
#include "utils/iterator.h"

struct record_handler {
    std::function<void(const sim_itr *)> record;
};

class SimulationManager {
  private:
    SimulationDataWriter &data_writer;
    std::unordered_map<std::string, SimulationBuffer> simulation_buffers;

  public:
    SimulationManager(SimulationDataWriter &writer);

    void addSimulation(const std::string &simulation_name, sim_type type);

    void appendData(const std::string &simulation_name,
                    std::unordered_map<std::string, scalar> data);

    void flushSimulation(
        const std::string &simulation_name,
        const std::unordered_map<std::string, std::string> &meta_data);

    void clearSimulation(const std::string &simulation_name);

    void clearAllSimulations();
};

#endif // SIMULATION_MANAGER_HPP
