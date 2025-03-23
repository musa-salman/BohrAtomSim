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
    if (ImGui::BeginTable("SimTable", 7,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Record Interval");
        ImGui::TableSetupColumn("Time Interval");
        ImGui::TableSetupColumn("Orbit");
        ImGui::TableSetupColumn("Revolutions");
        ImGui::TableHeadersRow();

        for (const auto &sim : simulations) {
            if (!searchQuery.empty() &&
                sim->name.find(searchQuery) == std::string::npos &&
                std::to_string(sim->id).find(searchQuery) == std::string::npos)
                continue;

            ImGui::TableNextRow();

            for (int column = 0; column < 7; column++) {
                ImGui::TableSetColumnIndex(column);

                switch (column) {
                case 0:
                    if (ImGui::Selectable(
                            std::to_string(sim->id).c_str(),
                            selectedSimulationId == sim->id,
                            ImGuiSelectableFlags_SpanAllColumns)) {
                        selectedSimulationId = sim->id;
                    }
                    break;
                case 1:
                    ImGui::Text("%s", sim->name.c_str());
                    break;
                case 2:
                    ImGui::Text("%s", sim->getType());
                    break;
                case 3:
                    ImGui::Text("%d", sim->record_interval);
                    break;
                case 4:
                    ImGui::Text("%e", sim->time_interval);
                    break;
                case 5:
                    ImGui::Text("n=%d, k=%d, m=%d", sim->orbit.principal,
                                sim->orbit.angular, sim->orbit.magnetic);
                    break;
                case 6:
                    ImGui::Text("%.2f", sim->revolutions);
                    break;
                }
            }
        }
        ImGui::EndTable();
    }
}
