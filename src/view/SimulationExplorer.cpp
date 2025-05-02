#include <IconsFontAwesome4.h>
#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <sys/stat.h>

#include "service_locator/ServiceLocator.hpp"
#include "view/AddSimulationDialog.hpp"
#include "view/SimulationExplorer.hpp"

SimulationExplorer::SimulationExplorer()
    : simulationManager(
          ServiceLocator::getInstance().get<OngoingSimulationManager>()),
      simulator(ServiceLocator::getInstance().get<ISimulator>()),
      simulations(simulationManager->getSimulations()) {
    add_simulation_interface.setOnSubmit([this](const Simulation &simulation) {
        try {
            simulationManager->addSimulation(simulation);
        } catch (const std::exception &e) {
            std::cerr << "Error adding simulation: " << e.what() << std::endl;
        }
    });

    this->selected_simulation = nullptr;

    if (!simulations.empty())
        selected_simulation = simulations.begin()->second.get();

    plotSelection.emplace("trajectories", false);
}

void SimulationExplorer::render() {
    ImGui::BeginChild("Simulations List", ImVec2(200, 0),
                      ImGuiChildFlags_Borders);
    {
        ImGui::Text("Simulations");
        add_simulation_interface.render();

        for (auto const &[id, simulation] : simulations) {
            if (ImGui::Selectable(simulation->getName().c_str(),
                                  selected_simulation != nullptr &&
                                      selected_simulation->getId() == id)) {
                selected_simulation = simulation.get();
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Borders);
    {
        if (selected_simulation == nullptr) {
            ImGui::Text("No simulation selected.");
            ImGui::EndChild();
            return;
        }

        auto opt_monitor =
            simulationManager->getMonitor(selected_simulation->getId());
        std::shared_ptr<SimulationResultMonitor> monitor;
        if (opt_monitor.has_value()) {
            monitor = opt_monitor.value();
        }
        auto datasets = monitor->getDatasets();

        if (selected_simulation->status ==
            Simulation::SimulationStatus::COMPLETED) {
            ImGui::Text("Simulation completed.");
        } else if (selected_simulation->status ==
                   Simulation::SimulationStatus::RUNNING) {
            if (ImGui::Button("Pause Simulation")) {
                simulator->pauseSimulation(selected_simulation->getId());
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop Simulation")) {
                simulator->stopSimulation(selected_simulation->getId());
            }

            ImGui::Text("Simulation is running...");
        } else if (selected_simulation->status ==
                   Simulation::SimulationStatus::PAUSED) {
            if (ImGui::Button("Resume Simulation")) {
                simulator->resumeSimulation(selected_simulation->getId());
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop Simulation")) {
                simulator->stopSimulation(selected_simulation->getId());
            }
            ImGui::Text("Simulation is paused.");
        } else if (selected_simulation->status ==
                   Simulation::SimulationStatus::IDLE) {
            if (ImGui::Button("Run Simulation")) {
                simulator->simulateOrbit(*selected_simulation, [this]() {
                    simulationManager->markSimulationAsComplete(
                        selected_simulation->getId());
                });

                auto monitor =
                    simulationManager->getMonitor(selected_simulation->getId());
                if (monitor.has_value()) {
                    monitor->get()->startMonitoring();
                }
            }
            ImGui::Text("Simulation is ready to run.");
        }

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
