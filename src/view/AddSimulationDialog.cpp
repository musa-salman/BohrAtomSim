#include <imgui.h>
#include <sys/stat.h>

#include "simulator_runner/Simulation.hpp"
#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog() { simulation = Simulation(); }

void AddSimulationDialog::setOnSubmit(
    const std::function<void(const Simulation &)> &_on_submit) {
    this->on_submit = _on_submit;
}

void AddSimulationDialog::render() {
    static bool is_open = false;
    if (ImGui::Button("Add Simulation")) {
        is_open = true;
        ImGui::OpenPopup("Add Simulation");
    }

    if (is_open && ImGui::Begin("Add Simulation")) {
        static char name[20]; // NOSONAR
        // Simulation name
        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        simulation.name = std::string(name);
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

            is_open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // Cancel button
        if (ImGui::Button("Cancel")) {
            is_open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::End();
    }
}