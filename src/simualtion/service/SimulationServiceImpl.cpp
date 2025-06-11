
#include <filesystem>
#include <gsl/assert>
#include <memory>
#include <optional>

#include "simulation/core/ISimulator.hpp"
#include "simulation/model/Simulation.hpp"
#include "simulation/service/SimulationServiceImpl.hpp"
#include "storage/dataset/Dataset.hpp"
#include "storage/persistence/ArchivedSimulationManager.hpp"
#include "storage/persistence/SimulationMonitorManager.hpp"
#include "storage/persistence/SimulationRepository.hpp"
#include "utils/ServiceLocator.hpp"
#include "utils/utils.hpp"

namespace simulation::service {
using namespace simulation::model;
using namespace simulation::core;
using namespace storage::dataset;
using namespace storage::persistence;
using namespace utils;

SimulationServiceImpl::SimulationServiceImpl()
    : simulationRepository(
          ServiceLocator::getInstance().get<SimulationRepository>()),
      simulator(ServiceLocator::getInstance().get<ISimulator>()),
      simulationMonitorManager(
          ServiceLocator::getInstance().get<SimulationMonitorManager>()),
      archivedSimulationManager(
          ServiceLocator::getInstance().get<ArchivedSimulationManager>()) {
    auto simulations = simulationRepository.getAll();

    for (auto &simulation : simulations) {
        if (simulation->status == Simulation::SimulationStatus::COMPLETED) {
            completedSimulations.emplace(
                simulation->getId(),
                std::shared_ptr<Simulation>(std::move(simulation)));
        } else {
            simulation->status = Simulation::SimulationStatus::READY;
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

void SimulationServiceImpl::updateSimulation(const Simulation &simulation) {
    Expects(ongoingSimulations.contains(simulation.getId()) ||
            completedSimulations.contains(simulation.getId()));

    std::shared_ptr<Simulation> &currentSimulation =
        ongoingSimulations.contains(simulation.getId())
            ? ongoingSimulations[simulation.getId()]
            : completedSimulations[simulation.getId()];

    simulationRepository.update(simulation);

    currentSimulation->setName(simulation.getName());
    currentSimulation->setR0(simulation.getR0());
    currentSimulation->setV0(simulation.getV0());
    currentSimulation->setRelativistic(simulation.isRelativistic());
    currentSimulation->setQuantized(simulation.isQuantized());
    currentSimulation->set3D(simulation.is3D());
    currentSimulation->setDeltaTime(simulation.getDeltaTime());
    currentSimulation->setTotalDuration(simulation.getTotalDuration());
    currentSimulation->setRecordInterval(simulation.getRecordInterval());
    currentSimulation->setRLocalMaxCountLimit(
        simulation.getRLocalMaxCountLimit());
    currentSimulation->setPotential(simulation.getPotential());
    currentSimulation->setConstantValues(simulation.getConstantValues());
    currentSimulation->setStatus(simulation.getStatus());
}

void SimulationServiceImpl::startSimulation(size_t id) {
    if (!ongoingSimulations.contains(id))
        throw std::runtime_error("Simulation not found");

    simulationMonitorManager.startMonitoring(id);

    simulator.simulateOrbit(*ongoingSimulations[id], [this, id]() {
        ongoingSimulations[id]->status =
            Simulation::SimulationStatus::COMPLETED;
        simulationRepository.completeSimulation(id);

        completedSimulations.emplace(id, std::move(ongoingSimulations[id]));
        ongoingSimulations.erase(id);

        simulationMonitorManager.stopMonitoring(id);
    });
}

void SimulationServiceImpl::pauseSimulation(size_t id) {
    Expects(ongoingSimulations.contains(id));

    simulator.pauseSimulation(id);
    simulationMonitorManager.pauseMonitoring(id);
    ongoingSimulations.at(id)->status = Simulation::SimulationStatus::PAUSED;
}

void SimulationServiceImpl::stopSimulation(size_t id) {
    simulator.stopSimulation(id);
}

void SimulationServiceImpl::resumeSimulation(size_t id) {
    Expects(ongoingSimulations.contains(id));

    simulator.resumeSimulation(*ongoingSimulations.at(id));
    simulationMonitorManager.startMonitoring(id);
}

void SimulationServiceImpl::removeSimulation(size_t id) {
    Expects(ongoingSimulations.contains(id) ||
            completedSimulations.contains(id));

    if (ongoingSimulations.contains(id)) {
        stopSimulation(id);
        ongoingSimulations.erase(id);
    }

    completedSimulations.erase(id);

    simulationRepository.remove(id);

    std::filesystem::remove(formatOutputFilename(id));
}

const boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                                 std::greater<size_t>> &
SimulationServiceImpl::getOngoingSimulations() const {
    return ongoingSimulations;
}

const boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                                 std::greater<size_t>> &
SimulationServiceImpl::getCompletedSimulations() const {
    return completedSimulations;
}

const Dataset &SimulationServiceImpl::getSimulationResult(size_t id) const {
    Expects(!ongoingSimulations.contains(id) ||
            completedSimulations.contains(id));

    return archivedSimulationManager.getSimulation(id);
}

std::optional<std::shared_ptr<SimulationResultMonitor>>
SimulationServiceImpl::getSimulationMonitor(size_t id) const {
    if (ongoingSimulations.contains(id))
        return simulationMonitorManager.getMonitor(id);

    return std::nullopt;
}
} // namespace simulation::service
