#ifndef SIMULATION_RESULT_MONITOR_HPP
#define SIMULATION_RESULT_MONITOR_HPP

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "simulator_runner/Simulation.hpp"

class SimulationResultMonitor {
  public:
    SimulationResultMonitor(const std::string &filepath,
                            Simulation::SimulationStatus *status);
    ~SimulationResultMonitor();

    void startMonitoring();
    void stopMonitoring();

    std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
    getDatasets() const;

    std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
    getTrajectories() const;

  private:
    std::string filepath;
    std::atomic<bool> running;

    std::jthread monitor_thread;

    Simulation::SimulationStatus *status;

    std::unordered_map<std::string, std::vector<double>> accumulated_data;

    std::atomic<
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        trajectories;

    std::atomic<
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        shared_datasets;

    size_t loaded_rows = 0;
};

#endif // SIMULATION_RESULT_MONITOR_HPP
