#include <algorithm>
#include <imgui.h>
#include <implot.h>

#include "physics/math/math_utils.hpp"
#include "simulation/service/SimulationService.hpp"
#include "storage/dataset/FilteredDatasetView.hpp"
#include "storage/dataset/expression_utils.hpp"
#include "ui/analysis/components/DatasetVisualizerPanel.hpp"
#include "ui/components/Components.hpp"
#include "utils/Lazy.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::analysis::components {
using namespace simulation::service;
using namespace storage::dataset;
using namespace utils;

DatasetVisualizerPanel::DatasetVisualizerPanel(size_t simulationId)
    : m_simulationId(simulationId),
      m_trajectoryData(
          utils::Lazy<std::unordered_map<std::string, std::vector<double>>>(
              [&]() {
                  const auto &dataset =
                      ServiceLocator::getInstance()
                          .get<SimulationService>()
                          .getSimulationResult(m_simulationId);

                  const bool is2D =
                      dataset.getColumnsNames().end() !=
                      std::find(dataset.getColumnsNames().begin(),
                                dataset.getColumnsNames().end(), "psi");
                  if (is2D)
                      return physics::math::polar2cartesian(dataset.get("r"),
                                                            dataset.get("psi"));
                  return physics::math::spherical2cartesian(
                      dataset.get("r"), dataset.get("theta"),
                      dataset.get("phi"));
              })),
      m_filteredDatasetView(utils::Lazy<FilteredDatasetView>([&]() {
          const auto &dataset = ServiceLocator::getInstance()
                                    .get<SimulationService>()
                                    .getSimulationResult(m_simulationId);
          return FilteredDatasetView(dataset.getRowCount());
      })) {
    m_plotSelection.emplace("trajectories", false);
}

void DatasetVisualizerPanel::render() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Trajectories");
        ImGui::SameLine();
        m_expressionHelper.render();
        ImGui::Separator();

        ui::components::renderAutoGrowInputMultiline("Filter Expression",
                                                     m_filterExpr);
        const auto &dataset = ServiceLocator::getInstance()
                                  .get<SimulationService>()
                                  .getSimulationResult(m_simulationId);
        if (ImGui::Button("Apply")) {
            if (std::string(m_filterExpr).empty()) {
                m_filteredDatasetView->includeAllRows(dataset.getRowCount());
            } else {
                const auto mask =
                    computeMaskFromExpression(m_filterExpr, dataset);
                m_filteredDatasetView->updateMask(mask);
                m_errorMessageExpr.clear();
            }
        }
        if (!m_errorMessageExpr.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s",
                               m_errorMessageExpr.c_str());
        }

        ImGui::Text("Select Datasets to Plot:");
        ImGui::PushID("plot_selection");
        for (const auto &name : dataset.getColumnsNames()) {
            if (name == "t")
                continue;

            if (!m_plotSelection.contains(name)) {
                m_plotSelection[name] = false;
            }

            ImGui::SameLine();
            ImGui::Checkbox(name.c_str(), &m_plotSelection[name]);
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show Trajectories", &m_plotSelection["trajectories"]);
        ImGui::PopID();

        const auto &t_column = dataset.get("t");
        const auto &t_ranges = m_filteredDatasetView->ranges();

        double minT = 0, maxT = 800;
        bool hasData = false;

        for (const auto &range : t_ranges) {
            if (range.size() == 0)
                continue;
            auto range_min = *std::min_element(t_column.begin() + range.start,
                                               t_column.begin() + range.end);
            auto range_max = *std::max_element(t_column.begin() + range.start,
                                               t_column.begin() + range.end);
            if (!hasData) {
                minT = range_min;
                maxT = range_max;
                hasData = true;
            } else {
                minT = std::min(minT, range_min);
                maxT = std::max(maxT, range_max);
            }
        }

        // Plot trajectories
        if (m_plotSelection["trajectories"]) {
            if (ImPlot::BeginPlot("Trajectories")) {
                ImPlot::SetupAxes("X", "Y");
                ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

                ImPlot::PlotLine("Trajectory", m_trajectoryData->at("x").data(),
                                 m_trajectoryData->at("y").data(),
                                 m_trajectoryData->at("x").size());

                ImPlot::EndPlot();
            }
        }

        for (const auto &name : dataset.getColumnsNames()) {
            if (name == "t" || !m_plotSelection[name])
                continue;

            const auto &col = dataset.get(name);

            if (ImPlot::BeginPlot(name.c_str())) {
                ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                                  ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxisLimits(ImAxis_X1, minT, maxT, ImGuiCond_Once);

                for (const auto &range : t_ranges) {
                    if (range.size() == 0)
                        continue;
                    ImPlot::PlotLine(name.c_str(),
                                     t_column.data() + range.start,
                                     col.data() + range.start, range.size());
                }

                ImPlot::EndPlot();
            }
        }
        ImGui::EndGroup();
    }
}
} // namespace ui::analysis::components
