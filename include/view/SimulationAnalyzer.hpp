#ifndef SIMULATION_ANALYZER_HPP
#define SIMULATION_ANALYZER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "simulator_runner/Simulation.hpp"
#include "view/Component.hpp"

class SimulationAnalyzer : public Component {
    const std::shared_ptr<Simulation> simulation;

    std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        datasets;

    std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        trajectoryData;

    std::unordered_map<std::string, bool> plotSelection;

    bool isInitialized = false;

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