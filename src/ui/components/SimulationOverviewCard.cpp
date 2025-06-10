#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "simulation/model/Simulation.hpp"
#include "ui/components/SimulationOverviewCard.hpp"

namespace ui::components {
using namespace simulation::model;

void SimulationOverviewCard::render(const Simulation &sim) {
    ImGui::Text("Simulation: %s", sim.getName().c_str());
    ImGui::Separator();

    ImGui::Columns(3, nullptr, false);
    ImGui::Text("ID: %zu", sim.getId());
    ImGui::Text("Status: %s", magic_enum::enum_name(sim.getStatus()).data());

    const auto &r0 = sim.getR0();
    ImGui::Text("r0: [%.3f, %.3f, %.3f]", r0.x, r0.y, r0.z);
    const auto &v0 = sim.getV0();
    ImGui::Text("v0: [%.3f, %.3f, %.3f]", v0.x, v0.y, v0.z);

    ImGui::NextColumn();
    ImGui::Text("Dimensions: %s", sim.is3D() ? "3D" : "2D");
    ImGui::Text("Relativistic: %s", sim.isRelativistic() ? "Yes" : "No");
    ImGui::Text("Quantized: %s", sim.isQuantized() ? "Yes" : "No");

    ImGui::NextColumn();
    ImGui::Text("dt: %.3e", sim.getDeltaTime());
    ImGui::Text("Duration: %.3f", sim.getTotalDuration());
    ImGui::Text("Interval: %u", sim.getRecordInterval());

    if (sim.getRLocalMaxCountLimit() > 0) {
        ImGui::Text("Radial Local Max Count Limit: %d",
                    sim.getRLocalMaxCountLimit());
    }

    ImGui::Columns(1);
    ImGui::Separator();

    if (sim.getConstantValues().empty())
        return;

    ImGui::Text("Constants:");
    if (ImGui::BeginTable("constants_table", 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &[key, value] : sim.getConstantValues()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(key.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3e", value);
        }

        ImGui::EndTable();
    }
}

} // namespace ui::components
