#include <cstring>
#include <imgui.h>
#include <implot.h>
#include <string>
#include <vector>

#include "simulation_repositories/SimulationRepository.hpp"
#include "view/SimulationTableUI.hpp"

SimulationTableUI::SimulationTableUI(SimulationRepository &repo)
    : repository(repo) {
    simulations = repository.getSimulations(true);
}

void SimulationTableUI::render() {
    ImGui::Begin("Simulation Table");

    renderSearchBar();
    renderActionButtons();
    renderTable();

    ImGui::End();
}

void SimulationTableUI::renderSearchBar() {
    static char buffer[512] = "";
    std::strcpy(buffer, searchQuery.c_str());
    ImGui::InputText("Search", buffer, sizeof(buffer));
    searchQuery = buffer;
}

void SimulationTableUI::renderActionButtons() {
    if (ImGui::Button("Refresh")) {
        simulations = repository.getSimulations(false);
    }

    ImGui::SameLine();
    if (selectedSimulationId.has_value()) {
        if (ImGui::Button("Delete Selected")) {
            repository.removeSimulation(*selectedSimulationId);
            selectedSimulationId.reset();
            simulations = repository.getSimulations(false);
        }
    }
}

void SimulationTableUI::renderTable() {
    if (ImGui::BeginTable("SimTable", 8,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Record Interval");
        ImGui::TableSetupColumn("Time Interval");
        ImGui::TableSetupColumn("Total Duration");
        ImGui::TableSetupColumn("R0");
        ImGui::TableSetupColumn("V0");
        ImGui::TableSetupColumn("Theta");
        ImGui::TableHeadersRow();

        for (const auto &sim : simulations) {
            if (!searchQuery.empty() &&
                sim->getName().find(searchQuery) == std::string::npos &&
                std::to_string(sim->getId()).find(searchQuery) ==
                    std::string::npos)
                continue;

            ImGui::TableNextRow();

            for (int column = 0; column < 7; column++) {
                ImGui::TableSetColumnIndex(column);

                switch (column) {
                case 0:
                    if (ImGui::Selectable(
                            std::to_string(sim->getId()).c_str(),
                            selectedSimulationId == sim->getId(),
                            ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedSimulationId = sim->getId();
                    }
                    break;
                case 1:
                    ImGui::Text("%s", sim->getName().c_str());
                    break;
                case 2:
                    ImGui::Text("%d", sim->getRecordInterval());
                    break;
                case 3:
                    ImGui::Text("%7e", sim->getDeltaTime());
                    break;
                case 4:
                    ImGui::Text("%.7e", sim->getTotalDuration());
                    break;
                case 5:
                    ImGui::Text("%.7e", sim->getR0());
                    break;
                case 6:
                    ImGui::Text("%.7e", sim->getV0());
                    break;
                case 7:
                    ImGui::Text("%.7e", sim->getThetaRV());
                    break;
                }
            }
        }
        ImGui::EndTable();
    }
}
