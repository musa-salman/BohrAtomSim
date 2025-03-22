#ifndef SIDEBAR_HPP
#define SIDEBAR_HPP

#include "view/Component.hpp"

class Sidebar : public Component {
  public:
    enum Section { RUNNING_SIMULATIONS, SIMULATION_MANAGER, NONE };

  private:
    Section activeSection;

  public:
    Sidebar();
    void render() override;
    Section getActiveSection();
};

#endif // SIDEBAR_HPP