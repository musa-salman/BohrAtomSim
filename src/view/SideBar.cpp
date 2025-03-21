#include "view/SideBar.hpp"
#include "imgui.h"

Sidebar::Sidebar() : activeSection(ONGOING_SIMULATIONS) {}

void Sidebar::render() {
    ImGui::Begin("Sidebar", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    if (ImGui::Button("Simulator"))
        activeSection = ONGOING_SIMULATIONS;
    if (ImGui::Button("Simulation Manager"))
        activeSection = SIMULATION_MANAGER;

    ImGui::End();
}

Sidebar::Section Sidebar::getActiveSection() { return activeSection; }
