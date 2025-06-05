#ifndef SIMULATION_ANALYSIS_MANAGER_HPP
#define SIMULATION_ANALYSIS_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "Component.hpp"
#include "SimulationAnalyzer.hpp"

namespace ui {

class SimulationAnalysisManager : public Component {
    std::unordered_map<size_t, std::shared_ptr<SimulationAnalyzer>>
        simulationAnalyzers;
    std::shared_ptr<SimulationAnalyzer> selectedAnalyzer;

  public:
    SimulationAnalysisManager();

    void render() override;
};
} // namespace ui

#endif // SIMULATION_ANALYSIS_MANAGER_HPP
