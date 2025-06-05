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
    if (ImGui::BeginChild("Simulation Statistics", ImVec2(0, 70),
                          ImGuiChildFlags_Border)) {

        int readyCount = 0, runningCount = 0, pausedCount = 0, queuedCount = 0;

        for (const auto &[id, sim] :
             simulationService.getOngoingSimulations()) {
            switch (sim->getStatus()) {
            case Simulation::SimulationStatus::READY:
                ++readyCount;
                break;
            case Simulation::SimulationStatus::RUNNING:
                ++runningCount;
                break;
            case Simulation::SimulationStatus::PAUSED:
                ++pausedCount;
                break;
            case Simulation::SimulationStatus::QUEUED:
                ++queuedCount;
                break;
            case Simulation::SimulationStatus::COMPLETED:
                break;
            }
        }

        ImGui::Text("Simulation Statistics");
        ImGui::Separator();
        ImGui::Text("Ready: %d    Queued: %d    Running: %d    Paused: %d",
                    readyCount, queuedCount, runningCount, pausedCount);
    }
    ImGui::EndChild();

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
