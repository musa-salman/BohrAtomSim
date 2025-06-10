#ifndef UI_POTENTIAL_VIEW_HPP
#define UI_POTENTIAL_VIEW_HPP

#include <memory>
#include <string>
#include <vector>

#include "simulation/factories/PotentialBuilder.hpp"
#include "simulation/model/Potential.hpp"
#include "storage/persistence/PotentialRepository.hpp"

namespace ui {

class PotentialsView {
  public:
    PotentialsView();

    void render();

  private:
    void renderPotentialList();
    void renderPotentialForm();

    void refreshPotentials();

    storage::persistence::PotentialRepository &potentialRepo;
    std::vector<std::unique_ptr<simulation::model::Potential>> potentials;

    simulation::factories::PotentialBuilder builder;

    char nameBuffer[128] = "";
    bool showError = false;
    std::string errorMessage;
};
} // namespace ui

#endif // UI_POTENTIAL_VIEW_HPP