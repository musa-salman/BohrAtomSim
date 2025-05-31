#ifndef POTENTIAL_VIEW_HPP
#define POTENTIAL_VIEW_HPP

#include <memory>
#include <string>
#include <vector>

#include "simulation_repositories/PotentialRepository.hpp"
#include "steppers/PotentialBuilder.hpp"

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

#endif // POTENTIAL_VIEW_HPP