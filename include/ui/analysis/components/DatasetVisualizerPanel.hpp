#pragma once

#include "dataset/FilteredDatasetView.hpp"
#include "utils/Lazy.hpp"

namespace ui::analysis::components {

class DatasetVisualizerPanel {
  public:
    DatasetVisualizerPanel(size_t simulationId);

    void render();

  private:
    size_t m_simulationId;
    char m_filter[512] = "";
    std::string m_filterExpr;
    std::string m_errorMessageExpr;

    utils::Lazy<std::unordered_map<std::string, std::vector<double>>>
        trajectoryData;
    utils::Lazy<dataset::FilteredDatasetView> m_filteredDatasetView;
    std::unordered_map<std::string, bool> plotSelection;
};

} // namespace ui::analysis::components