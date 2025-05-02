#include "imgui.h"
#include "implot.h"
#include "math_utils.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include <sys/stat.h>

#include "view/SimulationAnalyzer.hpp"

SimulationAnalyzer::SimulationAnalyzer(
    const std::shared_ptr<Simulation> simulation)
    : simulation(simulation) {}

void SimulationAnalyzer::renderSimulationDetails() {
    if (!isInitialized) {

        datasets = ServiceLocator::getInstance()
                       .get<ArchivedSimulationManager>()
                       ->getSimulation(simulation->getId());

        trajectoryData = polar2cartesian(datasets);
        plotSelection.emplace("trajectories", false);
        isInitialized = true;
    }
    ImGui::BeginGroup();
    {
        Simulation::SimulationStatus status = simulation->status;

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Simulation Details");
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);

        ImGui::TextDisabled("General Info");
        ImGui::Text("ID:");
        ImGui::NextColumn();
        ImGui::Text("%zu", simulation->getId());
        ImGui::NextColumn();

        ImGui::Text("Name:");
        ImGui::NextColumn();
        ImGui::Text("%s", simulation->getName().c_str());
        ImGui::NextColumn();

        ImGui::Text("Status:");
        ImGui::NextColumn();
        ImGui::Text(
            "%s", status == Simulation::SimulationStatus::COMPLETED
                      ? "Completed"
                  : status == Simulation::SimulationStatus::RUNNING ? "Running"
                  : status == Simulation::SimulationStatus::PAUSED  ? "Paused"
                                                                   : "Unknown");
        ImGui::NextColumn();

        ImGui::Text("Time Step:");
        ImGui::NextColumn();
        ImGui::Text("%.5e", simulation->getDeltaTime());
        ImGui::NextColumn();

        ImGui::Text("Duration:");
        ImGui::NextColumn();
        ImGui::Text("%.2f", simulation->getTotalDuration());
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::TextDisabled("Initial Conditions");
        ImGui::Text("Record Interval:");
        ImGui::NextColumn();
        ImGui::Text("%hu", simulation->getRecordInterval());
        ImGui::NextColumn();

        ImGui::Text("Position (x, y):");
        ImGui::NextColumn();
        ImGui::Text("(%.2e, %.2e)", simulation->getR0X(), simulation->getR0Y());
        ImGui::NextColumn();

        ImGui::Text("Velocity (x, y):");
        ImGui::NextColumn();
        ImGui::Text("(%.2e, %.2e)", simulation->getV0X(), simulation->getV0Y());
        ImGui::NextColumn();

        ImGui::Text("Angle θ:");
        ImGui::NextColumn();
        ImGui::Text("%.2e", simulation->getThetaRV());
        ImGui::NextColumn();

        ImGui::Text("Radius:");
        ImGui::NextColumn();
        ImGui::Text("%.2e", simulation->getR0());
        ImGui::NextColumn();

        ImGui::Text("Velocity Magnitude:");
        ImGui::NextColumn();
        ImGui::Text("%.2e", simulation->getV0());
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::TextDisabled("Derived Values");
        ImGui::Text("Angular Momentum:");
        ImGui::NextColumn();
        ImGui::Text("%.2e", simulation->getV0() * simulation->getR0() *
                                sin(simulation->getThetaRV()));
        ImGui::NextColumn();

        ImGui::Columns(1);
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderTrajectories() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Trajectories");
        ImGui::Separator();

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

        auto t_data = datasets->at("t");

        if (plotSelection["trajectories"]) {
            if (ImPlot::BeginPlot("Trajectories")) {
                ImPlot::SetupAxes("X", "Y");

                ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

                ImPlot::PlotLine(
                    "Trajectory", trajectoryData->at("x").data(),
                    trajectoryData->at("y").data(),
                    static_cast<int>(trajectoryData->at("x").size()));
                ImPlot::EndPlot();
            }
        }

        for (const auto &[name, data] : *datasets) {
            if (name == "t" || !plotSelection[name])
                continue;

            if (ImPlot::BeginPlot(name.c_str())) {
                ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                                  ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxisLimits(ImAxis_X1, t_data.front(),
                                        t_data.back(), ImGuiCond_Once);
                ImPlot::PlotLine(name.c_str(), t_data.data(), data.data(),
                                 static_cast<int>(data.size()));

                ImPlot::EndPlot();
            }
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderExportOptions() {
    ImGui::BeginChild("ExportOptions", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Export Options");
        ImGui::Separator();

        static char fileName[128] = "simulation_data.hdf5";

        ImGui::Text("Export Data to File:");
        ImGui::InputText("File Name", fileName, sizeof(fileName));
        if (ImGui::Button("Export")) {
            // Export logic here
        }
    }
    ImGui::EndChild();
}

void SimulationAnalyzer::renderGraphOptions() {
    ImGui::BeginChild("GraphOptions", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Graph Options");
        ImGui::Separator();

        ImGui::Text("Select Graph Type:");

        if (ImGui::Button("Apply")) {
            // Apply graph type logic here
        }
    }
    ImGui::EndChild();
}

void SimulationAnalyzer::render() {
    renderSimulationDetails();
    renderTrajectories();
}
