
#include <imgui.h>

#include "simulation/service/SimulationService.hpp"
#include "storage/dataset/FilteredDatasetView.hpp"
#include "storage/dataset/expression_utils.hpp"
#include "ui/analysis/components/DatasetExportPanel.hpp"
#include "ui/analysis/components/DatasetViewerPanel.hpp"
#include "ui/components/Components.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::analysis::components {
using namespace simulation::service;
using namespace storage::dataset;
using namespace utils;

DatasetViewerPanel::DatasetViewerPanel(
    gsl::not_null<const Simulation *> simulation)
    : m_simulation(simulation),
      m_filteredDatasetView(utils::Lazy<FilteredDatasetView>([&]() {
          const auto &dataset = ServiceLocator::getInstance()
                                    .get<SimulationService>()
                                    .getSimulationResult(m_simulation->getId());
          return FilteredDatasetView(dataset.getRowCount());
      })),
      m_exportPanel(utils::Lazy<DatasetExportPanel>([&]() {
          return DatasetExportPanel(
              gsl::not_null<const FilteredDatasetView *>(
                  &m_filteredDatasetView.get()),
              gsl::not_null<const Simulation *>(m_simulation.get()));
      })) {}

void DatasetViewerPanel::render() {
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Dataset Viewer");
        ImGui::SameLine();
        m_expressionHelper.render();
        ImGui::Separator();

        ui::components::renderAutoGrowInputMultiline("Filter Expression",
                                                     m_filterExpr);

        const auto &dataset = ServiceLocator::getInstance()
                                  .get<SimulationService>()
                                  .getSimulationResult(m_simulation->getId());
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

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            m_filteredDatasetView->includeAllRows(dataset.getRowCount());
            m_filterExpr.clear();
            m_errorMessageExpr.clear();
        }

        ImGui::SameLine();
        m_exportPanel->render();

        ImGui::Text("Rows: %zu", m_filteredDatasetView->rowCount());

        if (ImGui::BeginTable("HeaderTable", dataset.getColumnsNames().size(),
                              ImGuiTableFlags_Borders)) {
            for (const auto &key : dataset.getColumnsNames())
                ImGui::TableSetupColumn(key.c_str());

            ImGui::TableHeadersRow();
            ImGui::EndTable();
        }

        if (ImGui::BeginChild("ScrollableData", ImVec2(0, 400), false)) {
            size_t colCount = dataset.getColumnsNames().size();
            if (ImGui::BeginTable("DatasetsTable", colCount,
                                  ImGuiTableFlags_Borders |
                                      ImGuiTableFlags_RowBg)) {

                ImGuiListClipper clipper;
                clipper.Begin(
                    static_cast<int>(m_filteredDatasetView->rowCount()));
                while (clipper.Step()) {
                    for (int row = clipper.DisplayStart;
                         row < clipper.DisplayEnd; ++row) {
                        std::optional<size_t> originalRowOpt =
                            m_filteredDatasetView->mapFilteredIndex(row);
                        if (!originalRowOpt.has_value())
                            continue;

                        size_t originalRow = *originalRowOpt;

                        ImGui::TableNextRow();
                        int col = 0;
                        for (const auto &key : dataset.getColumnsNames()) {
                            const auto &colData = dataset.get(key);
                            ImGui::TableSetColumnIndex(col++);
                            if (originalRow < colData.size())
                                ImGui::Text("%.6e", colData[originalRow]);
                            else
                                ImGui::Text("NaN");
                        }
                    }
                }

                ImGui::EndTable();
            }
        } else {
            ImGui::Text("No dataset loaded.");
        }
        ImGui::EndChild();

        ImGui::EndGroup();
    }
}
} // namespace ui::analysis::components