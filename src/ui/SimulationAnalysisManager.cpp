
#include "imgui.h"

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/SimulationAnalysisManager.hpp"
#include "ui/SimulationAnalyzer.hpp"

namespace ui {
SimulationAnalysisManager::SimulationAnalysisManager() {
    for (const auto &[id, simulation] : ServiceLocator::getInstance()
                                            .get<SimulationService>()
                                            .getCompletedSimulations()) {
        auto analyzer = std::make_shared<SimulationAnalyzer>(
            *simulation.get(), [this](size_t id) {
                if (selectedAnalyzer && selectedAnalyzer->getId() == id)
                    selectedAnalyzer.reset();

                simulationAnalyzers.erase(id);
                ServiceLocator::getInstance()
                    .get<SimulationService>()
                    .removeSimulation(id);
            });
        simulationAnalyzers[simulation->getId()] = analyzer;
    }

    if (!simulationAnalyzers.empty())
        selectedAnalyzer = simulationAnalyzers.begin()->second;
}

void SimulationAnalysisManager::render() {
    ImGui::BeginChild("Simulation Analysis");
    {
        ImGui::BeginChild("Simulations List", ImVec2(200, 0), true);
        ImGui::Text("Simulations");

        const auto &completedSimulations = ServiceLocator::getInstance()
                                               .get<SimulationService>()
                                               .getCompletedSimulations();
        if (completedSimulations.size() != simulationAnalyzers.size()) {
            for (auto const &[id, simulation] :
                 ServiceLocator::getInstance()
                     .get<SimulationService>()
                     .getCompletedSimulations()) {
                auto analyzer = std::make_shared<SimulationAnalyzer>(
                    *simulation.get(), [this](size_t id) {
                        if (selectedAnalyzer && selectedAnalyzer->getId() == id)
                            selectedAnalyzer.reset();

                        simulationAnalyzers.erase(id);
                    });
                simulationAnalyzers[simulation->getId()] = analyzer;
            }
        }

        for (auto const &[id, analyzer] : simulationAnalyzers) {
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
} // namespace ui