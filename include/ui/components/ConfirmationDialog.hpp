#ifndef CONFIRMATIONDIALOG_HPP
#define CONFIRMATIONDIALOG_HPP

#include <functional>
#include <string>

#include "Component.hpp"

namespace ui::components {

class ConfirmationDialog : public Component {
  public:
    explicit ConfirmationDialog(const std::string &message);

    void open(const std::function<void()> &onConfirm);
    void render() override;

  private:
    std::string message;
    bool isOpen = false;
    std::function<void()> confirmCallback;
};
} // namespace ui::components

#endif // UI_CONFIRMATION_DIALOG_HPP
