#ifndef SIMULATION_VISUALIZER_HPP
#define SIMULATION_VISUALIZER_HPP

#include <GL/gl.h>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

#include "plotter/IPlotFunction.hpp"
#include "plotter/SimulationPlotter.hpp"
#include "simulation_repositories/NotebookRepository.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "view/Component.hpp"

class SimulationVisualizer : public Component {
  public:
    explicit SimulationVisualizer(
        SimulationRepository &simulationRepository,
        NotebookRepository &notebookRepository,
        ArchivedSimulationManager &simulationArchiveManager);

    void setSelectedNotebook(size_t notebookId);

    void draw() override;

  private:
    SimulationRepository &simulationRepository;
    SimulationPlotter plotter;
    std::vector<std::shared_ptr<Simulation>> simulations;
    std::vector<std::shared_ptr<Simulation>> selectedSimulations;

    std::vector<std::shared_ptr<IPlotFunction>> plotFunctions3DR;
    std::vector<std::shared_ptr<IPlotFunction>> plotFunctions3DNR;
    std::vector<std::shared_ptr<IPlotFunction>> plotFunctions2DR;
    std::vector<std::shared_ptr<IPlotFunction>> plotFunctions2DNR;

    std::unordered_map<size_t, std::vector<std::shared_ptr<IPlotFunction>>>
        assignedFunctions;

    NotebookRepository &notebookRepository;

    size_t selectedNotebookId = 0;

    bool showPlotPopup;
    bool showSimulationSelector;

    bool isPlottingInProgress;
    bool showPlotResults;
    GLuint textureID;

    std::atomic<bool> isPlotLoading = false;

    GLuint loadTexture(const char *filename) const;
    void addPlot(size_t simulationId, std::shared_ptr<IPlotFunction> plotFunc);
    void removePlot(size_t simulationId,
                    std::shared_ptr<IPlotFunction> plotFunc);
    void removeSimulation(size_t simulationId);
    void createCombinedPlot(const std::string &outputFilePath);

    void displaySimulationSelector();
    void manageSelectedSimulations();
    void manageSimulationPlotFunctions(const std::shared_ptr<Simulation> &sim);

    void plotSelectedSimulations();

    void showPlotPopupWindow();
};

#endif // SIMULATION_VISUALIZER_HPP
