#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Component {
  public:
    virtual void draw() = 0;
    virtual ~Component() = default;
};

#endif // COMPONENT_HPP
