#include <IconsFontAwesome4.h>
#include <imgui.h>

#include "view/SimulationExplorer.hpp"

SimulationExplorer::SimulationExplorer(
    const SimulationManager &simulation_manager,
    const std::function<void(Simulation &)> &on_simulation_selected)
    : simulation_manager(simulation_manager),
      on_simulation_selected(on_simulation_selected),
      simulations(simulation_manager.getSimulations()) {
    if (!simulations.empty()) {
        selected_simulation = simulations.begin()->first;
    }
}

void SimulationExplorer::draw() {
    ImGui::Begin("Simulations Explorer");

    ImGui::Separator();

    for (auto &[name, simulation] : simulations) {
        ImGui::PushID(name.c_str());

        // Make simulation name selectable
        if (ImGui::Selectable(name.c_str(), selected_simulation == name)) {
            selected_simulation = name;
            on_simulation_selected(*simulation);
        }

        ImGui::PopID();
    }

    ImGui::End();
}