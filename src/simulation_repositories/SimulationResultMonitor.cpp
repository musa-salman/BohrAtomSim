#include "simulation_repositories/SimulationResultMonitor.hpp"
#include "simulation_repositories/SimulationResultLoader.hpp"

#include <chrono>

SimulationResultMonitor::SimulationResultMonitor(const std::string &filepath)
    : filepath(filepath), running(false), loaded_rows(0) {
    shared_datasets.store(
        std::make_shared<
            std::unordered_map<std::string, std::vector<double>>>());
}

SimulationResultMonitor::~SimulationResultMonitor() { stopMonitoring(); }

void SimulationResultMonitor::startMonitoring() {
    running = true;
    monitor_thread = std::jthread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            size_t rows_added = SimulationResultLoader::loadSimulation(
                filepath, accumulated_data, loaded_rows, 1024);

            if (rows_added > 0) {
                loaded_rows += rows_added;
                auto snapshot = std::make_shared<
                    std::unordered_map<std::string, std::vector<double>>>(
                    accumulated_data);
                shared_datasets.store(snapshot);
            }
        }
    });
}

void SimulationResultMonitor::stopMonitoring() {
    running = false;
    if (monitor_thread.joinable()) {
        monitor_thread.join();
    }
}

std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
SimulationResultMonitor::getDatasets() const {
    return shared_datasets.load();
}
