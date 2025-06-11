#pragma once

#include <imgui.h>

namespace ui::components {

class ExpressionHelper {
  public:
    void render();

  private:
    bool m_isOpen = false;
};

} // namespace ui::components