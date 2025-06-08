#include <gsl/pointers>
#include <imgui.h>

#include "simulator_runner/Simulation.hpp"
#include "ui/analysis/SimulationAnalyzerTabs.hpp"
#include "ui/analysis/components/CustomPlotterPanel.hpp"
#include "ui/analysis/components/DatasetViewerPanel.hpp"
#include "ui/analysis/components/DatasetVisualizerPanel.hpp"
#include "ui/components/SimulationOverviewCard.hpp"

namespace ui::analysis {
SimulationAnalyzerTabs::SimulationAnalyzerTabs(
    gsl::not_null<const Simulation *> simulation,
    std::function<void(size_t)> &&onDeleteCallback)
    : m_simulation(simulation), m_datasetVisualizerPanel(simulation->getId()),
      m_datasetViewerPanel(simulation),
      m_customPlotterPanel(simulation->getId()),
      m_onDeleteCallback(std::move(onDeleteCallback)) {}

void SimulationAnalyzerTabs::render() {
    ImGui::BeginTabBar("##SimulationAnalyzerTabBar", ImGuiTabBarFlags_None);

    if (ImGui::BeginTabItem("Details")) {
        renderSimulationDetails();
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

    ImGui::EndTabBar();
}

void SimulationAnalyzerTabs::renderSimulationDetails() {
    ImGui::BeginGroup();
    {
        ui::components::SimulationOverviewCard::render(*m_simulation);
        ImGui::Separator();
        ImGui::TextDisabled("Actions");
        if (ImGui::Button("Delete"))
            m_onDeleteCallback(m_simulation->getId());
    }
    ImGui::EndGroup();
}

} // namespace ui::analysis