#ifndef UI_POTENTIAL_VIEW_HPP
#define UI_POTENTIAL_VIEW_HPP

#include <memory>
#include <string>
#include <vector>

#include "simulation_repositories/PotentialRepository.hpp"
#include "steppers/PotentialBuilder.hpp"

namespace ui {

class PotentialsView {
  public:
    PotentialsView();

    void render();

  private:
    void renderPotentialList();
    void renderPotentialForm();

    void refreshPotentials();

    PotentialRepository &potentialRepo;
    std::vector<std::unique_ptr<Potential>> potentials;

    PotentialBuilder builder;

    char nameBuffer[128] = "";
    bool showError = false;
    std::string errorMessage;
};
} // namespace ui

#endif // UI_POTENTIAL_VIEW_HPP