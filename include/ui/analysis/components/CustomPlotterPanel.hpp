#pragma once

#include "storage/dataset/FilteredDatasetView.hpp"
#include "ui/components/ExpressionHelper.hpp"
#include "utils/Lazy.hpp"

namespace ui::analysis::components {

class CustomPlotterPanel {
  public:
    explicit CustomPlotterPanel(size_t simulationId);

    void render();

  private:
    size_t m_simulationId;

    ui::components::ExpressionHelper m_expressionHelper;
    utils::Lazy<storage::dataset::FilteredDatasetView> m_filteredDatasetView;

    std::string m_filterExpr;
    std::string m_errorMessageExpr;

    std::string m_xExpr;
    std::string m_yExpr;
    std::string errorMessagePlot;

    std::optional<std::vector<double>> m_xData;
    std::optional<std::vector<double>> m_yData;
};
} // namespace ui::analysis::components