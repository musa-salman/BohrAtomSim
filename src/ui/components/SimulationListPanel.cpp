#include <gsl/span_ext>
#include <gsl/zstring>
#include <imgui.h>

#include "ui/components/SimulationListPanel.hpp"

namespace ui::components {

SimulationListPanel::SimulationListPanel(OnSelectCallback &&onSelect,
                                         SimulationFetcher &&fetcher,
                                         CardRenderer renderer)
    : m_fetchSimulations(std::move(fetcher)), m_renderCard(renderer),
      m_onSelect(std::move(onSelect)) {
    m_selectedSimulationId = 0;
    m_filters.dimension = DimensionFilter::All;
    m_filters.relativity = RelativityFilter::All;
    m_filters.quantization = QuantizationFilter::All;
    m_filters.nameQuery[0] = '\0';
}

void SimulationListPanel::render() {
    _renderSearchingTools();

    for (const auto &[id, simulation] : m_fetchSimulations()) {
        if (!_matchFilters(*simulation))
            continue;
        ImGui::PushID(id);
        if (m_renderCard(*simulation, m_selectedSimulationId == id)) {
            m_selectedSimulationId = id;
            m_onSelect(id);
        }
        ImGui::PopID();
    }
}

void SimulationListPanel::_renderSearchingTools() noexcept {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));

    ImGui::PushItemWidth(90);
    const char *dimOptions[] = {"All Dims", "2D", "3D"};
    int dimCurrent = static_cast<int>(m_filters.dimension);
    ImGui::Combo("##Dim", &dimCurrent, dimOptions, IM_ARRAYSIZE(dimOptions));
    m_filters.dimension = static_cast<DimensionFilter>(dimCurrent);
    ImGui::SameLine();

    ImGui::SameLine();
    const char *relOptions[] = {"All Models", "Relativistic",
                                "Nonrelativistic"};
    int relCurrent = static_cast<int>(m_filters.relativity);
    ImGui::Combo("##Rel", &relCurrent, relOptions, IM_ARRAYSIZE(relOptions));
    m_filters.relativity = static_cast<RelativityFilter>(relCurrent);
    ImGui::SameLine();

    ImGui::SameLine();
    const char *quantOptions[] = {"All Types", "Quantized", "Nonquantized"};
    int quantCurrent = static_cast<int>(m_filters.quantization);
    ImGui::Combo("##Quant", &quantCurrent, quantOptions,
                 IM_ARRAYSIZE(quantOptions));
    m_filters.quantization = static_cast<QuantizationFilter>(quantCurrent);

    ImGui::TextUnformatted("Name:");
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGui::InputText("##Search", m_filters.nameQuery,
                     sizeof(m_filters.nameQuery));
    ImGui::PopItemWidth();

    ImGui::PopItemWidth();
    ImGui::PopStyleVar(2);
}

} // namespace ui::components
