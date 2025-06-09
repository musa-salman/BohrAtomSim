#ifndef SIMULATION_ANALYSIS_MANAGER_HPP
#define SIMULATION_ANALYSIS_MANAGER_HPP

#include <boost/container/flat_map.hpp>
#include <memory>
#include <optional>

#include "eom/utils.hpp"
#include "ui/analysis/SimulationAnalyzerTabs.hpp"
#include "ui/components/Component.hpp"
#include "ui/components/SimulationListPanel.hpp"

namespace ui::analysis {

class SimulationAnalysisManager : public ui::components::Component {
  public:
    SimulationAnalysisManager();

    void render() override;

  private:
    void _loadCompletedSimulations(
        const boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                                         std::greater<size_t>>
            &kCompletedSimulations) noexcept;

    SIM_CONST SIM_INLINE inline bool
    _isSimulationCountModified(size_t activeSimulationCount) const noexcept {
        return activeSimulationCount != simulationAnalyzers.size();
    }

    static bool _renderSimulationCard(const Simulation &simulation,
                                      bool isSelected) noexcept;

    boost::container::flat_map<size_t, std::unique_ptr<SimulationAnalyzerTabs>,
                               std::greater<size_t>>
        simulationAnalyzers;
    std::optional<SimulationAnalyzerTabs *> selectedAnalyzer;

    ui::components::SimulationListPanel m_simulationListPanel;
};

} // namespace ui::analysis

#endif // SIMULATION_ANALYSIS_MANAGER_HPP
