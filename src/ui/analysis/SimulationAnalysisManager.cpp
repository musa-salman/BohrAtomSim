
#include <boost/container/flat_map.hpp>
#include <format>
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
    if (ImGui::BeginChild("Simulation Analysis")) {
        if (ImGui::BeginChild("Simulations List", ImVec2(200, 0), true)) {
            ImGui::Text("Simulations");

            const auto &kCompletedSimulations = ServiceLocator::getInstance()
                                                    .get<SimulationService>()
                                                    .getCompletedSimulations();
            if (isSimulationCountModified(kCompletedSimulations.size())) {
                _loadCompletedSimulations(kCompletedSimulations);
            }

            for (auto const &[id, kAnalyzer] : simulationAnalyzers) {
                if (ImGui::Selectable(
                        std::format("{}##{}",
                                    kAnalyzer->getSimulation().getName(), id)
                            .c_str(),

                        selectedAnalyzer &&
                            selectedAnalyzer.value()->getSimulation().getId() ==
                                kAnalyzer.get()->getSimulation().getId())) {
                    selectedAnalyzer = kAnalyzer.get();
                }
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

void SimulationAnalysisManager::_loadCompletedSimulations(
    const boost::container::flat_map<size_t, std::shared_ptr<Simulation>,
                                     std::greater<size_t>>
        &kCompletedSimulations) noexcept {
    for (const auto &[id, simulation] : kCompletedSimulations) {
        if (simulationAnalyzers.find(id) == simulationAnalyzers.end()) {
            auto analyzer = std::make_unique<SimulationAnalyzerTabs>(
                gsl::not_null<const Simulation *>(simulation.get()),
                [this](size_t id) {
                    if (selectedAnalyzer &&
                        selectedAnalyzer.value()->getSimulation().getId() == id)
                        selectedAnalyzer.reset();

                    simulationAnalyzers.erase(id);
                });
            simulationAnalyzers[id] = std::move(analyzer);
        }
    }
    if (!simulationAnalyzers.empty()) {
        selectedAnalyzer = simulationAnalyzers.begin()->second.get();
    } else {
        selectedAnalyzer.reset();
    }
}

} // namespace ui::analysis