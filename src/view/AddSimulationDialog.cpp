#include <cstdio>
#include <imgui.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/PotentialRepository.hpp"
#include "simulator_runner/Simulation.hpp"
#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog() { resetSimulation(); }

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

    if (!is_open)
        return;

    if (ImGui::Begin("Add Simulation", &is_open,
                     ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize)) {
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
        ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
        ImGui::PushStyleColor(ImGuiCol_Header,
                              ImVec4(0.15f, 0.25f, 0.55f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                              ImVec4(0.2f, 0.4f, 0.9f, 0.9f));

        // State Builder Section
        if (ImGui::CollapsingHeader("State Configuration",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Initial Conditions");
            ImGui::BeginDisabled(is_quant);
            if ((is_3d && ImGui::InputScalarN("Position (r0)",
                                              ImGuiDataType_Double, r0, 3)) ||
                (!is_3d && ImGui::InputScalarN("Position (r0)",
                                               ImGuiDataType_Double, r0, 2))) {
                stateBuilder.setR0(
                    eom::Vector3{r0[0], r0[1], is_3d ? r0[2] : 0.0});
                updateSimulation();
            }

            if ((is_3d && ImGui::InputScalarN("Velocity (v0)",
                                              ImGuiDataType_Double, v0, 3)) ||
                (!is_3d && ImGui::InputScalarN("Velocity (v0)",
                                               ImGuiDataType_Double, v0, 2))) {
                stateBuilder.setV0(
                    eom::Vector3{v0[0], v0[1], is_3d ? v0[2] : 0.0});
                updateSimulation();
            }

            if (ImGui::InputScalar("Position Magnitude", ImGuiDataType_Double,
                                   &r0_mag)) {
                stateBuilder.setR0Magnitude(r0_mag);
                updateSimulation();
            }
            if (ImGui::InputScalar("Velocity Magnitude", ImGuiDataType_Double,
                                   &v0_mag)) {
                stateBuilder.setV0Magnitude(v0_mag);
                updateSimulation();
            }

            const scalar angular_momentum = stateBuilder.getAngularMomentum();
            ImGui::Text("Angular Momentum: %.5e", angular_momentum);

            ImGui::EndDisabled();

            ImGui::Spacing();
            if (ImGui::Checkbox("Relativistic", &is_rel)) {
                stateBuilder.setRelativistic(is_rel);
                updateSimulation();
            }

            ImGui::SameLine();
            if (ImGui::Checkbox("Quantized", &is_quant)) {
                stateBuilder.setQuantized(is_quant);
                updateSimulation();
            }

            ImGui::SameLine();
            if (ImGui::Checkbox("3D Enabled", &is_3d)) {
                stateBuilder.set3D(is_3d);
                updateSimulation();
            }

            ImGui::Spacing();
            if (is_quant) {
                ImGui::Text("Quantum Numbers");
                ImGui::Separator();
                if (ImGui::InputInt("Principal (n)", &principal)) {
                    stateBuilder.setPrincipalQuantumNumber(
                        static_cast<quantum_principle>(principal));
                    updateSimulation();
                }
                if (ImGui::InputInt("Angular (k)", &angular)) {
                    stateBuilder.setAngularQuantumNumber(
                        static_cast<quantum_angular>(angular));
                    updateSimulation();
                }
                if (is_3d && ImGui::InputInt("Magnetic (m)", &magnetic)) {
                    stateBuilder.setMagneticQuantumNumber(
                        static_cast<quantum_magnetic>(magnetic));
                    updateSimulation();
                }
            }
        }
        static double delta_time = 1e-7;
        static double total_duration = 800.0;
        static int record_interval = 10000;

        if (ImGui::CollapsingHeader("Simulation Configuration",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::Text("Simulation Parameters");
            if (ImGui::InputDouble("Delta Time", &delta_time)) {
                simulation.setDeltaTime(delta_time);
            }
            if (ImGui::InputInt("Record Interval", &record_interval)) {
                simulation.setRecordInterval(record_interval);
            }
            if (ImGui::InputDouble("Total Duration", &total_duration)) {
                simulation.setTotalDuration(total_duration);
            }

            static bool isRLocalMaxLimited = false;
            // largest int
            static int rLocalMaxCountLimit = 10;
            if (ImGui::Checkbox("Limit R Local Maxima", &isRLocalMaxLimited)) {
                simulation.setRLocalMaxCountLimit( isRLocalMaxLimited
                                                      ? rLocalMaxCountLimit
                                                      : -1);
            }
            if (isRLocalMaxLimited) {
                ImGui::InputInt("R Local Max Count", &rLocalMaxCountLimit);
                if (rLocalMaxCountLimit < 1) {
                    rLocalMaxCountLimit = 1;
                }
                simulation.setRLocalMaxCountLimit(rLocalMaxCountLimit);
            }
        }

        if (ImGui::CollapsingHeader("Potential Configuration",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Available Potentials");
            if (potentials.empty()) {
                ImGui::Text("No potentials available. Please add one first.");
            } else {
                for (const auto &potential : potentials) {
                    ImGui::PushID(potential->getId());
                    if (ImGui::Selectable(
                            potential->getName().c_str(),
                            potential->getId() ==
                                simulation.getPotential().getId())) {
                        simulation.setPotential(*potential);
                    }
                    ImGui::PopID();
                }

                // Potential details
                if (simulation.getPotential().getId() != 0) {
                    ImGui::Separator();
                    ImGui::Text("Selected Potential: %s",
                                simulation.getPotential().getName().c_str());

                    ImGui::Text(
                        "Expression: %s",
                        simulation.getPotential().getExpression().c_str());

                    // Display constants editor
                    ImGui::Text("Constants:");
                    auto &constants = simulation.getConstantValues();
                    for (const auto &[key, value] : constants) {
                        ImGui::PushID(key.c_str());
                        double val = value;
                        if (ImGui::InputDouble(key.c_str(), &val)) {
                            simulation.setConstantValue(key, val);
                        }
                        ImGui::PopID();
                    }
                }
            }
        }

        if (ImGui::Button("Submit")) {
            is_open = false;
            simulation.setDeltaTime(delta_time);
            simulation.setTotalDuration(total_duration);
            simulation.setRecordInterval(record_interval);
            simulation.setRelativistic(is_rel);
            simulation.setQuantized(is_quant);
            simulation.set3D(is_3d);
            simulation.setR0(eom::Vector3{r0[0], r0[1], is_3d ? r0[2] : 0.0});
            simulation.setV0(eom::Vector3{v0[0], v0[1], is_3d ? v0[2] : 0.0});

            on_submit(simulation);
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            resetSimulation();
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            is_open = false;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        ImGui::End();
    }
}

void AddSimulationDialog::resetSimulation() {
    simulation = Simulation();
    stateBuilder = StateBuilder();
    stateBuilder.setQuantized(true);
    stateBuilder.set3D(true);

    stateBuilder.setPrincipalQuantumNumber(3);
    stateBuilder.setAngularQuantumNumber(2);
    stateBuilder.setMagneticQuantumNumber(1);

    stateBuilder.setQuantized(false);

    potentials =
        ServiceLocator::getInstance().get<PotentialRepository>().getAll();

    updateSimulation();
}

void AddSimulationDialog::updateSimulation() {
    const eom::Vector3 default_r0 = stateBuilder.getR0();
    r0[0] = default_r0.x;
    r0[1] = default_r0.y;
    r0[2] = default_r0.z;
    r0_mag = default_r0.magnitude();

    const eom::Vector3 default_v0 = stateBuilder.getV0();
    v0[0] = default_v0.x;
    v0[1] = default_v0.y;
    v0[2] = default_v0.z;
    v0_mag = default_v0.magnitude();
    is_rel = stateBuilder.isRelativistic();
    is_quant = stateBuilder.isQuantized();
    is_3d = stateBuilder.is3D();
    principal = stateBuilder.getPrincipalQuantumNumber();
    angular = stateBuilder.getAngularQuantumNumber();
    magnetic = stateBuilder.getMagneticQuantumNumber();
}

void AddSimulationDialog::formatName(char *name) {
    // randomly generate a name
    if (name[0] == '\0') {
        snprintf(name, 200, "Simulation %s", date);
    } else {
        snprintf(name, 200, "%s %s", name, date);
    }
}