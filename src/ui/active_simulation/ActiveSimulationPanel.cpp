#include <imgui.h>
#include <iostream>

#include "simulation/service/SimulationService.hpp"
#include "ui/active_simulation/ActiveSimulationPanel.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::active_simulation {
using namespace simulation::service;
using namespace utils;

ActiveSimulationPanel::ActiveSimulationPanel()
    : listPanel(
          [this](size_t id) {
              selectedSimulationId = id;
              inspectorPanel.setSimulation(ServiceLocator::getInstance()
                                               .get<SimulationService>()
                                               .getOngoingSimulations()
                                               .at(id));
          },
          []() {
              return ServiceLocator::getInstance()
                  .get<SimulationService>()
                  .getOngoingSimulations();
          },
          _renderSimulationCard) {

    addSimulationWindow.setOnSubmit([this](const Simulation &simulation) {
        size_t id = ServiceLocator::getInstance()
                        .get<SimulationService>()
                        .addSimulation(simulation);
        if (id == 0) {
            std::cerr << "Failed to create simulation" << std::endl;
            return;
        }
        selectedSimulationId = id;
        inspectorPanel.setSimulation(ServiceLocator::getInstance()
                                         .get<SimulationService>()
                                         .getOngoingSimulations()
                                         .at(id));
    });

    if (!ServiceLocator::getInstance()
             .get<SimulationService>()
             .getOngoingSimulations()
             .empty()) {
        selectedSimulationId = ServiceLocator::getInstance()
                                   .get<SimulationService>()
                                   .getOngoingSimulations()
                                   .begin()
                                   ->first;
        inspectorPanel.setSimulation(ServiceLocator::getInstance()
                                         .get<SimulationService>()
                                         .getOngoingSimulations()
                                         .begin()
                                         ->second);
    }
}

void ActiveSimulationPanel::render() {
    if (ImGui::BeginChild("Simulation Statistics", ImVec2(0, 70),
                          ImGuiChildFlags_Border)) {

        int readyCount = 0, runningCount = 0, pausedCount = 0, queuedCount = 0;

        for (const auto &[id, sim] : ServiceLocator::getInstance()
                                         .get<SimulationService>()
                                         .getOngoingSimulations()) {
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

    ImGui::BeginChild("Simulations List", ImVec2(300, 0),
                      ImGuiChildFlags_Border);

    ImGui::Text("Simulations");
    ImGui::SameLine();
    addSimulationWindow.render();
    ImGui::Separator();
    listPanel.render();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Simulation Details", ImVec2(0, 0),
                      ImGuiChildFlags_Border);
    inspectorPanel.render();
    ImGui::EndChild();
}

bool ActiveSimulationPanel::_renderSimulationCard(const Simulation &simulation,
                                                  bool isSelected) noexcept {
    ImGui::PushID(simulation.getId());
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::BeginGroup();
    {
        if (ImGui::Selectable(simulation.getName().c_str(), isSelected, 0,
                              ImVec2(0, 20))) {
            return true;
        }
        std::string dimension = simulation.is3D() ? "3D" : "2D";
        std::string modelType =
            simulation.isRelativistic() ? "Relativistic" : "Non-relativistic";
        ImGui::TextDisabled("Dim: %s   Model: %s", dimension.c_str(),
                            modelType.c_str());
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndGroup();
    ImGui::Spacing();
    ImGui::PopID();
    return false;
}
} // namespace ui::active_simulation
