#include <imgui.h>
#include <implot.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/active_simulation/SimulationInspectorPanel.hpp"
#include "ui/components/SimulationOverviewCard.hpp"

namespace ui::active_simulation {

void SimulationInspectorPanel::setSimulation(
    const std::shared_ptr<Simulation> &sim) {
    simulation = sim;
}

void SimulationInspectorPanel::render() {
    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Border);

    const auto sim = simulation.lock();
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
        ImGui::Text("Simulation completed.");
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
        ImGui::Text("Simulation is running...");
    } else if (status == Simulation::SimulationStatus::PAUSED) {
        if (ImGui::Button("Resume Simulation")) {
            simulationService.resumeSimulation(simId);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Simulation")) {
            simulationService.stopSimulation(simId);
        }
        ImGui::Text("Simulation is paused.");
    } else if (status == Simulation::SimulationStatus::READY) {
        if (ImGui::Button("Run Simulation")) {
            simulationService.startSimulation(simId);
        }
        ImGui::Text("Simulation is ready to run.");
    }

    auto optMonitor = simulationService.getSimulationMonitor(simId);
    if (!optMonitor.has_value()) {
        ImGui::Text("No data available for plotting.");
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
        if (!plotSelection.contains(name)) {
            plotSelection[name] = false;
        }
        ImGui::SameLine();
        ImGui::Checkbox(name.c_str(), &plotSelection[name]);
    }
    ImGui::SameLine();
    ImGui::Checkbox("Show Trajectories", &plotSelection["trajectories"]);
    ImGui::PopID();

    for (const auto &[name, data] : *datasets) {
        if (name == "t" || !plotSelection[name])
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

    if (plotSelection["trajectories"]) {
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

} // namespace ui::active_simulation
