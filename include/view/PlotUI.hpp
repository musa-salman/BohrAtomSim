#ifndef PLOT_UI_HPP
#define PLOT_UI_HPP

#include <GL/gl.h>
#include <atomic>
#include <vector>

#include "dialogs/ConfirmationDialog.hpp"
#include "plotter/SimulationVisualizer.hpp"
#include "simulation_repositories/NotebookRepository.hpp"
#include "view/Component.hpp"

class PlotUI : public Component {
  private:
    NotebookRepository &repo;
    SimulationVisualizer &visualizer;
    std::vector<std::shared_ptr<Plot>> plots;
    std::vector<GLuint> textureIDs;
    char plotName[128];        // NOSONAR
    char plotDescription[256]; // NOSONAR
    size_t selectedNotebookID;
    bool showPlotPopup;
    bool requiresRefresh = false;

    ConfirmationDialog deletePlotDialog;

    std::atomic<bool> isPlotLoading = false;

    void loadPlots(size_t notebookID);

  public:
    explicit PlotUI(NotebookRepository &repo, SimulationVisualizer &visualizer);
    void draw() override;
    void setSelectedNotebook(size_t notebookID);
};

#endif // PLOT_UI_HPP
