
#include <filesystem>
#include <format>
#include <memory>
#include <optional>

#include "explorer_manager/OngoingSimulationManager.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationServiceImpl.hpp"
#include "utils/utils.h"

SimulationServiceImpl::SimulationServiceImpl()
    : simulationRepository(
          ServiceLocator::getInstance().get<SimulationRepository>()),
      simulator(ServiceLocator::getInstance().get<ISimulator>()),
      ongoingSimulationManager(
          ServiceLocator::getInstance().get<OngoingSimulationManager>()),
      archivedSimulationManager(
          ServiceLocator::getInstance().get<ArchivedSimulationManager>()) {
    auto simulations = simulationRepository.getAll();

    for (auto &simulation : simulations) {
        if (simulation->status == Simulation::SimulationStatus::COMPLETED) {
            completedSimulations.emplace(
                simulation->getId(),
                std::shared_ptr<Simulation>(std::move(simulation)));
        } else {
            // TODO: Need cleanup the file result
            ongoingSimulations.emplace(
                simulation->getId(),
                std::shared_ptr<Simulation>(std::move(simulation)));
        }
    }
}

size_t SimulationServiceImpl::addSimulation(const Simulation &simulation) {
    size_t id = simulationRepository.add(simulation);
    if (id == 0)
        throw std::runtime_error("Failed to create simulation");

    auto _simulation = simulation;
    _simulation.setId(id);

    ongoingSimulations.emplace(
        id, std::make_shared<Simulation>(std::move(_simulation)));

    return id;
}

void SimulationServiceImpl::startSimulation(size_t id) {
    if (!ongoingSimulations.contains(id))
        throw std::runtime_error("Simulation not found");

    ongoingSimulationManager.startMonitoring(id);

    simulator.simulateOrbit(*ongoingSimulations[id], [this, id]() {
        ongoingSimulations[id]->status =
            Simulation::SimulationStatus::COMPLETED;
        simulationRepository.markSimulationComplete(id);

        completedSimulations.emplace(id, std::move(ongoingSimulations[id]));
        ongoingSimulations.erase(id);

        ongoingSimulationManager.stopMonitoring(id);
    });
}

void SimulationServiceImpl::pauseSimulation(size_t id) {
    if (!ongoingSimulations.contains(id))
        throw std::runtime_error("Simulation not found");

    simulator.pauseSimulation(id);
    ongoingSimulationManager.pauseMonitoring(id);
    ongoingSimulations.at(id)->status = Simulation::SimulationStatus::PAUSED;
}

void SimulationServiceImpl::stopSimulation(size_t id) {
    simulator.stopSimulation(id);
}

void SimulationServiceImpl::resumeSimulation(size_t id) {
    if (!ongoingSimulations.contains(id))
        throw std::runtime_error("Simulation not found");

    simulator.resumeSimulation(*ongoingSimulations.at(id));
    ongoingSimulationManager.startMonitoring(id);
}

void SimulationServiceImpl::removeSimulation(size_t id) {
    if (ongoingSimulations.contains(id))
        return;

    if (completedSimulations.contains(id)) {

        completedSimulations.erase(id);

        simulationRepository.remove(id);

        std::string filename =
            std::format("{}/simulations/{}.bin", DB_PATH, id);
        std::filesystem::remove(filename);
    }
}

const std::unordered_map<size_t, std::shared_ptr<Simulation>> &
SimulationServiceImpl::getOngoingSimulations() const {
    return ongoingSimulations;
}

const std::unordered_map<size_t, std::shared_ptr<Simulation>> &
SimulationServiceImpl::getCompletedSimulations() const {
    return completedSimulations;
}

const Dataset &SimulationServiceImpl::getSimulationResult(size_t id) const {
    if (ongoingSimulations.contains(id))
        throw std::runtime_error("Simulation is still running");

    if (completedSimulations.contains(id))
        return archivedSimulationManager.getSimulation(id);

    throw std::runtime_error("Simulation not found");
}

std::optional<std::shared_ptr<SimulationResultMonitor>>
SimulationServiceImpl::getSimulationMonitor(size_t id) const {
    if (ongoingSimulations.contains(id))
        return ongoingSimulationManager.getMonitor(id);

    return std::nullopt;
}
