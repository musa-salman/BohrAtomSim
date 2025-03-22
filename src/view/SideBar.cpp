#include "imgui.h"
#include "view/SideBar.hpp"

Sidebar::Sidebar() : isCollapsed(true), activeSection(CURRENT_SIMULATIONS) {}

void Sidebar::render() {
    ImGui::Begin("Sidebar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    if (ImGui::Button(isCollapsed ? ">" : "<")) {
        isCollapsed = !isCollapsed;
    }

    if (isCollapsed) {
        renderCollapsedView();
    } else {
        renderExpandedView();
    }

    ImGui::End();
}

Sidebar::Section Sidebar::getActiveSection() { return activeSection; }

void Sidebar::renderCollapsedView() {
    if (ImGui::Button("🔄"))
        activeSection = CURRENT_SIMULATIONS;
    if (ImGui::Button("➕"))
        activeSection = ADD_SIMULATION;
    if (ImGui::Button("📂"))
        activeSection = SIMULATION_MANAGER;
    if (ImGui::Button("⚙"))
        activeSection = SETTINGS;
}

void Sidebar::renderExpandedView() {
    if (ImGui::Button("🔄 Current Simulations"))
        activeSection = CURRENT_SIMULATIONS;
    if (ImGui::Button("➕ Add Simulation"))
        activeSection = ADD_SIMULATION;
    if (ImGui::Button("📂 Simulation Manager"))
        activeSection = SIMULATION_MANAGER;
    if (ImGui::Button("⚙ Settings"))
        activeSection = SETTINGS;
}
