#ifndef NOTEBOOK_UI_HPP
#define NOTEBOOK_UI_HPP

#include <vector>

#include "dialogs/ConfirmationDialog.hpp"
#include "plotter/SimulationVisualizer.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/NotebookRepository.hpp"
#include "view/Component.hpp"
#include "view/PlotUI.hpp"

class NotebookUI : public Component {
  private:
    NotebookRepository &notebookRepository;
    ArchivedSimulationManager &archivedManager;
    SimulationRepository &simulationRepository;
    SimulationVisualizer visualizer;

    PlotUI plotUI;
    std::vector<Notebook> notebooks;
    char notebookName[128];        // NOSONAR
    char notebookDescription[256]; // NOSONAR
    size_t selectedNotebookID;
    bool showNotebookPopup;
    ConfirmationDialog deleteNotebookDialog;

  public:
    explicit NotebookUI(NotebookRepository &repo,
                        SimulationRepository &simulationRepository,
                        ArchivedSimulationManager &archivedManager);
    void render() override;
    void loadNotebooks();
    size_t getSelectedNotebookID();
};

#endif // NOTEBOOK_UI_HPP
