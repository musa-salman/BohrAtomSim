#ifndef SIMULATION_ANALYZER_HPP
#define SIMULATION_ANALYZER_HPP

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "dataset/BitVector.hpp"
#include "dataset/FilteredDatasetView.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "simulator_runner/Simulation.hpp"
#include "view/Component.hpp"

class SimulationAnalyzer : public Component {
    const Simulation &simulation;
    const std::function<void(size_t)> onDeleteCallback;

    SimulationService &simulationService;

    std::unordered_map<std::string, std::vector<double>> trajectoryData;

    std::unordered_map<std::string, bool> plotSelection;

    bool isInitialized = false;

    std::unique_ptr<BitVector> bitVector;
    FilteredDatasetView filteredDatasetView;

    char filter[512] = "";
    std::string filterExpr;
    std::string errorMessageExpr;

    char xExpr[512] = "";
    char yExpr[512] = "";
    std::string xExprStr;
    std::string yExprStr;
    std::string errorMessagePlot;

    std::unique_ptr<std::vector<double>> xData;
    std::unique_ptr<std::vector<double>> yData;

  public:
    SimulationAnalyzer(const Simulation &simulation,
                       const std::function<void(size_t)> onDeleteCallback);

    std::string getName() const { return simulation.getName(); }
    size_t getId() const { return simulation.getId(); }

    void render() override;

  private:
    void renderSimulationDetails();
    void renderVisualizer();
    void renderPlotter();
    void renderDatasetsViewer();
    void renderExportDatasetOptions();
};

#endif // SIMULATION_ANALYZER_HPP