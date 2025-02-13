#include <imgui.h>
#include <implot.h>

#include "simulator_runner/Simulation.hpp"
#include "view/SimulationCard.hpp"

void SimulationCard::draw() {
    // Check if there's no simulation to display
    if (!simulation) {
        ImGui::Text("No simulation selected");
        return;
    }

    // Simulation controls
    if (is_simulation_running) {
        ImGui::Text("Simulation is running...");
    }

    if (ImGui::Button("Run Simulation")) {
        is_simulation_running = true;
        on_simulation_run(simulation);
    }

    ImGui::Separator();
}

void SimulationCard::setOnSimulate(
    const std::function<void(const Simulation *)> &_on_simulation_run) {
    this->on_simulation_run = _on_simulation_run;
}

void SimulationCard::setSimulation(Simulation &_simulation) {
    this->simulation = &_simulation;
    is_simulation_running = false;
}