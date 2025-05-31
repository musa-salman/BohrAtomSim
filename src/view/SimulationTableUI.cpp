#include <cstring>
#include <imgui.h>
#include <implot.h>
#include <string>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "view/SimulationTableUI.hpp"

SimulationTableUI::SimulationTableUI() : searchQuery("") {}

void SimulationTableUI::render() {
    if (ImGui::BeginChild("#simTable")) {
        renderActionButtons();
        ImGui::SameLine();
        renderSearchBar();
        renderTable();
    }

    ImGui::EndChild();
}

void SimulationTableUI::renderSearchBar() {
    static char buffer[512] = "";
    std::strcpy(buffer, searchQuery.c_str());
    ImGui::InputText("Search", buffer, sizeof(buffer));
    searchQuery = buffer;
}

void SimulationTableUI::renderActionButtons() {
    if (selectedSimulationId.has_value()) {
        if (ImGui::Button("Delete Selected")) {
            ServiceLocator::getInstance()
                .get<SimulationService>()
                .removeSimulation(selectedSimulationId.value());
            selectedSimulationId.reset();
        }
    }
}
void SimulationTableUI::renderTable() {
    if (ImGui::BeginTable("SimTable", 4,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Stepper Type");
        ImGui::TableSetupColumn("Parameters");
        ImGui::TableHeadersRow();

        for (const auto &[id, sim] : ServiceLocator::getInstance()
                                         .get<SimulationService>()
                                         .getCompletedSimulations()) {
            if (!searchQuery.empty() &&
                sim->getName().find(searchQuery) == std::string::npos &&
                std::to_string(sim->getId()).find(searchQuery) ==
                    std::string::npos)
                continue;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(std::to_string(sim->getId()).c_str(),
                                  selectedSimulationId == sim->getId(),
                                  ImGuiSelectableFlags_SpanAllColumns)) {
                selectedSimulationId = sim->getId();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", sim->getName().c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("is rel: %b, is quant: %b, is 3d: %b.",
                        sim->getIsRelativistic(), sim->getIsQuantized(),
                        sim->getIs3D());

            ImGui::TableSetColumnIndex(3);
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(sim->serializeConstants().c_str());
            ImGui::PopTextWrapPos();
        }

        ImGui::EndTable();
    }
}
