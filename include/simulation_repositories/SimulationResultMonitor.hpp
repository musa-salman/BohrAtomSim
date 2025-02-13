#ifndef SIMULATION_RESULT_MONITOR_HPP
#define SIMULATION_RESULT_MONITOR_HPP

#include <atomic>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class SimulationResultMonitor {
  public:
    SimulationResultMonitor(const std::string &filepath, int simulation_type);
    ~SimulationResultMonitor();

    void startMonitoring();

    void stopMonitoring();

    const std::unordered_map<std::string, std::vector<double>> &
    getDatasets() const;

  private:
    std::string filepath;
    int simulation_type;
    std::unordered_map<std::string, std::vector<double>> datasets;

    std::atomic<bool> running;
    std::jthread monitor_thread;
};

#endif // SIMULATION_RESULT_MONITOR_HPP