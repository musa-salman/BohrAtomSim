#pragma once

#include <gsl/gsl>
#include <gsl/pointers>

#include "dataset/FilteredDatasetView.hpp"
#include "simulator_runner/Simulation.hpp"

namespace ui::analysis::components {

class DatasetExportPanel {
  public:
    explicit DatasetExportPanel(
        gsl::not_null<const dataset::FilteredDatasetView *> filteredView,
        gsl::not_null<const Simulation *> simulation);

    void render();

  private:
    gsl::not_null<const dataset::FilteredDatasetView *> m_filteredView;
    gsl::not_null<const Simulation *> m_simulation;
};

} // namespace ui::analysis::components
