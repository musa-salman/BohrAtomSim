#pragma once

#include <boost/container/flat_map.hpp>
#include <cstddef>
#include <functional>
#include <memory>

#include "simulator_runner/Simulation.hpp"

namespace ui::components {

class SimulationListPanel {
  public:
    using OnSelectCallback = std::function<void(size_t)>;

    using SimulationFetcher = std::function<boost::container::flat_map<
        size_t, std::shared_ptr<Simulation>, std::greater<size_t>>()>;

    using CardRenderer = bool (*)(const Simulation &, bool) noexcept;

    SimulationListPanel(OnSelectCallback &&onSelect,
                        SimulationFetcher &&fetcher, CardRenderer renderer);

    void render();

  private:
    enum class DimensionFilter { All, D2, D3 };
    enum class RelativityFilter { All, Relativistic, NonRelativistic };
    enum class QuantizationFilter { All, Quantized, NonQuantized };

    struct Filters {
        DimensionFilter dimension = DimensionFilter::All;
        RelativityFilter relativity = RelativityFilter::All;
        QuantizationFilter quantization = QuantizationFilter::All;
        char nameQuery[128] = {};
    };

    Filters m_filters;

    SimulationFetcher m_fetchSimulations;
    CardRenderer m_renderCard;
    OnSelectCallback m_onSelect;
    size_t m_selectedSimulationId = 0;

    void _renderSearchingTools() noexcept;

    SIM_CONST SIM_INLINE inline bool
    _matchFilters(const Simulation &simulation) const noexcept {
        return (m_filters.dimension == DimensionFilter::All ||
                (m_filters.dimension == DimensionFilter::D2 &&
                 !simulation.getIs3D()) ||
                (m_filters.dimension == DimensionFilter::D3 &&
                 simulation.getIs3D())) &&
               (m_filters.relativity == RelativityFilter::All ||
                (m_filters.relativity == RelativityFilter::Relativistic &&
                 simulation.getIsRelativistic()) ||
                (m_filters.relativity == RelativityFilter::NonRelativistic &&
                 !simulation.getIsRelativistic())) &&
               (m_filters.quantization == QuantizationFilter::All ||
                (m_filters.quantization == QuantizationFilter::Quantized &&
                 simulation.getIsQuantized()) ||
                (m_filters.quantization == QuantizationFilter::NonQuantized &&
                 !simulation.getIsQuantized())) &&
               (m_filters.nameQuery[0] == '\0' ||
                simulation.getName().find(m_filters.nameQuery) !=
                    std::string::npos);
    }
};

} // namespace ui::components
