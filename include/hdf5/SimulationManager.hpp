#ifndef SIMULATION_MANAGER_HPP
#define SIMULATION_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "SimulationBuffer.hpp"
#include "SimulationHDF5.hpp"
#include "view/Simulation.hpp"

class SimulationManager {
  private:
    SimulationDataWriter &data_writer;
    std::unordered_map<std::string, std::shared_ptr<Simulation>> simulations;

  public:
    SimulationManager() = delete;
    explicit SimulationManager(SimulationDataWriter &writer);

    void addSimulation(const Simulation &simulation);

    void appendData(const std::string &simulation_name,
                    const std::unordered_map<std::string, scalar> &data);

    void flushSimulation(
        const std::string &simulation_name,
        const std::unordered_map<std::string, std::string> &meta_data);

    const std::unordered_map<std::string, std::shared_ptr<Simulation>> &
    getSimulations() const;

    SimulationBuffer &getSimulationBuffer(const std::string name) const;

    void clearSimulation(const std::string &simulation_name);

    void clearAllSimulations();
};

#endif // SIMULATION_MANAGER_HPP
