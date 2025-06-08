
#include <gsl/pointers>
#include <imgui.h>
#include <memory>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/analysis/SimulationAnalysisManager.hpp"

namespace ui::analysis {
SimulationAnalysisManager::SimulationAnalysisManager() {
    for (const auto &[id, simulation] : ServiceLocator::getInstance()
                                            .get<SimulationService>()
                                            .getCompletedSimulations()) {
        auto analyzer = std::make_unique<SimulationAnalyzerTabs>(
            gsl::not_null<const Simulation *>(simulation.get()),
            [this](size_t id) {
                if (selectedAnalyzer &&
                    selectedAnalyzer.value()->getSimulation().getId() == id)
                    selectedAnalyzer.reset();

                simulationAnalyzers.erase(id);
                ServiceLocator::getInstance()
                    .get<SimulationService>()
                    .removeSimulation(id);
            });
        simulationAnalyzers[simulation->getId()] = std::move(analyzer);
    }

    if (!simulationAnalyzers.empty())
        selectedAnalyzer = simulationAnalyzers.begin()->second.get();
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
                auto analyzer = std::make_unique<SimulationAnalyzerTabs>(
                    gsl::not_null<const Simulation *>(simulation.get()),
                    [this](size_t id) {
                        if (selectedAnalyzer &&
                            selectedAnalyzer.value()->getSimulation().getId() ==
                                id)
                            selectedAnalyzer.reset();

                        simulationAnalyzers.erase(id);
                    });
                simulationAnalyzers[simulation->getId()] = std::move(analyzer);
            }
        }

        for (auto const &[id, analyzer] : simulationAnalyzers) {
            if (ImGui::Selectable(
                    analyzer->getSimulation().getName().c_str(),
                    selectedAnalyzer &&
                        selectedAnalyzer.value()->getSimulation().getId() ==
                            analyzer.get()->getSimulation().getId())) {
                selectedAnalyzer = analyzer.get();
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("#SimulationAnalyzerFromManger", ImVec2(0, 0), true);
        if (selectedAnalyzer) {
            selectedAnalyzer.value()->render();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}
} // namespace ui::analysis