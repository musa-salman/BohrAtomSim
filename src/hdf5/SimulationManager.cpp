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

void SimulationManager::addSimulation(const Simulation &simulation) {
    const std::vector<std::string> *fields = nullptr;
    switch (simulation.type) {
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

    auto _simulation = std::make_shared<Simulation>(simulation);
    _simulation->data = std::make_shared<
        std::unordered_map<std::string, std::vector<scalar>>>();

    for (const auto &field : *fields) {
        _simulation->data->emplace(field, std::vector<scalar>());
    }
    simulations.try_emplace(simulation.name, _simulation);
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