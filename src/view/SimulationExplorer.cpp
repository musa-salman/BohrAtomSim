#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <sys/stat.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "simulator_runner/Simulation.hpp"
#include "view/AddSimulationDialog.hpp"
#include "view/SimulationExplorer.hpp"

SimulationExplorer::SimulationExplorer()
    : simulationService(
          ServiceLocator::getInstance().get<SimulationService>()) {
    addSimulationWindow.setOnSubmit([](const Simulation &simulation) {
        size_t id = ServiceLocator::getInstance()
                        .get<SimulationService>()
                        .addSimulation(simulation);
        if (id == 0) {
            std::cerr << "Failed to create simulation" << std::endl;
            return;
        }
    });

    this->selectedSimulationId = 0;

    if (!simulationService.getOngoingSimulations().empty()) {
        selectedSimulationId =
            simulationService.getOngoingSimulations().begin()->first;
        selectedSimulationStatus = &simulationService.getOngoingSimulations()
                                        .at(selectedSimulationId)
                                        ->status;
        plotSelection.emplace("trajectories", false);
    }
}

void SimulationExplorer::render() {
    ImGui::BeginChild("Simulations List", ImVec2(200, 0),
                      ImGuiChildFlags_Border);
    {
        ImGui::Text("Simulations");
        addSimulationWindow.render();

        for (const auto &[id, simulation] :
             simulationService.getOngoingSimulations()) {
            if (ImGui::Selectable(simulation->getName().c_str(),
                                  selectedSimulationId != 0 &&
                                      selectedSimulationId ==
                                          simulation->getId(),
                                  ImGuiSelectableFlags_SpanAllColumns)) {
                selectedSimulationId = simulation->getId();
                selectedSimulationStatus = &simulation->status;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Border);
    {
        if (selectedSimulationId == 0) {
            ImGui::Text("No simulation selected.");
            ImGui::EndChild();
            return;
        }

        if (*selectedSimulationStatus ==
            Simulation::SimulationStatus::COMPLETED) {
            selectedSimulationId = 0;
            ImGui::Text("Simulation completed.");
            ImGui::EndChild();
            return;
        } else if (*selectedSimulationStatus ==
                   Simulation::SimulationStatus::RUNNING) {
            if (ImGui::Button("Pause Simulation")) {
                simulationService.pauseSimulation(selectedSimulationId);
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop Simulation")) {
                simulationService.stopSimulation(selectedSimulationId);
            }

            ImGui::Text("Simulation is running...");
        } else if (*selectedSimulationStatus ==
                   Simulation::SimulationStatus::PAUSED) {
            if (ImGui::Button("Resume Simulation")) {
                simulationService.resumeSimulation(selectedSimulationId);
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop Simulation")) {
                simulationService.stopSimulation(selectedSimulationId);
            }
            ImGui::Text("Simulation is paused.");
        } else if (*selectedSimulationStatus ==
                   Simulation::SimulationStatus::IDLE) {
            if (ImGui::Button("Run Simulation")) {
                simulationService.startSimulation(selectedSimulationId);
            }
            ImGui::Text("Simulation is ready to run.");
        }

        auto optMonitor =
            simulationService.getSimulationMonitor(selectedSimulationId);
        std::shared_ptr<SimulationResultMonitor> monitor;
        if (optMonitor.has_value()) {
            monitor = optMonitor.value();
        } else {
            ImGui::Text("No data available for plotting.");
            ImGui::EndChild();
            return;
        }
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
                continue; // Skip "t"

            if (!plotSelection.contains(name)) {
                plotSelection[name] = false;
            }

            ImGui::SameLine();

            ImGui::Checkbox(name.c_str(), &plotSelection[name]);
        }

        ImGui::SameLine();
        ImGui::Checkbox("Show Trajectories", &plotSelection["trajectories"]);

        ImGui::PopID();

        // Loop again to plot only selected datasets
        for (const auto &[name, data] : *datasets) {
            if (name == "t" || !plotSelection[name])
                continue;

            if (ImPlot::BeginPlot(name.c_str())) {
                ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                                  ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxisLimits(ImAxis_X1, t_min, t_max,
                                        ImGuiCond_Always);

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

                ImPlot::PlotLine(
                    "Trajectory", cartesian_data->at("x").data(),
                    cartesian_data->at("y").data(),
                    static_cast<int>(cartesian_data->at("x").size()));
                ImPlot::EndPlot();
            }
        }
    }
    ImGui::EndChild();
}
