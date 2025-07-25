#include <chrono>
#include <cmath>
#include <memory>
#include <unordered_map>

#include "physics/math/math_utils.hpp"
#include "storage/persistence/SimulationResultLoader.hpp"
#include "storage/persistence/SimulationResultMonitor.hpp"

namespace storage::persistence {
using namespace physics::math;

SimulationResultMonitor::SimulationResultMonitor(const std::string &filepath)
    : filepath(filepath), running(false), loaded_rows(0) {
    shared_datasets.store(
        std::make_shared<
            std::unordered_map<std::string, std::vector<double>>>());
}

SimulationResultMonitor::~SimulationResultMonitor() { pauseMonitoring(); }

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

                std::unordered_map<std::string, std::vector<double>>
                    snapshot_trajectories;
                if (snapshot->contains("psi"))
                    snapshot_trajectories =
                        polar2cartesian(snapshot->at("r"), snapshot->at("psi"));
                else
                    snapshot_trajectories = spherical2cartesian(
                        snapshot->at("r"), snapshot->at("theta"),
                        snapshot->at("phi"));

                shared_datasets.store(snapshot);
                trajectories.store(
                    std::make_shared<
                        std::unordered_map<std::string, std::vector<double>>>(
                        std::move(snapshot_trajectories)));
            }
        }
    });
}

void SimulationResultMonitor::pauseMonitoring() {
    running = false;
    if (monitor_thread.joinable()) {
        monitor_thread.join();
    }
}

std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
SimulationResultMonitor::getDatasets() const {
    return shared_datasets.load();
}

std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
SimulationResultMonitor::getTrajectories() const {
    return trajectories.load();
}
} // namespace storage::persistence
