#include "plotter/TextureLoader.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <future>
#include <imgui.h>
#include <memory>

#include "plotter/ColumnExtractionFunction.hpp"
#include "plotter/SimulationVisualizer.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/NotebookRepository.hpp"

SimulationVisualizer::SimulationVisualizer(
    SimulationRepository &simulationRepository,
    NotebookRepository &_notebookRepository,
    ArchivedSimulationManager &simulationArchiveManager)
    : simulationRepository(simulationRepository),
      plotter(simulationArchiveManager),
      notebookRepository(_notebookRepository) {

    auto fun_r = std::make_shared<ColumnExtractionFunction>("r");
    auto fun_r_dot = std::make_shared<ColumnExtractionFunction>("r_dot");
    auto fun_r_dot_dot =
        std::make_shared<ColumnExtractionFunction>("r_dot_dot");

    auto fun_theta = std::make_shared<ColumnExtractionFunction>("theta");
    auto fun_theta_dot =
        std::make_shared<ColumnExtractionFunction>("theta_dot");
    auto fun_theta_dot_dot =
        std::make_shared<ColumnExtractionFunction>("theta_dot_dot");

    auto fun_phi = std::make_shared<ColumnExtractionFunction>("phi");
    auto fun_phi_dot = std::make_shared<ColumnExtractionFunction>("phi_dot");

    auto fun_psi = std::make_shared<ColumnExtractionFunction>("psi");
    auto fun_psi_dot = std::make_shared<ColumnExtractionFunction>("psi_dot");

    auto fun_gamma = std::make_shared<ColumnExtractionFunction>("gamma");
    auto fun_delta_psi =
        std::make_shared<ColumnExtractionFunction>("delta_psi");

    // 2DNR
    plotFunctions2DNR.push_back(fun_r);
    plotFunctions2DNR.push_back(fun_r_dot);
    plotFunctions2DNR.push_back(fun_r_dot_dot);
    plotFunctions2DNR.push_back(fun_psi);
    plotFunctions2DNR.push_back(fun_psi_dot);

    // 2DR
    plotFunctions2DR.push_back(fun_r);
    plotFunctions2DR.push_back(fun_r_dot);
    plotFunctions2DR.push_back(fun_r_dot_dot);
    plotFunctions2DR.push_back(fun_psi);
    plotFunctions2DR.push_back(fun_psi_dot);
    plotFunctions2DR.push_back(fun_gamma);
    plotFunctions2DR.push_back(fun_delta_psi);

    // 3DNR
    plotFunctions3DNR.push_back(fun_r);
    plotFunctions3DNR.push_back(fun_r_dot);
    plotFunctions3DNR.push_back(fun_r_dot_dot);
    plotFunctions3DNR.push_back(fun_phi);
    plotFunctions3DNR.push_back(fun_phi_dot);
    plotFunctions3DNR.push_back(fun_theta);
    plotFunctions3DNR.push_back(fun_theta_dot);
    plotFunctions3DNR.push_back(fun_theta_dot_dot);

    // 3DR
    plotFunctions3DR.push_back(fun_r);
    plotFunctions3DR.push_back(fun_r_dot);
    plotFunctions3DR.push_back(fun_r_dot_dot);
    plotFunctions3DR.push_back(fun_phi);
    plotFunctions3DR.push_back(fun_phi_dot);
    plotFunctions3DR.push_back(fun_theta);
    plotFunctions3DR.push_back(fun_theta_dot);
    plotFunctions3DR.push_back(fun_theta_dot_dot);
    plotFunctions3DR.push_back(fun_gamma);
    plotFunctions3DR.push_back(fun_delta_psi);
}

void SimulationVisualizer::render() {
    if (ImGui::Begin("Simulation Visualizer")) {
        displaySimulationSelector();
        manageSelectedSimulations();
        showPlotPopupWindow();

        ImGui::BeginDisabled(isPlottingInProgress);
        if (ImGui::Button("Plot All")) {
            plotSelectedSimulations();
        }
        ImGui::EndDisabled();
    }

    ImGui::End();
}

void SimulationVisualizer::displaySimulationSelector() {
    if (ImGui::Button("Add Simulation")) {
        showSimulationSelector = true;
        if (simulations.empty())
            simulations = simulationRepository.getAll();
    }

    if (showSimulationSelector && ImGui::Begin("Select Simulation")) {
        for (const auto &sim : simulations) {
            if (ImGui::Selectable(sim->getName().c_str()) &&
                std::ranges::find(selectedSimulations, sim) ==
                    selectedSimulations.end()) {
                selectedSimulations.push_back(sim);
                showSimulationSelector = false;
            }
        }
        ImGui::End();
    }
}

void SimulationVisualizer::manageSelectedSimulations() {
    for (auto it = selectedSimulations.begin();
         it != selectedSimulations.end();) {
        ImGui::PushID(it->get());
        if (auto sim = *it; ImGui::TreeNode(sim->getName().c_str())) {
            manageSimulationPlotFunctions(sim);
            ImGui::TreePop();
        } else if (ImGui::Button("X")) {
            it = selectedSimulations.erase(it);
            ImGui::PopID();
            continue;
        }
        ++it;
        ImGui::PopID();
    }
}

void SimulationVisualizer::manageSimulationPlotFunctions(
    const std::shared_ptr<Simulation> &sim) {
    if (ImGui::Button("Add Function")) {
        showPlotPopup = true;
    }

    std::vector<std::shared_ptr<IPlotFunction>> &funcs =
        assignedFunctions[sim->getId()];
    std::vector<std::shared_ptr<IPlotFunction>> plotFunctions;
    switch (1) {
    case 0:
        plotFunctions = plotFunctions2DNR;
        break;
    case 1:
        plotFunctions = plotFunctions2DR;
        break;
    case 2:
        plotFunctions = plotFunctions3DNR;
        break;
    case 3:
        plotFunctions = plotFunctions3DR;
        break;
    }

    if (showPlotPopup && ImGui::Begin("Select Plot Function", &showPlotPopup)) {
        ImGui::SeparatorText(
            std::format("Select Functions for {}", sim->getName()).c_str());
        ImGui::PushID((int)sim->getId());
        for (const auto &func : plotFunctions) {
            if (ImGui::Selectable(func->name().c_str()) &&
                std::ranges::find(funcs, func) == funcs.end())
                funcs.push_back(func);
        }
        ImGui::PopID();
        ImGui::End();
    }

    for (auto fit = funcs.begin(); fit != funcs.end();) {
        ImGui::Text("Function: %s", (*fit)->name().c_str());
        ImGui::SameLine();
        ImGui::PushID(fit->get());
        if (ImGui::Button("X")) {
            fit = funcs.erase(fit);
        } else {
            ++fit;
        }
        ImGui::PopID();
    }
}

void SimulationVisualizer::showPlotPopupWindow() {
    static char title[128] = "";       // NOSONAR
    static char description[512] = ""; // NOSONAR

    if (showPlotResults && ImGui::Begin("Plot Result", &showPlotResults)) {
        if (ImGui::Button("Reload")) {
            isPlotLoading = true;
            this->textureID = TextureLoader::loadTexture(
                std::format("{}/plots/tmp.png", DB_PATH).c_str());
            isPlotLoading = false;
        }

        if (isPlotLoading) {
            ImGui::Text("Loading plot...");
        } else {
            ImVec2 imageSize(560, 420);
            ImGui::Image((ImTextureID)(intptr_t)textureID, imageSize);
        }

        ImGui::InputText("Title", title, IM_ARRAYSIZE(title));

        ImGui::InputTextMultiline("Description", description,
                                  IM_ARRAYSIZE(description), ImVec2(400, 100));

        if (ImGui::Button("Save")) {
            Plot newPlot;
            newPlot.notebook_id = selectedNotebookId;
            newPlot.name = title;
            newPlot.description = description;

            newPlot.id = notebookRepository.addPlotToNotebook(newPlot);

            std::string filepath =
                std::format("{}/plots/{}/", DB_PATH, selectedNotebookId);

            // create directory if it doesn't exist
            if (!std::filesystem::exists(filepath)) {
                std::filesystem::create_directory(filepath);
            }

            std::string filename = std::format("{}/plots/{}/{}.png", DB_PATH,
                                               selectedNotebookId, newPlot.id);

            std::filesystem::copy(std::format("{}/plots/tmp.png", DB_PATH),
                                  filename);

            showPlotResults = false;
            description[0] = '\0';
            title[0] = '\0';
            plotter.clearPlots();
            assignedFunctions.clear();
            selectedSimulations.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            showPlotResults = false;
            description[0] = '\0';
            title[0] = '\0';
            plotter.clearPlots();
            assignedFunctions.clear();
            selectedSimulations.clear();
        }

        ImGui::End();
    }
}

void SimulationVisualizer::plotSelectedSimulations() {
    isPlottingInProgress = true;
    auto f = std::async(std::launch::async, [this]() {
        for (const auto &sim : selectedSimulations) {
            for (const auto &func : assignedFunctions[sim->getId()]) {
                plotter.addPlot(sim->getId(), func);
            }
        }
        plotter.createCombinedPlot(std::format("{}/plots/tmp.png", DB_PATH));

        this->textureID = TextureLoader::loadTexture(
            std::format("{}/plots/tmp.png", DB_PATH).c_str());
        this->isPlottingInProgress = false;
        this->showPlotResults = true;
    });
}

void SimulationVisualizer::setSelectedNotebook(size_t notebookId) {
    selectedNotebookId = notebookId;
}