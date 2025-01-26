#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <string>

#include "view/Simulation.hpp"
#include "view/SimulationCard.hpp"

SimulationCard::SimulationCard(
    const std::function<void(const Simulation *)> &on_simulation_run)
    : on_simulation_run(on_simulation_run) {}

void SimulationCard::draw() {
    ImGuiWindowFlags dockspace_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    dockspace_flags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
    dockspace_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockSpace", nullptr, dockspace_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();

    if (!simulation) {
        return;
    }
    std::string window_name =
        std::string("Simulation: ") + std::string(simulation->name);
    ImGui::Begin(window_name.c_str());

    // Simulation controls
    if (is_simulation_running) {
        if (ImGui::Button("Stop Simulation")) {
            is_simulation_running = false;
        }
    } else {
        if (ImGui::Button("Run Simulation")) {
            on_simulation_run(simulation);
        }
    }

    ImGui::Separator();

    std::cout << "Simulation data size: " << simulation->data->size()
              << std::endl;
    // Plot data
    for (const auto &[name, values] : *simulation->data) {
        std::cout << "Plotting " << name << std::endl;
        if (ImPlot::BeginPlot(name.c_str())) {
            ImPlot::PlotLine(name.c_str(), values.data(),
                             static_cast<int>(values.size()));
            ImPlot::EndPlot();
        }
    }

    std::cout << "Plotting done" << std::endl;

    ImGui::End();
}

void SimulationCard::setSimulation(Simulation &_simulation) {
    this->simulation = &_simulation;
    is_simulation_running = false;
}