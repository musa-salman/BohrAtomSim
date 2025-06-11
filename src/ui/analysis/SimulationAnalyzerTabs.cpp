#include <gsl/pointers>
#include <imgui.h>

#include "simulation/model/Simulation.hpp"
#include "simulation/service/SimulationService.hpp"
#include "ui/analysis/SimulationAnalyzerTabs.hpp"
#include "ui/analysis/components/CustomPlotterPanel.hpp"
#include "ui/analysis/components/DatasetViewerPanel.hpp"
#include "ui/analysis/components/DatasetVisualizerPanel.hpp"
#include "ui/components/SimulationOverviewCard.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::analysis {
using namespace simulation::model;

SimulationAnalyzerTabs::SimulationAnalyzerTabs(
    gsl::not_null<Simulation *> simulation,
    std::function<void(size_t)> &&onDeleteCallback)
    : m_simulation(simulation), m_datasetVisualizerPanel(simulation->getId()),
      m_datasetViewerPanel(simulation),
      m_customPlotterPanel(simulation->getId()),
      m_onDeleteCallback(std::move(onDeleteCallback)) {
    m_simulationEditorDialog.setOnSubmit([](Simulation simulation) {
        utils::ServiceLocator::getInstance()
            .get<simulation::service::SimulationService>()
            .updateSimulation(simulation);
    });
}

void SimulationAnalyzerTabs::render() {
    if (ImGui::BeginTabBar("##SimulationAnalyzerTabBar",
                           ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("Details")) {
            _renderSimulationDetails();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Custom Plotter")) {
            m_customPlotterPanel.render();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Dataset Visualizer")) {
            m_datasetVisualizerPanel.render();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Dataset Viewer")) {
            m_datasetViewerPanel.render();
            ImGui::EndTabItem();
        }
    }

    ImGui::EndTabBar();
}

void SimulationAnalyzerTabs::_renderSimulationDetails() {
    ImGui::BeginGroup();
    {
        ui::components::SimulationOverviewCard::render(*m_simulation);
        ImGui::Separator();
        ImGui::TextDisabled("Actions");
        if (ImGui::Button("Delete"))
            m_onDeleteCallback(m_simulation->getId());
        ImGui::SameLine();
        if (ImGui::Button("Edit")) {
            m_simulationEditorDialog.setSimulation(*m_simulation);
        }
        m_simulationEditorDialog.render();
    }
    ImGui::EndGroup();
}

} // namespace ui::analysis