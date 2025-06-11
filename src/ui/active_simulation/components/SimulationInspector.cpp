#include <imgui.h>
#include <implot.h>

#include "simulation/model/Simulation.hpp"
#include "simulation/service/SimulationService.hpp"
#include "ui/active_simulation/components/SimulationInspectorPanel.hpp"
#include "ui/components/SimulationOverviewCard.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::active_simulation::components {
using namespace simulation::model;
using namespace simulation::service;
using namespace utils;

SimulationInspectorPanel::SimulationInspectorPanel()
    : m_simulationEditorDialog() {
    m_simulationEditorDialog.setOnSubmit([](Simulation simulation) {
        SimulationService &simulationService =
            ServiceLocator::getInstance().get<SimulationService>();
        simulationService.updateSimulation(simulation);
    });
    m_plotSelection = {{"t", true}, {"trajectories", false}};
}

void SimulationInspectorPanel::setSimulation(
    const std::shared_ptr<Simulation> &sim) {
    m_simulation = sim;
}

void SimulationInspectorPanel::render() {
    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Border);

    const auto sim = m_simulation.lock();
    if (!sim) {
        ImGui::Text("No simulation selected.");
        ImGui::EndChild();
        return;
    }

    ui::components::SimulationOverviewCard::render(*sim);

    SimulationService &simulationService =
        ServiceLocator::getInstance().get<SimulationService>();
    size_t simId = sim->getId();
    const auto &status = sim->status;

    if (status == Simulation::SimulationStatus::COMPLETED) {
        ImGui::EndChild();
        return;
    } else if (status == Simulation::SimulationStatus::RUNNING) {
        if (ImGui::Button("Pause Simulation")) {
            simulationService.pauseSimulation(simId);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Simulation")) {
            simulationService.stopSimulation(simId);
        }
    } else if (status == Simulation::SimulationStatus::PAUSED) {
        if (ImGui::Button("Resume Simulation")) {
            simulationService.resumeSimulation(simId);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Simulation")) {
            simulationService.stopSimulation(simId);
        }
    } else if (status == Simulation::SimulationStatus::READY) {
        if (ImGui::Button("Run Simulation")) {
            simulationService.startSimulation(simId);
        }
    }

    ImGui::SameLine();
    if (status == Simulation::SimulationStatus::RUNNING ||
        status == Simulation::SimulationStatus::QUEUED) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Delete Simulation")) {
        simulationService.removeSimulation(simId);
        m_simulation.reset();
        ImGui::EndChild();
        return;
    }
    if (status == Simulation::SimulationStatus::RUNNING ||
        status == Simulation::SimulationStatus::QUEUED) {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();
    if (ImGui::Button("Edit Simulation")) {
        m_simulationEditorDialog.setSimulation(*sim);
    }
    m_simulationEditorDialog.render();

    auto optMonitor = simulationService.getSimulationMonitor(simId);
    if (!optMonitor.has_value()) {
        ImGui::EndChild();
        return;
    }

    auto monitor = optMonitor.value();
    auto datasets = monitor->getDatasets();

    if (!datasets->contains("t") || datasets->at("t").empty()) {
        ImGui::Text("No data available for plotting.");
        ImGui::EndChild();
        return;
    }

    const auto &t_data = datasets->at("t");
    double t_min = t_data.front();
    double t_max = t_data.back();

    ImGui::Text("Select Datasets to Plot:");
    ImGui::PushID("plot_selection");
    for (const auto &[name, data] : *datasets) {
        if (name == "t")
            continue;
        if (!m_plotSelection.contains(name)) {
            m_plotSelection[name] = false;
        }
        ImGui::SameLine();
        ImGui::Checkbox(name.c_str(), &m_plotSelection[name]);
    }
    ImGui::SameLine();
    ImGui::Checkbox("Show Trajectories", &m_plotSelection["trajectories"]);
    ImGui::PopID();

    for (const auto &[name, data] : *datasets) {
        if (name == "t" || !m_plotSelection[name])
            continue;
        if (ImPlot::BeginPlot(name.c_str())) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                              ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisLimits(ImAxis_X1, t_min, t_max, ImGuiCond_Always);
            ImPlot::PlotLine(name.c_str(), t_data.data(), data.data(),
                             static_cast<int>(data.size()));
            ImPlot::EndPlot();
        }
    }

    if (m_plotSelection["trajectories"]) {
        auto cartesian_data = monitor->getTrajectories();
        if (ImPlot::BeginPlot("Trajectories")) {
            ImPlot::SetupAxes("X", "Y");
            ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);
            ImPlot::PlotLine("Trajectory", cartesian_data->at("x").data(),
                             cartesian_data->at("y").data(),
                             static_cast<int>(cartesian_data->at("x").size()));
            ImPlot::EndPlot();
        }
    }

    ImGui::EndChild();
}

} // namespace ui::active_simulation::components
