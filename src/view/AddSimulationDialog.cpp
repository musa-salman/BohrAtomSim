#include <cstdio>
#include <imgui.h>
#include <sys/stat.h>

#include "simulator_runner/Simulation.hpp"
#include "utils/iterator.h"
#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog() { simulation = Simulation(); }

void AddSimulationDialog::setOnSubmit(
    const std::function<void(const Simulation &)> &_on_submit) {
    this->on_submit = _on_submit;
}

void AddSimulationDialog::render() {
    static bool is_open = false;
    static time_t t = time(0);

    if (ImGui::Button("Add Simulation")) {
        is_open = true;
        // update date
        t = time(0);
        struct tm *now = localtime(&t);
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", now);

        ImGui::OpenPopup("Add Simulation");
    }

    if (is_open && ImGui::Begin("Add Simulation")) {
        static char name[200]; // NOSONAR
        static bool auto_name = true;
        // Simulation name
        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        ImGui::SameLine();
        ImGui::Checkbox("Auto", &auto_name);

        if (auto_name)
            formatName(name);

        simulation.name = std::string(name);
        ImGui::Separator();

        // Simulation type
        ImGui::Text("Simulation Type");
        ImGui::RadioButton("2D non-relativistic", &simulation.type, POLAR);
        ImGui::RadioButton("3D non-relativistic", &simulation.type, SPHERICAL);
        ImGui::RadioButton("2D relativistic", &simulation.type, REL_POLAR);
        ImGui::RadioButton("3D relativistic", &simulation.type, REL_SPHERICAL);
        ImGui::RadioButton("3D relativistic with spin", &simulation.type, SPIN);
        ImGui::Separator();

        // Electron orbit
        ImGui::Text("Electron Orbit");
        ImGui::InputScalar("Principal", ImGuiDataType_U8,
                           &simulation.orbit.principal);
        ImGui::InputScalar("Angular", ImGuiDataType_U8,
                           &simulation.orbit.angular);

        if (simulation.type == SPIN || simulation.type == REL_SPHERICAL ||
            simulation.type == SPHERICAL)
            ImGui::InputScalar("Magnetic", ImGuiDataType_U8,
                               &simulation.orbit.magnetic);
        ImGui::Separator();

        // Simulation parameters
        ImGui::Text("Simulation Parameters");
        ImGui::InputFloat("Revolutions", &simulation.revolutions);
        ImGui::InputScalar("Time Interval", ImGuiDataType_Double,
                           &simulation.time_interval, 0, 0, "%e");
        ImGui::Separator();

        // Submit button
        if (ImGui::Button("Submit")) {
            on_submit(simulation);
            resetSimulation();
            is_open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // Cancel button
        if (ImGui::Button("Cancel")) {
            is_open = false;
            resetSimulation();
            ImGui::CloseCurrentPopup();
        }

        ImGui::End();
    }
}

void AddSimulationDialog::resetSimulation() { simulation = Simulation(); }

void AddSimulationDialog::formatName(char *name) {
    if (simulation.type == POLAR || simulation.type == REL_POLAR)
        sprintf(name, "%s_%d_%d_%s", simulation.getType(),
                simulation.orbit.principal, simulation.orbit.angular, date);
    else
        sprintf(name, "%s_%d_%d_%d_%s", simulation.getType(),
                simulation.orbit.principal, simulation.orbit.angular,
                simulation.orbit.magnetic, date);
}