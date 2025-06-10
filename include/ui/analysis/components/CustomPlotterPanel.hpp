#pragma once

#include "storage/dataset/FilteredDatasetView.hpp"
#include "utils/Lazy.hpp"

namespace ui::analysis::components {

class CustomPlotterPanel {
  public:
    explicit CustomPlotterPanel(size_t simulationId);

    void render();

  private:
    size_t m_simulationId;

    utils::Lazy<storage::dataset::FilteredDatasetView> m_filteredDatasetView;

    char m_filter[256]{};
    std::string m_filterExpr;
    std::string m_errorMessageExpr;

    char xExpr[512] = "";
    char yExpr[512] = "";
    std::string xExprStr;
    std::string yExprStr;
    std::string errorMessagePlot;

    std::optional<std::vector<double>> m_xData;
    std::optional<std::vector<double>> m_yData;
};
} // namespace ui::analysis::components