#include <imgui.h>

#include "dialogs/ConfirmationDialog.hpp"

ConfirmationDialog::ConfirmationDialog(const std::string &message)
    : message(message), isOpen(false), confirmCallback(nullptr) {}

void ConfirmationDialog::open(const std::function<void()> &onConfirm) {
    confirmCallback = onConfirm;
    isOpen = true;
}

void ConfirmationDialog::render() {
    if (isOpen) {
        ImGui::OpenPopup("Confirmation");
    }

    if (ImGui::BeginPopupModal("Confirmation", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", message.c_str());
        ImGui::Separator();

        if (ImGui::Button("Yes")) {
            if (confirmCallback)
                confirmCallback();
            ImGui::CloseCurrentPopup();
            isOpen = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("No")) {
            ImGui::CloseCurrentPopup();
            isOpen = false;
        }

        ImGui::EndPopup();
    }
}
