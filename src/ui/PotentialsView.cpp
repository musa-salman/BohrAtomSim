#include <gsl/zstring>
#include <imgui.h>
#include <optional>

#include "storage/persistence/PotentialRepository.hpp"
#include "ui/PotentialsView.hpp"
#include "ui/components/Components.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui {
using namespace simulation::model;
using namespace storage::persistence;
using namespace utils;

PotentialsView::PotentialsView()
    : m_potentialRepo(
          ServiceLocator::getInstance().get<PotentialRepository>()) {
    _refreshPotentials();
}

void PotentialsView::_refreshPotentials() {
    m_potentials = m_potentialRepo.getAll();
}

void PotentialsView::render() {
    ImGui::BeginChild("Potential Manager");

    _renderPotentialList();
    ImGui::Separator();
    _renderPotentialForm();

    ImGui::EndChild();
}

void PotentialsView::_renderPotentialList() {
    m_expressionHelper.render();
    ImGui::Text("Existing Potentials:");
    bool wasPotentialRemoved = false;
    for (const auto &potential : m_potentials) {
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
            m_potentialRepo.remove(potential->getId());
            wasPotentialRemoved = true;
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    if (wasPotentialRemoved) {
        _refreshPotentials();
    }
}

void PotentialsView::_renderPotentialForm() {
    ImGui::Text("Add New Potential:");

    {
        char nameBuffer[512] = "";
        std::strncpy(nameBuffer, m_potentialBuilder.getName().c_str(),
                     sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0';
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            m_potentialBuilder.setName(nameBuffer);
        }
    }

    static int type = static_cast<int>(PotentialType::GeneralExpression);
    ImGui::Combo("Type", &type, "Coulomb\0GeneralExpression\0");
    m_potentialBuilder.setPotentialType(static_cast<PotentialType>(type));

    ui::components::renderAutoGrowInputMultiline("Expression", m_potExpr);
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
        m_potentialBuilder.addPotentialConstant(constName, constValue);
        constName[0] = '\0';
        constValue = 1.0f;
    }

    const auto &constants = m_potentialBuilder.getPotentialConstants();
    std::optional<std::string> toRemove;
    for (const auto &[name, value] : constants) {
        ImGui::Text("%s = %.3f", name.c_str(), value);
        ImGui::SameLine();
        ImGui::PushID(name.c_str());
        if (ImGui::Button("Remove")) {
            toRemove = name;
        }
        ImGui::PopID();
    }

    if (toRemove.has_value()) {
        m_potentialBuilder.removePotentialConstant(toRemove.value());
    }

    if (ImGui::Button("Create Potential")) {
        if (m_potentialBuilder.getName().empty()) {
            m_errorMessage = "Name cannot be empty.";
            m_showError = true;
        } else {
            m_potentialBuilder.setPotentialExpression(m_potExpr);
            Potential newPot = m_potentialBuilder.build();

            m_potentialRepo.add(newPot);
            _refreshPotentials();
            m_potentialBuilder.clearPotentialConstants();
        }
    }

    if (m_showError) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", m_errorMessage.c_str());
    }
}

} // namespace ui