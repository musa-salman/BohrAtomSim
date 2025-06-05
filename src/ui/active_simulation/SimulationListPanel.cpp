#include <imgui.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/active_simulation/SimulationListPanel.hpp"

namespace ui::active_simulation {

SimulationListPanel::SimulationListPanel(AddSimulationDialog &dialog)
    : dialog(dialog) {}

void SimulationListPanel::setOnSelect(OnSelectCallback cb) {
    onSelect = std::move(cb);
}

void SimulationListPanel::render() {
    SimulationService &simulationService =
        ServiceLocator::getInstance().get<SimulationService>();

    ImGui::Text("Simulations");
    ImGui::SameLine();
    dialog.render();
    ImGui::Separator();

    int cardId = 0;
    for (const auto &[id, simulation] :
         simulationService.getOngoingSimulations()) {

        ImGui::PushID(cardId++);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
        ImGui::BeginChild("SimCard",
                          ImVec2(ImGui::GetContentRegionAvail().x, 60), true,
                          ImGuiWindowFlags_NoScrollbar);

        const bool isSelected = (selectedSimulationId != 0 &&
                                 selectedSimulationId == simulation->getId());
        if (ImGui::Selectable(simulation->getName().c_str(), isSelected, 0,
                              ImVec2(0, 20))) {
            if (onSelect) {
                onSelect(simulation->getId());
            }
        }

        std::string dimension = simulation->getIs3D() ? "3D" : "2D";
        std::string modelType = simulation->getIsRelativistic()
                                    ? "Relativistic"
                                    : "Non-relativistic";
        ImGui::TextDisabled("Dim: %s   Model: %s", dimension.c_str(),
                            modelType.c_str());

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::PopID();
    }
}

} // namespace ui::active_simulation
