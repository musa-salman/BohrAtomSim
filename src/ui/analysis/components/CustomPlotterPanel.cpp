
#include <ImGuiFileDialog.h>
#include <gsl/zstring>
#include <implot.h>
#include <matplot/matplot.h>

#include "imgui.h"
#include "simulation/service/SimulationService.hpp"
#include "storage/dataset/expression_utils.hpp"
#include "ui/analysis/components/CustomPlotterPanel.hpp"
#include "ui/components/Components.hpp"
#include "utils/Lazy.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::analysis::components {
using namespace simulation::service;
using namespace storage::dataset;
using namespace utils;

CustomPlotterPanel::CustomPlotterPanel(size_t simulationId)
    : m_simulationId(simulationId),
      m_filteredDatasetView(utils::Lazy<FilteredDatasetView>([&]() {
          const auto &dataset = ServiceLocator::getInstance()
                                    .get<SimulationService>()
                                    .getSimulationResult(m_simulationId);
          return FilteredDatasetView(dataset.getRowCount());
      })) {}

void CustomPlotterPanel::render() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Plotter");
        ImGui::SameLine();
        m_expressionHelper.render();
        ImGui::Separator();

        ui::components::renderAutoGrowInputMultiline("X Expression", m_xExpr);
        ImGui::Spacing();

        ui::components::renderAutoGrowInputMultiline("Y Expression", m_yExpr);
        ImGui::Spacing();

        ui::components::renderAutoGrowInputMultiline("Filter Expression",
                                                     m_filterExpr);

        if (ImGui::Button("Apply")) {
            const auto &dataset = ServiceLocator::getInstance()
                                      .get<SimulationService>()
                                      .getSimulationResult(m_simulationId);

            // Apply filter
            if (std::string(m_filterExpr).empty()) {
                m_filteredDatasetView->includeAllRows(dataset.getRowCount());
            } else {
                auto mask = computeMaskFromExpression(m_filterExpr, dataset);
                m_filteredDatasetView->updateMask(mask);
            }
            m_errorMessageExpr.clear();

            // Compute X and Y data
            m_xData = computeVectorFromExpression(m_xExpr, dataset);
            m_yData = computeVectorFromExpression(m_yExpr, dataset);
            errorMessagePlot.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            m_xData->clear();
            m_yData->clear();

            m_filterExpr.clear();
            errorMessagePlot.clear();
            m_errorMessageExpr.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Export")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            config.fileName = "plot.png"; // Default file name
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;

            ImGuiFileDialog::Instance()->OpenDialog(
                "Export Plot As", "Export Plot As", ".png,.svg,.pdf,.eps",
                config);
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

            if (m_xData.has_value() && m_yData.has_value()) {
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

    if (ImGuiFileDialog::Instance()->Display("Export Plot As")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const std::string filePath =
                ImGuiFileDialog::Instance()->GetCurrentPath() + "/" +
                ImGuiFileDialog::Instance()->GetCurrentFileName();

            if (!m_xData->empty() && !m_yData->empty()) {
                try {
                    using namespace matplot;

                    std::vector<double> xVec, yVec;
                    const auto &ranges = m_filteredDatasetView->ranges();
                    for (const auto &range : ranges) {
                        for (size_t i = range.start; i < range.end; ++i) {
                            xVec.push_back((*m_xData)[i]);
                            yVec.push_back((*m_yData)[i]);
                        }
                    }

                    auto fig = figure(true);
                    plot(xVec, yVec);
                    xlabel(m_xExpr);
                    ylabel(m_yExpr);

                    save(filePath);
                } catch (const std::exception &e) {
                    errorMessagePlot = e.what();
                }
            } else {
                errorMessagePlot = "No data to export.";
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

} // namespace ui::analysis::components
