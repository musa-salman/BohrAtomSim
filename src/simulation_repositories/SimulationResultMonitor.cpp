#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "explorer_manager/SimulationFields.hpp"
#include "simulation_repositories/SimulationResultLoader.hpp"
#include "simulation_repositories/SimulationResultMonitor.hpp"

SimulationResultMonitor::SimulationResultMonitor(const std::string &filepath,
                                                 int simulation_type)
    : filepath(filepath), simulation_type(simulation_type), running(false) {

    std::cout << "SimulationResultMonitor created with filepath: " << filepath
              << std::endl;
}

SimulationResultMonitor::~SimulationResultMonitor() { stopMonitoring(); }

const std::unordered_map<std::string, std::vector<scalar>> &
SimulationResultMonitor::getDatasets() const {
    return datasets;
}

void SimulationResultMonitor::startMonitoring() {
    running = true;
    monitor_thread = std::jthread([this]() {
        do {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            SimulationResultLoader::loadSimulation(
                filepath, getColumnNames(simulation_type), datasets);
        } while (running);
    });
}

void SimulationResultMonitor::stopMonitoring() {
    running = false;
    if (monitor_thread.joinable()) {
        monitor_thread.join();
    }
}
