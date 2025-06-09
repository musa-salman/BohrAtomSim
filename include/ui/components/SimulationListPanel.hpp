#pragma once

#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <functional>
#include <gsl/pointers>
#include <memory>

#include "simulator_runner/Simulation.hpp"

namespace ui::components {

class SimulationListPanel {
  public:
    using OnSelectCallback = std::function<void(size_t)>;

    using SimulationFetcher = std::function<boost::container::flat_map<
        size_t, std::shared_ptr<Simulation>, std::greater<size_t>>()>;

    using CardRenderer = bool (*)(const Simulation &, bool) noexcept;

    SimulationListPanel(OnSelectCallback onSelect, SimulationFetcher fetcher,
                        CardRenderer renderer);

    void render();

  private:
    SimulationFetcher m_fetchSimulations;
    CardRenderer m_renderCard;
    OnSelectCallback m_onSelect;
    size_t m_selectedSimulationId = 0;
};

} // namespace ui::components
