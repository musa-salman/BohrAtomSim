#include <imgui.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/PotentialRepository.hpp"
#include "view/PotentialsView.hpp"

PotentialsView::PotentialsView()
    : potentialRepo(ServiceLocator::getInstance().get<PotentialRepository>()) {
    refreshPotentials();
}

void PotentialsView::refreshPotentials() {
    potentials = potentialRepo.getAll();
}

void PotentialsView::render() {
    ImGui::BeginChild("Potential Manager");

    renderPotentialList();
    ImGui::Separator();
    renderPotentialForm();

    ImGui::EndChild();
}

void PotentialsView::renderPotentialList() {
    ImGui::Text("Existing Potentials:");
    for (const auto &potential : potentials) {
        ImGui::PushID(potential->getId());

        ImGui::Text("Name: %s", potential->getName().c_str());
        ImGui::Text("Type: %d", static_cast<int>(potential->getType()));
        ImGui::Text("Expr: %s", potential->getExpression().c_str());

        const auto &constants = potential->getConstants();
        if (!constants.empty()) {
            ImGui::Text("Constants:");
            for (const auto &[name, value] : constants) {
                ImGui::BulletText("%s = %f", name.c_str(), value);
            }
        }

        ImGui::Spacing();

        if (ImGui::Button("Delete")) {
            // TODO: Check if used by any Simulation
            potentialRepo.remove(potential->getId());
            refreshPotentials();
        }

        ImGui::Separator();
        ImGui::PopID();
    }
}

void PotentialsView::renderPotentialForm() {
    ImGui::Text("Add New Potential:");

    ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));

    static int type = static_cast<int>(PotentialType::GeneralExpression);
    ImGui::Combo("Type", &type, "Coulomb\0GeneralExpression\0");
    builder.setPotentialType(static_cast<PotentialType>(type));

    static char exprBuffer[256] = "";
    ImGui::InputText("Expression", exprBuffer, sizeof(exprBuffer));
    builder.setPotentialExpression(exprBuffer);
    //     errorMessage.clear();
    // else {
    //     errorMessage = "Invalid expression.";
    //     showError = true;
    // }

    ImGui::Text("Constants:");
    static char constName[64] = "";
    static float constValue = 1.0f;

    ImGui::InputText("Const Name", constName, sizeof(constName));
    ImGui::InputFloat("Const Value", &constValue);

    if (ImGui::Button("Add Constant")) {
        builder.addPotentialConstant(constName, constValue);
        constName[0] = '\0';
        constValue = 1.0f;
    }

    const auto &constants = builder.getPotentialConstants();
    for (const auto &[name, value] : constants) {
        ImGui::Text("%s = %.3f", name.c_str(), value);
        ImGui::SameLine();
        ImGui::PushID(name.c_str());
        if (ImGui::Button("Remove")) {
            builder.removePotentialConstant(name);
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Create Potential")) {
        if (std::strlen(nameBuffer) == 0) {
            errorMessage = "Name cannot be empty.";
            showError = true;
        } else {
            Potential newPot = builder.build();
            newPot.setName(nameBuffer);
            potentialRepo.add(newPot);
            refreshPotentials();
            builder.clearPotentialConstants();
            exprBuffer[0] = '\0';
            nameBuffer[0] = '\0';
        }
    }

    if (showError) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMessage.c_str());
    }
}
