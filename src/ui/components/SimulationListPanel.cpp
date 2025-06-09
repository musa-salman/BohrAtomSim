#include <imgui.h>

#include "ui/components/SimulationListPanel.hpp"

namespace ui::components {

SimulationListPanel::SimulationListPanel(OnSelectCallback onSelect,
                                         SimulationFetcher fetcher,
                                         CardRenderer renderer)
    : m_fetchSimulations(fetcher), m_renderCard(renderer),
      m_onSelect(onSelect) {
    m_selectedSimulationId = 0;
}

void SimulationListPanel::render() {
    int cardId = 0;
    for (const auto &[id, simulation] : m_fetchSimulations()) {

        ImGui::PushID(cardId++);
        if (m_renderCard(*simulation, m_selectedSimulationId == id)) {
            m_selectedSimulationId = id;
            m_onSelect(id);
        }
        ImGui::PopID();
    }
}

} // namespace ui::components
