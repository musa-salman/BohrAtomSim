#ifndef SIMULATION_ANALYSIS_MANAGER_HPP
#define SIMULATION_ANALYSIS_MANAGER_HPP

#include "simulation_repositories/ISimulationRepository.hpp"
#include "view/Component.hpp"
#include "view/SimulationAnalyzer.hpp"

class SimulationAnalysisManager : public Component {
    std::unordered_map<size_t, std::shared_ptr<SimulationAnalyzer>> simulations;
    std::shared_ptr<ISimulationRepository> simulationRepository;
    std::shared_ptr<SimulationAnalyzer> selectedAnalyzer;

  public:
    SimulationAnalysisManager();

    void render() override;
};

#endif // SIMULATION_ANALYSIS_MANAGER_HPP
