#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

namespace ui {

class Component {
  public:
    virtual void render() = 0;
    virtual ~Component() = default;
};

} // namespace ui

#endif // UI_COMPONENT_HPP
