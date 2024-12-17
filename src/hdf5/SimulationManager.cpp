
#include <stdexcept>
#include <unordered_map>

#include "hdf5/SimulationBuffer.hpp"
#include "hdf5/SimulationFields.hpp"
#include "hdf5/SimulationManager.hpp"

#include "utils/iterator.h"

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

    simulation_buffers[simulation_name] = SimulationBuffer(*fields);
}

void SimulationManager::appendData(
    const std::string &simulation_name,
    std::unordered_map<std::string, scalar> data) {
    if (!simulation_buffers.contains(simulation_name)) {
        throw std::invalid_argument("Simulation not found");
    }

    simulation_buffers[simulation_name].appendData(data);
}

void SimulationManager::flushSimulation(
    const std::string &simulation_name,
    const std::unordered_map<std::string, std::string> &meta_data) {

    if (!simulation_buffers.contains(simulation_name)) {
        throw std::invalid_argument("Simulation not found");
    }

    data_writer.createSimulationGroup(simulation_name, meta_data);
    data_writer.storeSimulationRecords(
        simulation_name, simulation_buffers[simulation_name].getAllData());
    simulation_buffers[simulation_name].clearData();
}