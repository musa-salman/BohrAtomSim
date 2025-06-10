#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace storage::persistence {

class SimulationResultMonitor {
  public:
    SimulationResultMonitor(const std::string &filepath);
    ~SimulationResultMonitor();

    void startMonitoring();
    void pauseMonitoring();

    std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
    getDatasets() const;

    std::shared_ptr<const std::unordered_map<std::string, std::vector<double>>>
    getTrajectories() const;

  private:
    std::string filepath;
    std::atomic<bool> running;

    std::jthread monitor_thread;

    std::unordered_map<std::string, std::vector<double>> accumulated_data;

    std::atomic<
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        trajectories;

    std::atomic<
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        shared_datasets;

    size_t loaded_rows = 0;
};
} // namespace storage::persistence
