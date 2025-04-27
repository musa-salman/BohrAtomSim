#include <IconsFontAwesome4.h>
#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <sys/stat.h>

#include "view/AddSimulationDialog.hpp"
#include "view/SimulationExplorer.hpp"

SimulationExplorer::SimulationExplorer(
    OngoingSimulationManager &_simulation_manager, Simulator &_simulator)
    : simulation_manager(_simulation_manager), simulator(_simulator),
      simulations(simulation_manager.getSimulations()) {
    add_simulation_interface.setOnSubmit([this](const Simulation &simulation) {
        try {
            simulation_manager.addSimulation(simulation);
        } catch (const std::exception &e) {
            std::cerr << "Error adding simulation: " << e.what() << std::endl;
        }
    });

    simulation_card.setOnSimulate([this](const Simulation *simulation) {
        sim2d_ctx ctx;

        ctx.id = simulation->id;

        ctx.record_interval = simulation->record_interval;
        ctx.time_interval = simulation->time_interval;
        ctx.total_duration = simulation->total_duration;
        ctx.r_0 = simulation->r_0;
        ctx.v_0 = simulation->v_0;
        ctx.theta_rv = simulation->theta_rv;

        simulator.simulateOrbit(ctx, [this, simulation]() {
            simulation_manager.markSimulationAsComplete(simulation->id);
            auto monitor = simulation_manager.getMonitor(simulation->id);
            if (monitor.has_value()) {
                monitor->get()->stopMonitoring();
            }
        });

        auto monitor = simulation_manager.getMonitor(simulation->id);
        if (monitor.has_value()) {
            monitor->get()->startMonitoring();
        }
    });

    this->selected_simulation = std::numeric_limits<size_t>::max();

    if (!simulations.empty()) {
        selected_simulation = simulations.begin()->first;
    }
}

void SimulationExplorer::render() {
    ImGui::Begin("Simulation Runner", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

    ImGuiID dockspace_id = ImGui::GetID("SimExplorerDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

    ImGui::End();

    ImGui::Begin("Simulation Manager", nullptr, ImGuiWindowFlags_NoCollapse);
    {
        ImGui::Text("Simulations");
        add_simulation_interface.render();

        for (auto const &[id, simulation] : simulations) {
            if (ImGui::Selectable(simulation->name.c_str(),
                                  selected_simulation == id)) {
                selected_simulation = id;
                simulation_card.setSimulation(*simulation);
            }
        }
    }
    ImGui::End();

    ImGui::Begin("Simulation Plot", nullptr, ImGuiWindowFlags_NoCollapse);
    {
        if (selected_simulation != std::numeric_limits<size_t>::max()) {
            auto opt_monitor =
                simulation_manager.getMonitor(selected_simulation);
            std::shared_ptr<SimulationResultMonitor> monitor;
            if (opt_monitor.has_value()) {
                monitor = opt_monitor.value();
            }
            auto datasets = monitor->getDatasets();
            simulation_card.render();

            if (!datasets->contains("t") || datasets->at("t").empty()) {
                ImGui::Text("No data available for plotting.");
                ImGui::End();
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

                if (!plot_selection.contains(name)) {
                    plot_selection[name] = false;
                }

                ImGui::SameLine();

                ImGui::Checkbox(name.c_str(), &plot_selection[name]);
            }
            ImGui::PopID();

            // Loop again to plot only selected datasets
            for (const auto &[name, data] : *datasets) {
                if (name == "t" || !plot_selection[name])
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

        } else {
            ImGui::Text("Select a simulation to view live plotting.");
        }
    }
    ImGui::End();
}
