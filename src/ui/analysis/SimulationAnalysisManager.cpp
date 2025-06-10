
#include <boost/container/flat_map.hpp>
#include <gsl/pointers>
#include <imgui.h>
#include <memory>

#include "simulation/service/SimulationService.hpp"
#include "ui/analysis/SimulationAnalysisManager.hpp"
#include "ui/components/SimulationListPanel.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::analysis {
using namespace simulation::service;
using namespace utils;
SimulationAnalysisManager::SimulationAnalysisManager()
    : m_simulationListPanel(
          [this](size_t id) {
              selectedAnalyzer = simulationAnalyzers[id].get();
          },
          []() {
              return ServiceLocator::getInstance()
                  .get<SimulationService>()
                  .getCompletedSimulations();
          },
          _renderSimulationCard)

{
    _loadCompletedSimulations(ServiceLocator::getInstance()
                                  .get<SimulationService>()
                                  .getCompletedSimulations());
}

void SimulationAnalysisManager::render() {
    const auto &kCompletedSimulations = ServiceLocator::getInstance()
                                            .get<SimulationService>()
                                            .getCompletedSimulations();
    if (_isSimulationCountModified(kCompletedSimulations.size())) {
        _loadCompletedSimulations(kCompletedSimulations);
    }

    if (ImGui::BeginChild("Simulations List", ImVec2(300, 0), true)) {
        ImGui::Text("Simulations");
        m_simulationListPanel.render();
    }
    ImGui::EndChild();
    ImGui::SameLine();

    if (ImGui::BeginChild("#SimulationAnalyzerFromManger", ImVec2(0, 0),
                          true)) {
        if (selectedAnalyzer)
            selectedAnalyzer.value()->render();
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

bool SimulationAnalysisManager::_renderSimulationCard(
    const Simulation &simulation, bool isSelected) noexcept {
    ImGui::PushID(simulation.getId());
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::BeginGroup();
    bool isNowSelected = false;
    {
        if (ImGui::Selectable(simulation.getName().c_str(), isSelected, 0,
                              ImVec2(0, 20))) {
            isNowSelected = true;
        }
        std::string dimension = simulation.getIs3D() ? "3D" : "2D";
        std::string modelType = simulation.getIsRelativistic()
                                    ? "Relativistic"
                                    : "Non-relativistic";
        ImGui::TextDisabled("Dim: %s   Model: %s", dimension.c_str(),
                            modelType.c_str());
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndGroup();
    ImGui::Spacing();
    ImGui::PopID();
    return isNowSelected;
}

} // namespace ui::analysis