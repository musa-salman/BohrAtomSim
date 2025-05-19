#include <cstdio>
#include <imgui.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "service_locator/ServiceLocator.hpp"
#include "simulator_runner/Simulation.hpp"
#include "steppers/SimulationStepper.hpp"
#include "steppers/StepperFactory.hpp"
#include "view/AddSimulationDialog.hpp"

AddSimulationDialog::AddSimulationDialog() { simulation = Simulation(); }

void AddSimulationDialog::setOnSubmit(
    const std::function<void(const Simulation &)> &_on_submit) {
    this->on_submit = _on_submit;
}

void AddSimulationDialog::render() {
    static bool is_open = false;
    static time_t t = time(0);

    static StepperType selectedStepper;

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

        // Simulation type combo box
        ImGui::Text("Simulation Type");
        const auto &availableSteppers = ServiceLocator::getInstance()
                                            .get<StepperFactory>()
                                            .availableSteppers();
        static const char *stepperNames[] = {"2D", "2D Symbolic Potential"};

        if (ImGui::BeginCombo(
                "Stepper Type",
                stepperNames[static_cast<int>(selectedStepper)])) {
            for (size_t i = 0; i < availableSteppers.size(); ++i) {
                const char *stepperName = stepperNames[i];
                bool isSelected =
                    (selectedStepper == static_cast<StepperType>(i));
                if (ImGui::Selectable(stepperName, isSelected)) {
                    selectedStepper = static_cast<StepperType>(i);
                    auto &params = simulation.getParams();
                    params.clear();

                    auto tempStepper =
                        ServiceLocator::getInstance()
                            .get<StepperFactory>()
                            .create(selectedStepper, params, []() {}, nullptr);
                    const auto &requiredParams =
                        tempStepper->requiredParameters();
                    for (const auto &param : requiredParams) {
                        if (param.type == ParameterType::Double) {
                            params[param.name] =
                                std::any_cast<double>(param.defaultValue);
                        } else if (param.type == ParameterType::Int) {
                            params[param.name] =
                                std::any_cast<int>(param.defaultValue);
                        } else if (param.type == ParameterType::String) {
                            params[param.name] =
                                std::any_cast<std::string>(param.defaultValue);
                        } else if (param.type == ParameterType::Bool) {
                            params[param.name] =
                                std::any_cast<bool>(param.defaultValue);
                        }
                    }
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Simulation Parameters");

        auto &params = simulation.getParams();
        for (auto &[key, value] : params) {
            if (value.type() == typeid(double)) {
                double &param = std::any_cast<double &>(value);
                ImGui::InputDouble(key.c_str(), &param);
            } else if (value.type() == typeid(int)) {
                int &param = std::any_cast<int &>(value);
                ImGui::InputInt(key.c_str(), &param);
            } else if (value.type() == typeid(bool)) {
                bool &param = std::any_cast<bool &>(value);
                ImGui::Checkbox(key.c_str(), &param);
            } else if (value.type() == typeid(std::string)) {
                std::string &param = std::any_cast<std::string &>(value);
                static char buffer[200];
                ImGui::InputText(key.c_str(), buffer, IM_ARRAYSIZE(buffer));
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    param = std::string(buffer);
                }
            }
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