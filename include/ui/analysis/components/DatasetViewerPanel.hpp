#pragma once

#include "dataset/FilteredDatasetView.hpp"
#include "simulator_runner/Simulation.hpp"
#include "ui/analysis/components/DatasetExportPanel.hpp"
#include "utils/Lazy.hpp"
#include <gsl/pointers>

namespace ui::analysis::components {
class DatasetViewerPanel {
  public:
    DatasetViewerPanel(gsl::not_null<const Simulation *> simulation);
    void render();

  private:
    gsl::not_null<const Simulation *> m_simulation;
    utils::Lazy<dataset::FilteredDatasetView> m_filteredDatasetView;
    utils::Lazy<DatasetExportPanel> m_exportPanel;
    char m_filter[512] = "";
    std::string m_filterExpr;
    std::string m_errorMessageExpr;
};
} // namespace ui::analysis::components