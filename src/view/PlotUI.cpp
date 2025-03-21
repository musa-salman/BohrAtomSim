#include "imgui.h"
#include <cstring>
#include <format>

#include "plotter/TextureLoader.hpp"
#include "view/PlotUI.hpp"

PlotUI::PlotUI(NotebookRepository &repo, SimulationVisualizer &visualizer)
    : repo(repo), visualizer(visualizer), selectedNotebookID(0),
      showPlotPopup(false),
      deletePlotDialog("are you sure you want to delete?") {
    memset(plotName, 0, sizeof(plotName));
    memset(plotDescription, 0, sizeof(plotDescription));
}

void PlotUI::setSelectedNotebook(size_t notebookID) {
    selectedNotebookID = notebookID;
    visualizer.setSelectedNotebook(notebookID);
    loadPlots(notebookID);
}

void PlotUI::loadPlots(size_t notebookID) {
    isPlotLoading = true;
    plots = repo.getPlotsByNotebook(notebookID);

    textureIDs.clear();
    for (const auto &plot : plots) {
        textureIDs.push_back(TextureLoader::loadTexture(
            std::format("{}/plots/{}/{}.png", DB_PATH, selectedNotebookID,
                        plot->id)
                .c_str()));
    }

    isPlotLoading = false;
}

void PlotUI::render() {
    if (isPlotLoading) {
        ImGui::Text("Loading plots...");
        return;
    }

    if (selectedNotebookID == 0) {
        ImGui::Text("Select a notebook to manage plots.");
        return;
    }

    deletePlotDialog.render();

    ImGui::Separator();
    ImGui::Text("Manage Plots");

    // Button to open the Add Plot popup
    if (ImGui::Button("Add Plot")) {
        showPlotPopup = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        requiresRefresh = true;
    }

    ImGui::Separator();
    for (size_t i = 0; i < textureIDs.size(); i++) {

        if (i < plots.size()) {
            const auto &plot = plots[i];
            ImGui::BulletText("%s - %s", plot->name.c_str(),
                              plot->description.c_str());
            ImGui::SameLine();
            ImGui::PushID(std::format("delete_plot_{}", i).c_str());
            if (ImGui::Button("X")) {
                deletePlotDialog.open([this, plot]() {
                    repo.removePlot(plot->id);
                    requiresRefresh = true;
                });
            }
            ImGui::PopID();
        } else {
            ImGui::BulletText("Plot %zu", i);
        }

        GLuint textureID = textureIDs[i];

        ImVec2 imageSize(560, 420);
        ImGui::Image((ImTextureID)(intptr_t)textureID, imageSize);
    }

    if (showPlotPopup && ImGui::Begin("Create New Plot", &showPlotPopup,
                                      ImGuiWindowFlags_AlwaysAutoResize)) {
        visualizer.render();
        ImGui::End();
    }

    if (requiresRefresh) {
        loadPlots(selectedNotebookID);
    }
}
