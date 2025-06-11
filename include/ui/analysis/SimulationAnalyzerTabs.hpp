#pragma once

#include <cstddef>

#include "components/CustomPlotterPanel.hpp"
#include "components/DatasetViewerPanel.hpp"
#include "components/DatasetVisualizerPanel.hpp"
#include "simulation/model/Simulation.hpp"
#include "ui/components/SimulationEditorDialog.hpp"

namespace ui::analysis {

class SimulationAnalyzerTabs {
  public:
    SimulationAnalyzerTabs(
        gsl::not_null<simulation::model::Simulation *> simulation,
        std::function<void(size_t)> &&onDeleteCallback);

    void render();

    const simulation::model::Simulation &getSimulation() const {
        return *m_simulation;
    }

  private:
    gsl::not_null<simulation::model::Simulation *> m_simulation;

    components::DatasetVisualizerPanel m_datasetVisualizerPanel;
    components::DatasetViewerPanel m_datasetViewerPanel;
    components::CustomPlotterPanel m_customPlotterPanel;
    ui::components::SimulationEditorDialog m_simulationEditorDialog;

    std::function<void(size_t)> m_onDeleteCallback;

    void _renderSimulationDetails();
};

} // namespace ui::analysis