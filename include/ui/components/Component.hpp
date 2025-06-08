#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

namespace ui::components {

class Component {
  public:
    virtual void render() = 0;
    virtual ~Component() = default;
};

} // namespace ui::components

#endif // UI_COMPONENT_HPP
