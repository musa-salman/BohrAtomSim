#ifndef SIMULATION_RESULT_MONITOR_HPP
#define SIMULATION_RESULT_MONITOR_HPP

#include "orbital_math.h"
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class SimulationResultMonitor {
  public:
    SimulationResultMonitor(const std::string &filepath);
    ~SimulationResultMonitor();

    void startMonitoring();
    void stopMonitoring();

    std::shared_ptr<const std::unordered_map<std::string, std::vector<scalar>>>
    getDatasets() const;

  private:
    std::string filepath;
    std::atomic<bool> running;

    std::jthread monitor_thread;

    std::unordered_map<std::string, std::vector<scalar>> accumulated_data;

    std::atomic<
        std::shared_ptr<std::unordered_map<std::string, std::vector<scalar>>>>
        shared_datasets;

    size_t loaded_rows = 0;
};

#endif // SIMULATION_RESULT_MONITOR_HPP
