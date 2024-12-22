#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "hdf5/SimulationFields.hpp"
#include "hdf5/SimulationManager.hpp"

#include "utils/iterator.h"
#include "view/Simulation.hpp"

SimulationManager::SimulationManager(SimulationDataWriter &writer)
    : data_writer(writer) {}

void SimulationManager::addSimulation(const std::string &simulation_name,
                                      sim_type type) {
    const std::vector<std::string> *fields = nullptr;
    switch (type) {
    case POLAR:
        fields = &SIMULATION_2D_NON_REL;
        break;
    case REL_POLAR:
        fields = &SIMULATION_2D_REL;
        break;
    case SPHERICAL:
        fields = &SIMULATION_3D_NON_REL;
        break;
    case REL_SPHERICAL:
        fields = &SIMULATION_3D_REL;
        break;
    default:
        throw std::invalid_argument("Invalid simulation type");
    }

    auto simulation =
        std::make_shared<Simulation>(simulation_name, type, *fields);
    simulations.try_emplace(simulation_name, simulation);
}

void SimulationManager::appendData(
    const std::string &simulation_name,
    const std::unordered_map<std::string, scalar> &data) {
    if (!simulations.contains(simulation_name)) {
        throw std::invalid_argument("Simulation not found");
    }

    simulations.at(simulation_name)->appendData(data);
}

void SimulationManager::flushSimulation(
    const std::string &simulation_name,
    const std::unordered_map<std::string, std::string> &meta_data) {

    if (!simulations.contains(simulation_name)) {
        throw std::invalid_argument("Simulation not found");
    }

    data_writer.createSimulationGroup(simulation_name, meta_data);
    data_writer.storeSimulationRecords(
        simulation_name, simulations.at(simulation_name)->getData());
    simulations.at(simulation_name);
}

const std::unordered_map<std::string, std::shared_ptr<Simulation>> &
SimulationManager::getSimulations() const {
    return simulations;
}