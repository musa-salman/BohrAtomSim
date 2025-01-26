#include <imgui.h>

#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog(
    const std::function<void(const Simulation &)> &on_submit)
    : on_submit(on_submit) {}

void AddSimulationDialog::draw() {
    if (ImGui::Button("Add Simulation")) {
        ImGui::OpenPopup("Add Simulation");
    }

    if (ImGui::Begin("Add Simulation")) {

        // Simulation name
        ImGui::InputText("Name", simulation.name, sizeof(simulation.name));
        ImGui::Separator();

        // Simulation type
        ImGui::Text("Simulation Type");
        ImGui::RadioButton("2D non-relativistic", &simulation.type, POLAR);
        ImGui::RadioButton("3D non-relativistic", &simulation.type, SPHERICAL);
        ImGui::RadioButton("2D relativistic", &simulation.type, REL_POLAR);
        ImGui::RadioButton("3D relativistic", &simulation.type, REL_SPHERICAL);
        ImGui::Separator();

        // Electron orbit
        ImGui::Text("Electron Orbit");
        ImGui::InputScalar("Principal", ImGuiDataType_U8,
                           &simulation.orbit.principal);
        ImGui::InputScalar("Angular", ImGuiDataType_U8,
                           &simulation.orbit.angular);
        ImGui::InputScalar("Magnetic", ImGuiDataType_U8,
                           &simulation.orbit.magnetic);
        ImGui::Separator();

        // Simulation parameters
        ImGui::Text("Simulation Parameters");
        ImGui::InputFloat("Revolutions", &simulation.revolutions);
        ImGui::InputScalar("Time Interval", ImGuiDataType_Double,
                           &simulation.time_interval);
        ImGui::Separator();

        // Submit button
        if (ImGui::Button("Submit")) {
            on_submit(simulation);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // Cancel button
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::End();
    }
}