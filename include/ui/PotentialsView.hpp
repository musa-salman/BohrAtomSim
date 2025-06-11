#ifndef UI_POTENTIAL_VIEW_HPP
#define UI_POTENTIAL_VIEW_HPP

#include <memory>
#include <string>
#include <vector>

#include "simulation/factories/PotentialBuilder.hpp"
#include "simulation/model/Potential.hpp"
#include "storage/persistence/PotentialRepository.hpp"
#include "ui/components/ExpressionHelper.hpp"

namespace ui {

class PotentialsView {
  public:
    PotentialsView();

    void render();

  private:
    void _renderPotentialList();
    void _renderPotentialForm();

    void _refreshPotentials();

    storage::persistence::PotentialRepository &m_potentialRepo;
    std::vector<std::unique_ptr<simulation::model::Potential>> m_potentials;
    std::string m_potExpr;

    simulation::factories::PotentialBuilder m_potentialBuilder;
    ui::components::ExpressionHelper m_expressionHelper;

    bool m_showError = false;
    std::string m_errorMessage;
};
} // namespace ui

#endif // UI_POTENTIAL_VIEW_HPP