#ifndef SIDEBAR_HPP
#define SIDEBAR_HPP

#include "view/Component.hpp"

class Sidebar : public Component {
  public:
    enum Section { ONGOING_SIMULATIONS, SIMULATION_MANAGER, NOTEBOOK, NONE };

  private:
    Section activeSection;

  public:
    Sidebar();
    void render() override;
    Section getActiveSection();
};

#endif // SIDEBAR_HPP