#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

#include "storage/dataset/FilteredDatasetView.hpp"
#include "ui/components/ExpressionHelper.hpp"
#include "utils/Lazy.hpp"

namespace ui::analysis::components {

class DatasetVisualizerPanel {
  public:
    DatasetVisualizerPanel(size_t simulationId);

    void render();

  private:
    size_t m_simulationId;
    std::string m_filterExpr;
    std::string m_errorMessageExpr;

    ui::components::ExpressionHelper m_expressionHelper;
    utils::Lazy<std::unordered_map<std::string, std::vector<double>>>
        m_trajectoryData;
    utils::Lazy<storage::dataset::FilteredDatasetView> m_filteredDatasetView;
    std::unordered_map<std::string, bool> m_plotSelection;
};

} // namespace ui::analysis::components