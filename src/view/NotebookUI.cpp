#include <cstring>
#include <format>
#include <imgui.h>

#include "dialogs/ConfirmationDialog.hpp"
#include "plotter/SimulationVisualizer.hpp"
#include "simulation_repositories/NotebookRepository.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "view/NotebookUI.hpp"
#include "view/PlotUI.hpp"

NotebookUI::NotebookUI(NotebookRepository &notebookRepository,
                       SimulationRepository &simulationRepository,
                       ArchivedSimulationManager &archivedManager)
    : notebookRepository(notebookRepository), archivedManager(archivedManager),
      simulationRepository(simulationRepository),
      visualizer(simulationRepository, notebookRepository, archivedManager),
      plotUI(notebookRepository, visualizer), selectedNotebookID(0),
      showNotebookPopup(false),
      deleteNotebookDialog("Are you sure you want to delete this notebook?") {
    memset(notebookName, 0, sizeof(notebookName));
    memset(notebookDescription, 0, sizeof(notebookDescription));
    loadNotebooks();
}

void NotebookUI::loadNotebooks() {
    notebooks = notebookRepository.getNotebooks();
}

void NotebookUI::render() {
    if (ImGui::Begin("Notebook Explorer", nullptr,
                     ImGuiWindowFlags_NoCollapse)) {
        ImGuiID dockspace_id = ImGui::GetID("NotebookDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);
        ImGui::End();
    }

    deleteNotebookDialog.render();

    if (ImGui::Begin("Notebook Manager", nullptr,
                     ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Notebook List");

        if (ImGui::Button("Add Notebook")) {
            showNotebookPopup = true;
        }

        ImGui::Separator();

        for (const auto &notebook : notebooks) {
            if (ImGui::Selectable(notebook.name.c_str(),
                                  selectedNotebookID == notebook.id)) {
                selectedNotebookID = notebook.id;
                plotUI.setSelectedNotebook(notebook.id);
            }

            ImGui::SameLine();
            ImGui::PushID(std::format("dn{}", notebook.id).c_str());
            if (ImGui::Button("X")) {
                deleteNotebookDialog.open([this, notebook]() {
                    notebookRepository.removeNotebook(notebook.id);
                    loadNotebooks();
                    selectedNotebookID = 0;
                });
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    if (ImGui::Begin("Notebook Content", nullptr,
                     ImGuiWindowFlags_NoCollapse)) {
        if (selectedNotebookID == 0) {
            ImGui::Text("Select a notebook to view contents.");
        } else {
            ImGui::Text("Notebook Content");
            plotUI.render();
        }
        ImGui::End();
    }

    if (showNotebookPopup && ImGui::Begin("Create Notebook", &showNotebookPopup,
                                          ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Notebook Name:");
        ImGui::InputText("##notebook_name", notebookName,
                         IM_ARRAYSIZE(notebookName));

        if (ImGui::Button("Create") && strlen(notebookName) > 0) {
            Notebook newNotebook = {0, notebookName, notebookDescription};
            notebookRepository.createNotebook(newNotebook);
            loadNotebooks();
            memset(notebookName, 0, sizeof(notebookName));
            memset(notebookDescription, 0, sizeof(notebookDescription));
            showNotebookPopup = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showNotebookPopup = false;
        }

        ImGui::End();
    }
}
