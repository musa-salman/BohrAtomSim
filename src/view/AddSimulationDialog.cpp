#include <cstdio>
#include <imgui.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "simulator_runner/Simulation.hpp"
#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog() { simulation = Simulation(); }

void AddSimulationDialog::setOnSubmit(
    const std::function<void(const Simulation &)> &_on_submit) {
    simulation.initial_motion_step.time = 0;
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

        simulation.setName(std::string(name));
        ImGui::Separator();

        ImGui::Text("Simulation Parameters");
        ImGui::InputDouble("Initial Radius", &simulation.r_0);
        ImGui::InputDouble("Initial Velocity", &simulation.v_0);
        ImGui::InputDouble("Initial Angle", &simulation.theta_rv);
        ImGui::Separator();

        ImGui::Text("Simulation Control");
        ImGui::InputDouble("Total Duration", &simulation.total_duration);
        ImGui::InputDouble("Time Interval", &simulation.delta_time);
        ImGui::InputScalar("Record Interval", ImGuiDataType_U16,
                           &simulation.record_interval);
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
    // randomly generate a name
    if (name[0] == '\0') {
        snprintf(name, 200, "Simulation %s", date);
    } else {
        snprintf(name, 200, "%s %s", name, date);
    }
}