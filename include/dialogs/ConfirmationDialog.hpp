#ifndef CONFIRMATIONDIALOG_HPP
#define CONFIRMATIONDIALOG_HPP

#include <functional>
#include <string>

#include "view/Component.hpp"

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

#endif // CONFIRMATIONDIALOG_H
