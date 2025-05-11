
#include "imgui.h"

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ISimulationRepository.hpp"
#include "view/SimulationAnalysisManager.hpp"
#include "view/SimulationAnalyzer.hpp"

SimulationAnalysisManager::SimulationAnalysisManager() {
    simulationRepository =
        ServiceLocator::getInstance().get<ISimulationRepository>();
    auto allSimulations = simulationRepository->getAll();
    for (auto const &simulation : allSimulations) {
        auto analyzer = std::make_shared<SimulationAnalyzer>(simulation);
        simulations[simulation->getId()] = analyzer;
    }

    if (simulations.empty()) {
        ImGui::Text("No simulations available.");
    } else {
        selectedAnalyzer = simulations.begin()->second;
    }
}

void SimulationAnalysisManager::render() {
    ImGui::BeginChild("Simulation Analysis");
    {
        ImGui::BeginChild("Simulations List", ImVec2(200, 0), true);
        ImGui::Text("Simulations");
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            auto allSimulations = simulationRepository->getAll();
            for (auto const &simulation : allSimulations) {
                auto analyzer =
                    std::make_shared<SimulationAnalyzer>(simulation);
                simulations[simulation->getId()] = analyzer;
            }
        }
        for (auto const &[id, analyzer] : simulations) {
            if (ImGui::Selectable(analyzer->getName().c_str(),
                                  selectedAnalyzer == analyzer)) {
                selectedAnalyzer = analyzer;
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("#SimulationAnalyzerFromManger", ImVec2(0, 0), true);
        if (selectedAnalyzer) {
            selectedAnalyzer->render();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}