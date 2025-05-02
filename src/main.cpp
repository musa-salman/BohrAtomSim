#include <boost/asio.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <implot.h>

#include <IconsFontAwesome4.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/ISimulationRepository.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulator.hpp"
#include "view/SimulationAnalysisManager.hpp"
#include "view/SimulationExplorer.hpp"
#include "view/SimulationTableUI.hpp"

using namespace boost::interprocess;

void glfw_error_callback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    // Initialize GLFW

    if (!glfwInit())
        return -1;
    GLFWwindow *window =
        glfwCreateWindow(1920, 1080, "Atom Simulator", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(5);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = IM_GUI_INI_PATH;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Configure ImGui Style
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImPlot::CreateContext();
    const ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    // Initialize Simulation
    ServiceLocator::getInstance().registerService<ISimulator>(
        std::make_shared<Simulator>(10));

    ServiceLocator::getInstance().registerService<DataSource>(
        std::make_shared<DataSource>());

    ServiceLocator::getInstance().registerService<ISimulationRepository>(
        std::make_shared<SimulationRepository>());

    ServiceLocator::getInstance().registerService<OngoingSimulationManager>(
        std::make_shared<OngoingSimulationManager>());

    ServiceLocator::getInstance().registerService<ArchivedSimulationManager>(
        std::make_shared<ArchivedSimulationManager>());

    // Initialize UI Components
    SimulationExplorer explorer;

    SimulationTableUI simulationTableUI;

    SimulationAnalysisManager simulationAnalysisManager;

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::Begin("Atom Simulator", nullptr, window_flags);

        if (ImGui::BeginTabBar("Sidebar", ImGuiTabBarFlags_None)) {

            if (ImGui::BeginTabItem(" Simulations")) {
                explorer.render();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Archived Simulations")) {
                simulationTableUI.render();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Analysis")) {
                simulationAnalysisManager.render();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Settings")) {
                ImGui::Text("Settings");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("About")) {
                ImGui::Text("About");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        // Render GUI
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle Viewports (for multi-window support)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(window);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
