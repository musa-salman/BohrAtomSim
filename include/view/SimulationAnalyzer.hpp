#ifndef SIMULATION_ANALYZER_HPP
#define SIMULATION_ANALYZER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "dataset/BitVector.hpp"
#include "dataset/FilteredDatasetView.hpp"
#include "simulator_runner/Simulation.hpp"
#include "view/Component.hpp"

class SimulationAnalyzer : public Component {
    const std::shared_ptr<Simulation> simulation;

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
    SimulationAnalyzer(const std::shared_ptr<Simulation> simulation);

    std::string getName() const { return simulation->getName(); }

    void render() override;

  private:
    void renderSimulationDetails();
    void renderVisualizer();
    void renderPlotter();
    void renderDatasetsViewer();
};

#endif // SIMULATION_ANALYZER_HPP