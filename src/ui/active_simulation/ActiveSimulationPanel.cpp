#include "ui/active_simulation/ActiveSimulationPanel.hpp"

#include "service_locator/ServiceLocator.hpp"
#include <imgui.h>
#include <iostream>

namespace ui::active_simulation {

ActiveSimulationPanel::ActiveSimulationPanel()
    : listPanel(addSimulationWindow),
      simulationService(
          ServiceLocator::getInstance().get<SimulationService>()) {

    addSimulationWindow.setOnSubmit([this](const Simulation &simulation) {
        size_t id = simulationService.addSimulation(simulation);
        if (id == 0) {
            std::cerr << "Failed to create simulation" << std::endl;
            return;
        }
        selectedSimulationId = id;
        inspectorPanel.setSimulation(
            simulationService.getOngoingSimulations().at(id));
    });

    listPanel.setOnSelect([this](size_t id) {
        selectedSimulationId = id;
        inspectorPanel.setSimulation(
            simulationService.getOngoingSimulations().at(id));
    });

    if (!simulationService.getOngoingSimulations().empty()) {
        selectedSimulationId =
            simulationService.getOngoingSimulations().begin()->first;
        inspectorPanel.setSimulation(
            simulationService.getOngoingSimulations().begin()->second);
    }
}

void ActiveSimulationPanel::render() {
    ImGui::BeginChild("Simulations List", ImVec2(270, 0),
                      ImGuiChildFlags_Border);
    listPanel.render();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Border);
    inspectorPanel.render();
    ImGui::EndChild();
}

} // namespace ui::active_simulation
