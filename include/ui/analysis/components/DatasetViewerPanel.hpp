#pragma once

#include <gsl/pointers>

#include "DatasetExportPanel.hpp"
#include "simulation/model/Simulation.hpp"
#include "storage/dataset/FilteredDatasetView.hpp"
#include "ui/components/ExpressionHelper.hpp"
#include "utils/Lazy.hpp"

namespace ui::analysis::components {
class DatasetViewerPanel {
  public:
    DatasetViewerPanel(
        gsl::not_null<const simulation::model::Simulation *> simulation);
    void render();

  private:
    gsl::not_null<const simulation::model::Simulation *> m_simulation;
    utils::Lazy<storage::dataset::FilteredDatasetView> m_filteredDatasetView;
    utils::Lazy<DatasetExportPanel> m_exportPanel;
    ui::components::ExpressionHelper m_expressionHelper;
    std::string m_filterExpr;
    std::string m_errorMessageExpr;
};
} // namespace ui::analysis::components