#ifndef SIMULATION_ANALYSIS_MANAGER_HPP
#define SIMULATION_ANALYSIS_MANAGER_HPP

#include <memory>
#include <optional>
#include <unordered_map>

#include "ui/analysis/SimulationAnalyzerTabs.hpp"
#include "ui/components/Component.hpp"

namespace ui::analysis {

class SimulationAnalysisManager : public ui::components::Component {
    std::unordered_map<size_t, std::unique_ptr<SimulationAnalyzerTabs>>
        simulationAnalyzers;
    std::optional<SimulationAnalyzerTabs *> selectedAnalyzer;

  public:
    SimulationAnalysisManager();

    void render() override;
};
} // namespace ui::analysis

#endif // SIMULATION_ANALYSIS_MANAGER_HPP
