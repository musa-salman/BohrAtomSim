
#include <ImGuiFileDialog.h>
#include <imgui.h>

#include "dataset/Dataset.hpp"
#include "dataset/DatasetFactory.hpp"
#include "exporters/CSVExporter.hpp"
#include "exporters/HDF5Exporter.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "ui/analysis/components/DatasetExportPanel.hpp"
#include "ui/ui_utils.hpp"

namespace ui::analysis::components {

DatasetExportPanel::DatasetExportPanel(
    gsl::not_null<const dataset::FilteredDatasetView *> filteredView,
    gsl::not_null<const Simulation *> simulation)
    : m_filteredView(filteredView), m_simulation(simulation) {}

void DatasetExportPanel::render() {
    if (ImGui::Button("Export")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.fileName = ui::createFormattedSimulationName(*m_simulation);

        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseFileDlgKey", "Choose File", ".csv,.h5", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const std::string filePath =
                ImGuiFileDialog::Instance()->GetCurrentPath() + "/" +
                ImGuiFileDialog::Instance()->GetCurrentFileName();
            const std::string extension =
                ImGuiFileDialog::Instance()->GetCurrentFilter();

            std::unique_ptr<dataset::Dataset> dataset =
                dataset::DatasetFactory::create(
                    ServiceLocator::getInstance()
                        .get<SimulationService>()
                        .getSimulationResult(m_simulation->getId()),
                    *m_filteredView);

            if (extension == ".csv") {
                CSVExporter().exportData(filePath, *dataset);
            } else if (extension == ".h5") {
                HDF5Exporter().exportData(filePath, *dataset);
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid file type");
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

} // namespace ui::analysis::components
