#ifndef SIDEBAR_HPP
#define SIDEBAR_HPP

#include "view/Component.hpp"

class Sidebar : public Component {
  public:
    enum Section {
        CURRENT_SIMULATIONS,
        ADD_SIMULATION,
        SIMULATION_MANAGER,
        SETTINGS,
        NONE
    };

  private:
    bool isCollapsed;
    Section activeSection;

  public:
    Sidebar();
    void render() override;
    Section getActiveSection();

  private:
    void renderCollapsedView();
    void renderExpandedView();
};

#endif // SIDEBAR_HPP