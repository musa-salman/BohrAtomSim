#include "imgui.h"
#include "implot.h"
#include <sys/stat.h>

#include "math_utils.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "view/SimulationAnalyzer.hpp"

SimulationAnalyzer::SimulationAnalyzer(
    const std::shared_ptr<Simulation> simulation)
    : simulation(simulation) {}

void SimulationAnalyzer::render() {
    ImGui::BeginTabBar("SimulationAnalyzerTabBar", ImGuiTabBarFlags_None);
    if (ImGui::BeginTabItem("Details")) {
        renderSimulationDetails();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Visualizer")) {
        renderVisualizer();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Plotter")) {
        renderPlotter();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Datasets Viewer")) {
        renderDatasetsViewer();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}

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

        ImGui::BeginGroup();
        ImGui::TextDisabled("General Info");
        if (ImGui::BeginTable("GeneralInfoTable", 2,
                              ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("ID");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%zu", simulation->getId());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", simulation->getName().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Status");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", status == Simulation::SimulationStatus::COMPLETED
                                  ? "Completed"
                              : status == Simulation::SimulationStatus::RUNNING
                                  ? "Running"
                              : status == Simulation::SimulationStatus::PAUSED
                                  ? "Paused"
                                  : "Unknown");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Time Step");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.5e", simulation->getDeltaTime());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Duration");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f", simulation->getTotalDuration());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Record Interval");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%hu", simulation->getRecordInterval());

            ImGui::EndTable();
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();
        ImGui::TextDisabled("Initial Conditions");
        if (ImGui::BeginTable("InitialConditionsTable", 2,
                              ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position (x, y)");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.2e, %.2e)", simulation->getR0X(),
                        simulation->getR0Y());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Velocity (x, y)");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.2e, %.2e)", simulation->getV0X(),
                        simulation->getV0Y());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Angle θ");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getThetaRV());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Radius");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getR0());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Velocity Mag.");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getV0());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Angular Momentum");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getV0() * simulation->getR0() *
                                    sin(simulation->getThetaRV()));

            ImGui::EndTable();
        }
        ImGui::EndGroup();

        ImGui::Columns(1);

        // Buttons
        ImGui::Separator();
        ImGui::TextDisabled("Actions");
        ImGui::BeginGroup();
        if (ImGui::Button("Delete")) {
            // TODO: Delete logic here
        }
        ImGui::SameLine();
        if (ImGui::Button("Export")) {
            // TODO: Export logic here
        }
        ImGui::EndGroup();
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderVisualizer() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Trajectories");
        ImGui::Separator();

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

        auto t_data = datasets->at("t");

        float minT;
        float maxT;
        static float currentT = 800;
        static uint32_t currentIndex = 0;
        if (t_data.size() == 0) {
            minT = 0;
            maxT = 800;
        } else {
            minT = t_data.front();
            maxT = t_data.back();
        }

        if (ImGui::DragFloat("Time Range", &currentT, 1.0f, minT, maxT, "%.7e",
                             ImGuiSliderFlags_AlwaysClamp)) {
            auto it = std::lower_bound(t_data.begin(), t_data.end(), currentT);
            if (it != t_data.end())
                currentIndex = std::distance(t_data.begin(), it);
            else
                currentIndex = t_data.size() - 1;
        }

        ImGui::Text(
            "At Time %.3e: position = (%.7e, %.7e), radius = %.7e, r_dot = "
            "%7e, r_ddot = %.7e, phi = %.7e, phi_dot = %.7e, gamma = %.7e",
            currentT, trajectoryData->at("x")[currentIndex],
            trajectoryData->at("y")[currentIndex],
            datasets->at("r")[currentIndex],
            datasets->at("r_dot")[currentIndex],
            datasets->at("r_ddot")[currentIndex],
            datasets->at("phi")[currentIndex],
            datasets->at("phi_dot")[currentIndex],
            datasets->at("gamma")[currentIndex]);

        if (plotSelection["trajectories"]) {
            if (ImPlot::BeginPlot("Trajectories")) {
                ImPlot::SetupAxes("X", "Y");

                ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

                ImPlot::PlotLine("Trajectory", trajectoryData->at("x").data(),
                                 trajectoryData->at("y").data(), currentIndex);
                ImPlot::EndPlot();
            }
        }

        for (const auto &[name, data] : *datasets) {
            if (name == "t" || !plotSelection[name])
                continue;

            if (ImPlot::BeginPlot(name.c_str())) {
                ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                                  ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxisLimits(ImAxis_X1, minT, maxT, ImGuiCond_Once);
                ImPlot::PlotLine(name.c_str(), t_data.data(), data.data(),
                                 currentIndex);

                ImPlot::EndPlot();
            }
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderPlotter() {
    ImGui::BeginGroup();
    {

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Plotter");
        ImGui::Separator();

        if (ImPlot::BeginPlot("Plotter")) {
            ImPlot::SetupAxes("X", "Y");

            ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

            ImPlot::EndPlot();
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderDatasetsViewer() {
    ImGui::BeginGroup();
    if (datasets && !datasets->empty()) {
        const size_t rowCount = datasets->begin()->second.size();
        const size_t colCount = datasets->size();

        if (ImGui::BeginTable("HeaderTable", colCount,
                              ImGuiTableFlags_Borders)) {
            for (const auto &[key, _] : *datasets) {
                ImGui::TableSetupColumn(key.c_str());
            }
            ImGui::TableHeadersRow();
            ImGui::EndTable();
        }

        if (ImGui::BeginChild("ScrollableData", ImVec2(0, 400), false)) {
            if (ImGui::BeginTable("DatasetsTable", colCount,
                                  ImGuiTableFlags_Borders |
                                      ImGuiTableFlags_RowBg)) {
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(rowCount));
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart;
                         row < clipper.DisplayEnd; ++row) {
                        ImGui::TableNextRow();

                        int col = 0;
                        for (const auto &[key, vec] : *datasets) {
                            ImGui::TableSetColumnIndex(col++);
                            if (row < static_cast<int>(vec.size()))
                                ImGui::Text("%.10e", vec[row]);
                            else
                                ImGui::Text("NaN");
                        }
                    }
                }

                ImGui::EndTable();
            }
        } else {
            ImGui::Text("No dataset loaded.");
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();
}
