#include <cstdio>
#include <cstring>
#include <gsl/zstring>
#include <imgui.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "physics/math/Vector3.hpp"
#include "physics/math/VectorOps.hpp"
#include "physics/math/types.hpp"
#include "simulation/model/Potential.hpp"
#include "simulation/model/Simulation.hpp"
#include "storage/persistence/PotentialRepository.hpp"
#include "ui/components/SimulationEditorDialog.hpp"
#include "ui/ui_utils.hpp"
#include "utils/ServiceLocator.hpp"

namespace ui::components {
using namespace simulation::model;
using namespace simulation::factories;
using namespace physics::math;
using namespace utils;
using namespace storage::persistence;

SimulationEditorDialog::SimulationEditorDialog()
    : m_isDialogOpen(false), m_isAutoName(true), m_isRLocalMaxLimited(false) {}

void SimulationEditorDialog::setOnSubmit(
    std::function<void(Simulation)> &&onSubmit) {
    this->m_onSubmit = std::move(onSubmit);
}

void SimulationEditorDialog::setSimulation(const Simulation &simulation) {
    m_simulationBuilder.setSimulation(simulation);
    m_isAutoName = !_isLocked(simulation.getStatus());
    m_isDialogOpen = true;
}

void SimulationEditorDialog::render() {
    if (!m_isDialogOpen)
        return;

    const bool kIsEdit = m_simulationBuilder.getSimulation().getId() != 0;
    const bool kIsLocked =
        _isLocked(m_simulationBuilder.getSimulation().getStatus());

    gsl::czstring kPopupTitle = kIsEdit ? "Edit Simulation" : "Add Simulation";

    if (ImGui::Begin(kPopupTitle, &m_isDialogOpen,
                     ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_AlwaysAutoResize)) {

        char name[200]; // NOSONAR
        std::strncpy(name, m_simulationBuilder.getName().c_str(),
                     sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';

        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        if (!kIsEdit) {
            ImGui::SameLine();
            ImGui::Checkbox("Auto", &m_isAutoName);
        }

        if (m_isAutoName) {
            const std::string kFormattedSimulationName =
                createFormattedSimulationName(
                    m_simulationBuilder.getSimulation());
            std::strncpy(name, kFormattedSimulationName.c_str(),
                         sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
        }
        m_simulationBuilder.setName(std::string(name));

        ImGui::Separator();
        ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
        ImGui::PushStyleColor(ImGuiCol_Header,
                              ImVec4(0.15f, 0.25f, 0.55f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                              ImVec4(0.2f, 0.4f, 0.9f, 0.9f));

        _renderStateConfiguration(kIsLocked);
        _renderSimulationConfiguration(kIsLocked);
        _renderPotentialConfiguration(kIsLocked);

        if (ImGui::Button(kIsEdit ? "Apply Changes" : "Submit")) {
            m_isDialogOpen = false;
            m_onSubmit(m_simulationBuilder.build());
        }

        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            m_isDialogOpen = false;
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        ImGui::End();
    }
}

void SimulationEditorDialog::_renderStateConfiguration(bool isLocked) {
    if (!ImGui::CollapsingHeader("State Configuration",
                                 ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::BeginDisabled(isLocked);
    ImGui::Text("Initial Conditions");
    bool kIsQuant = m_simulationBuilder.isQuantized();
    bool kIs3d = m_simulationBuilder.is3D();

    double r0[3] = {m_simulationBuilder.getR0().x,
                    m_simulationBuilder.getR0().y,
                    m_simulationBuilder.getR0().z};

    ImGui::BeginDisabled(kIsQuant);
    if ((kIs3d &&
         ImGui::InputScalarN("Position (r0)", ImGuiDataType_Double, r0, 3)) ||
        (!kIs3d &&
         ImGui::InputScalarN("Position (r0)", ImGuiDataType_Double, r0, 2))) {
        m_simulationBuilder.setR0(Vector3{r0[0], r0[1], r0[2]});
    }

    double v0[3] = {m_simulationBuilder.getV0().x,
                    m_simulationBuilder.getV0().y,
                    m_simulationBuilder.getV0().z};
    if ((kIs3d &&
         ImGui::InputScalarN("Velocity (v0)", ImGuiDataType_Double, v0, 3)) ||
        (!kIs3d &&
         ImGui::InputScalarN("Velocity (v0)", ImGuiDataType_Double, v0, 2))) {
        m_simulationBuilder.setV0(Vector3{v0[0], v0[1], kIs3d ? v0[2] : 0.0});
    }

    ImGui::EndDisabled();

    double r0_mag = computeMagnitude(m_simulationBuilder.getR0());
    if (ImGui::InputScalar("Position Magnitude", ImGuiDataType_Double,
                           &r0_mag)) {
        m_simulationBuilder.setR0Magnitude(r0_mag);
    }

    double v0_mag = computeMagnitude(m_simulationBuilder.getV0());
    if (ImGui::InputScalar("Velocity Magnitude", ImGuiDataType_Double,
                           &v0_mag)) {
        m_simulationBuilder.setV0Magnitude(v0_mag);
    }

    ImGui::Text("Angular Momentum: %.5e",
                m_simulationBuilder.getAngularMomentum());

    ImGui::Spacing();

    bool kIsRel = m_simulationBuilder.isRelativistic();
    if (ImGui::Checkbox("Relativistic", &kIsRel))
        m_simulationBuilder.setRelativistic(kIsRel);

    ImGui::SameLine();
    if (ImGui::Checkbox("Quantized", &kIsQuant))
        m_simulationBuilder.setQuantized(kIsQuant);

    ImGui::SameLine();
    if (ImGui::Checkbox("3D Enabled", &kIs3d))
        m_simulationBuilder.set3D(kIs3d);

    if (kIsQuant) {
        ImGui::Text("Quantum Numbers");
        ImGui::Separator();

        int principal =
            static_cast<int>(m_simulationBuilder.getPrincipalQuantumNumber());
        if (ImGui::InputInt("Principal (n)", &principal))
            m_simulationBuilder.setPrincipalQuantumNumber(
                static_cast<quantum_principle>(principal));

        int angular =
            static_cast<int>(m_simulationBuilder.getAngularQuantumNumber());
        if (ImGui::InputInt("Angular (k)", &angular))
            m_simulationBuilder.setAngularQuantumNumber(
                static_cast<quantum_angular>(angular));

        int magnetic =
            static_cast<int>(m_simulationBuilder.getMagneticQuantumNumber());
        if (kIs3d && ImGui::InputInt("Magnetic (m)", &magnetic))
            m_simulationBuilder.setMagneticQuantumNumber(
                static_cast<quantum_magnetic>(magnetic));
    }
    ImGui::EndDisabled();
}

void SimulationEditorDialog::_renderSimulationConfiguration(bool isLocked) {
    if (!ImGui::CollapsingHeader("Simulation Configuration",
                                 ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::Text("Simulation Parameters");

    ImGui::BeginDisabled(isLocked);
    double deltaTime = m_simulationBuilder.getDeltaTime();
    if (ImGui::InputDouble("Delta Time", &deltaTime, 0, 0, "%.5e")) {
        m_simulationBuilder.setDeltaTime(deltaTime);
    }

    int recordInterval = m_simulationBuilder.getRecordInterval();
    if (ImGui::InputInt("Record Interval", &recordInterval)) {
        m_simulationBuilder.setRecordInterval(recordInterval);
    }

    double totalDuration = m_simulationBuilder.getTotalDuration();
    if (ImGui::InputDouble("Total Duration", &totalDuration)) {
        m_simulationBuilder.setTotalDuration(totalDuration);
    }

    if (ImGui::Checkbox("Limit R Local Maxima", &m_isRLocalMaxLimited)) {
        m_simulationBuilder.setRLocalMaxCountLimit(m_isRLocalMaxLimited ? 10
                                                                        : -1);
    }

    int rLocalMaxCountLimit = m_simulationBuilder.getRLocalMaxCountLimit();
    if (m_isRLocalMaxLimited) {
        ImGui::InputInt("R Local Max Count", &rLocalMaxCountLimit);
        if (rLocalMaxCountLimit < 1)
            m_simulationBuilder.setNotLimitedRLocalMaxCount();
        m_simulationBuilder.setRLocalMaxCountLimit(rLocalMaxCountLimit);
    }
    ImGui::EndDisabled();
}

void SimulationEditorDialog::_renderPotentialConfiguration(bool isLocked) {
    if (!ImGui::CollapsingHeader("Potential Configuration",
                                 ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::Text("Available Potentials");

    _loadPotentials();

    ImGui::BeginDisabled(isLocked);
    for (const auto &potential : m_potentials) {
        ImGui::PushID(potential->getId());
        if (ImGui::Selectable(potential->getName().c_str(),
                              potential->getId() ==
                                  m_simulationBuilder.getPotential().getId())) {
            m_simulationBuilder.setPotential(*potential);
        }
        ImGui::PopID();
    }

    const Potential &selected = m_simulationBuilder.getPotential();
    if (selected.getId() != 0) {
        ImGui::Separator();
        ImGui::Text("Selected Potential: %s", selected.getName().c_str());
        ImGui::Text("dU/dr = %s", selected.getExpression().c_str());

        const auto &constants = m_simulationBuilder.getConstantValues();
        if (!constants.empty()) {
            ImGui::Separator();
            ImGui::Text("Constants:");
            ImGui::BeginDisabled(isLocked);
            for (const auto &[key, value] : constants) {
                ImGui::PushID(key.c_str());
                double val = value;
                if (ImGui::InputDouble(key.c_str(), &val)) {
                    m_simulationBuilder.setConstantValue(key, val);
                }
                ImGui::PopID();
            }
            ImGui::EndDisabled();
        }
    }
    ImGui::EndDisabled();
}

void SimulationEditorDialog::_loadPotentials() {
    auto potentials =
        ServiceLocator::getInstance().get<PotentialRepository>().getAll();
    if (potentials.size() != this->m_potentials.size()) {
        this->m_potentials = std::move(potentials);
    }
}
} // namespace ui::components
