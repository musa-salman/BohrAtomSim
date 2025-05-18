#ifndef SIMULATION_ANALYSIS_MANAGER_HPP
#define SIMULATION_ANALYSIS_MANAGER_HPP

#include "view/Component.hpp"
#include "view/SimulationAnalyzer.hpp"

class SimulationAnalysisManager : public Component {
    std::unordered_map<size_t, std::shared_ptr<SimulationAnalyzer>>
        simulationAnalyzers;
    std::shared_ptr<SimulationAnalyzer> selectedAnalyzer;

  public:
    SimulationAnalysisManager();

    void render() override;
};

#endif // SIMULATION_ANALYSIS_MANAGER_HPP
