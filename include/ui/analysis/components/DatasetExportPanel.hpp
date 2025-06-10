#pragma once

#include <gsl/gsl>
#include <gsl/pointers>

#include "simulation/model/Simulation.hpp"
#include "storage/dataset/FilteredDatasetView.hpp"

namespace ui::analysis::components {

class DatasetExportPanel {
  public:
    explicit DatasetExportPanel(
        gsl::not_null<const storage::dataset::FilteredDatasetView *>
            filteredView,
        gsl::not_null<const simulation::model::Simulation *> simulation);

    void render();

  private:
    gsl::not_null<const storage::dataset::FilteredDatasetView *> m_filteredView;
    gsl::not_null<const simulation::model::Simulation *> m_simulation;
};

} // namespace ui::analysis::components
