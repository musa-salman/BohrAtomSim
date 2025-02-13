#ifndef SIMULATION_PLOTTER_HPP
#define SIMULATION_PLOTTER_HPP

#include <memory>
#include <string>
#include <vector>

#include "plotter/IPlotFunction.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"

class SimulationPlotter {
  public:
    explicit SimulationPlotter(
        ArchivedSimulationManager &archivedSimulationManager);

    void addPlot(size_t simulationId, std::shared_ptr<IPlotFunction> plotFunc);

    void createCombinedPlot(const std::string &outputFilePath);

    void clearPlots();

  private:
    ArchivedSimulationManager &archivedSimulationHandler;
    std::unordered_map<size_t, std::vector<std::shared_ptr<IPlotFunction>>>
        plotRequests;
};

#endif // SIMULATION_PLOTTER_HPP
