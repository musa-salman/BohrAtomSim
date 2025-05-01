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

    if (!is_open)
        return;

    ImGui::Begin("Add Simulation", &is_open,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    {
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

        static double r_0 = simulation.getR0();
        static double v_0 = simulation.getV0();
        static double theta_rv = simulation.getThetaRV();

        static double vecRadius[2] = {simulation.getR0X(), simulation.getR0Y()};

        static double vecVelocity[2] = {simulation.getV0X(),
                                        simulation.getV0Y()};

        static unsigned short record_interval = simulation.getRecordInterval();
        static double total_duration = simulation.getTotalDuration();
        static double delta_time = simulation.getDeltaTime();

        static double angular_momentum = simulation.getR0() *
                                         simulation.getV0() *
                                         sin(simulation.getThetaRV());

        static bool isInitialRadiusChanged = false;
        static bool isInitialVelocityChanged = false;
        static bool isInitialAngleChanged = false;

        static bool isRadiusXChanged = false;
        static bool isRadiusYChanged = false;

        static bool isVelocityXChanged = false;
        static bool isVelocityYChanged = false;

        static bool isRecordIntervalChanged = false;
        static bool isTotalDurationChanged = false;
        static bool isDeltaTimeChanged = false;

        static bool isQuantized = false;

        static bool isPrincipleQuantumNumberChanged = false;
        static bool isAzimuthalQuantumNumberChanged = false;

        static uint8_t principle_quantum_number = 3;
        static uint8_t azimuthal_quantum_number = 2;

        ImGui::Text("Simulation Parameters");
        ImGui::Text("Angular Momentum = %.7e", angular_momentum);

        ImGui::Checkbox("Quantized", &isQuantized);
        if (isQuantized) {
            ImGui::Text("Quantum Numbers");
            ImGui::SameLine();
            uint8_t step = 1;
            isPrincipleQuantumNumberChanged = ImGui::InputScalar(
                "n", ImGuiDataType_U8, &principle_quantum_number, &step);
            ImGui::SameLine();
            isAzimuthalQuantumNumberChanged = ImGui::InputScalar(
                "l", ImGuiDataType_U8, &azimuthal_quantum_number, &step);
            if (isPrincipleQuantumNumberChanged ||
                isAzimuthalQuantumNumberChanged) {
                simulation.quantize(principle_quantum_number,
                                    azimuthal_quantum_number);

                r_0 = simulation.getR0();
                theta_rv = simulation.getThetaRV();
                v_0 = simulation.getV0();
                vecRadius[0] = simulation.getR0X();
                vecRadius[1] = simulation.getR0Y();
                vecVelocity[0] = simulation.getV0X();
                vecVelocity[1] = simulation.getV0Y();
                angular_momentum = simulation.getR0() * simulation.getV0() *
                                   sin(simulation.getThetaRV());
            }
        }

        if (isQuantized)
            ImGui::BeginDisabled();
        isInitialRadiusChanged = ImGui::InputDouble("Initial Radius", &r_0);
        if (isInitialRadiusChanged) {
            simulation.setR0(r_0);

            theta_rv = simulation.getThetaRV();
            vecRadius[0] = simulation.getR0X();
            vecRadius[1] = simulation.getR0Y();

            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }

        ImGui::Text("vector (r) =");
        ImGui::SameLine();
        isRadiusXChanged = ImGui::InputDouble("x##r", &vecRadius[0]);
        if (isRadiusXChanged) {
            simulation.setR0X(vecRadius[0]);

            r_0 = simulation.getR0();
            theta_rv = simulation.getThetaRV();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }
        ImGui::SameLine();
        isRadiusYChanged = ImGui::InputDouble("y##r", &vecRadius[1]);
        if (isRadiusYChanged) {
            simulation.setR0Y(vecRadius[1]);

            r_0 = simulation.getR0();
            theta_rv = simulation.getThetaRV();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }

        isInitialVelocityChanged = ImGui::InputDouble("Initial Velocity", &v_0);
        if (isInitialVelocityChanged) {
            simulation.setV0(v_0);

            theta_rv = simulation.getThetaRV();
            vecVelocity[0] = simulation.getV0X();
            vecVelocity[1] = simulation.getV0Y();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }
        ImGui::Text("vector (v) =");
        ImGui::SameLine();
        isVelocityXChanged = ImGui::InputDouble("x##v", &vecVelocity[0]);
        if (isVelocityXChanged) {
            simulation.setV0X(vecVelocity[0]);

            v_0 = simulation.getV0();
            theta_rv = simulation.getThetaRV();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }

        ImGui::SameLine();
        isVelocityYChanged = ImGui::InputDouble("y##v", &vecVelocity[1]);
        if (isVelocityYChanged) {
            simulation.setV0Y(vecVelocity[1]);

            v_0 = simulation.getV0();
            theta_rv = simulation.getThetaRV();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
        }

        isInitialAngleChanged =
            ImGui::InputDouble("Initial Angle (theta)", &theta_rv);
        if (isInitialAngleChanged) {
            simulation.setThetaRV(theta_rv);

            vecRadius[0] = simulation.getR0X();
            vecRadius[1] = simulation.getR0Y();

            vecVelocity[0] = simulation.getV0X();
            vecVelocity[1] = simulation.getV0Y();
            angular_momentum = simulation.getR0() * simulation.getV0() *
                               sin(simulation.getThetaRV());
            theta_rv = simulation.getThetaRV();
            r_0 = simulation.getR0();
            v_0 = simulation.getV0();
        }
        if (isQuantized)
            ImGui::EndDisabled();
        ImGui::Separator();

        ImGui::Text("Simulation Control");
        isTotalDurationChanged = ImGui::InputDouble(
            "Total Duration", &total_duration, 10, 100, "%.7e");
        if (isTotalDurationChanged) {
            simulation.setTotalDuration(total_duration);
        }
        isDeltaTimeChanged =
            ImGui::InputDouble("Time Interval", &delta_time, 0.01, 0.1, "%.3e");
        if (isDeltaTimeChanged) {
            if (delta_time <= 0) {
                delta_time = simulation.getDeltaTime();
            }
            simulation.setDeltaTime(delta_time);
        }
        isRecordIntervalChanged = ImGui::InputScalar(
            "Record Interval", ImGuiDataType_U16, &record_interval);
        if (isRecordIntervalChanged) {
            simulation.setRecordInterval(record_interval);
        }
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
    }
    ImGui::End();
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