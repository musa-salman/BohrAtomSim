
#include <implot.h>

#include "dataset/expression_utils.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/analysis/components/CustomPlotterPanel.hpp"

namespace ui::analysis::components {

CustomPlotterPanel::CustomPlotterPanel(size_t simulationId)
    : m_simulationId(simulationId),
      m_filteredDatasetView(utils::Lazy<dataset::FilteredDatasetView>([&]() {
          const auto &dataset = ServiceLocator::getInstance()
                                    .get<SimulationService>()
                                    .getSimulationResult(m_simulationId);
          return dataset::FilteredDatasetView(dataset.getRowCount());
      })) {}

void CustomPlotterPanel::render() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Plotter");
        ImGui::Separator();

        ImGui::InputText("X Expression", xExpr, sizeof(xExpr));
        ImGui::InputText("Y Expression", yExpr, sizeof(yExpr));
        ImGui::InputText("Filter Expression", m_filter, sizeof(m_filter));

        if (ImGui::Button("Apply")) {
            const auto &dataset = ServiceLocator::getInstance()
                                      .get<SimulationService>()
                                      .getSimulationResult(m_simulationId);

            // Apply filter
            if (std::string(m_filter).empty()) {
                m_filteredDatasetView->includeAllRows(dataset.getRowCount());
            } else {
                auto mask =
                    dataset::computeMaskFromExpression(m_filter, dataset);
                m_filteredDatasetView->updateMask(mask);
                m_filterExpr = m_filter;
            }
            m_errorMessageExpr.clear();

            // Compute X and Y data
            m_xData = dataset::computeVectorFromExpression(xExpr, dataset);
            m_yData = dataset::computeVectorFromExpression(yExpr, dataset);
            errorMessagePlot.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            m_xData->clear();
            m_yData->clear();
            m_filter[0] = '\0';
            xExpr[0] = '\0';
            yExpr[0] = '\0';
            m_filterExpr.clear();
            errorMessagePlot.clear();
            m_errorMessageExpr.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Export")) {
            // TODO
        }

        if (!m_errorMessageExpr.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Filter Error: %s",
                               m_errorMessageExpr.c_str());
        }

        if (!errorMessagePlot.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Plot Error: %s",
                               errorMessagePlot.c_str());
        }

        if (ImPlot::BeginPlot("Plotter")) {
            ImPlot::SetupAxes("X", "Y");
            ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

            if (!m_xData->empty() && !m_yData->empty()) {
                const auto &ranges = m_filteredDatasetView->ranges();
                for (const auto &range : ranges) {
                    if (range.end <= m_xData->size() &&
                        range.end <= m_yData->size()) {
                        ImPlot::PlotLine("Filtered Plot",
                                         m_xData->data() + range.start,
                                         m_yData->data() + range.start,
                                         static_cast<int>(range.size()));
                    }
                }
            }

            ImPlot::EndPlot();
        }
    }

    ImGui::EndGroup();
}

} // namespace ui::analysis::components
