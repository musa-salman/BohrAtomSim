#pragma once

#include "components/CustomPlotterPanel.hpp"
#include "components/DatasetViewerPanel.hpp"
#include "components/DatasetVisualizerPanel.hpp"
#include "simulator_runner/Simulation.hpp"
#include <cstddef>

namespace ui::analysis {

class SimulationAnalyzerTabs {
  public:
    SimulationAnalyzerTabs(gsl::not_null<const Simulation *> simulation,
                           std::function<void(size_t)> &&onDeleteCallback);

    void render();

    const Simulation &getSimulation() const { return *m_simulation; }

  private:
    gsl::not_null<const Simulation *> m_simulation;

    components::DatasetVisualizerPanel m_datasetVisualizerPanel;
    components::DatasetViewerPanel m_datasetViewerPanel;
    components::CustomPlotterPanel m_customPlotterPanel;

    std::function<void(size_t)> m_onDeleteCallback;

    void renderSimulationDetails();
};

} // namespace ui::analysis