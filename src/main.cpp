#include <boost/asio.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <implot.h>

#include <memory>

#include "explorer_manager/OngoingSimulationManager.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/PotentialRepository.hpp"
#include "simulation_repositories/PotentialRepositoryImpl.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "simulation_repositories/SimulationRepositoryImpl.hpp"
#include "simulation_repositories/SimulationServiceImpl.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulator.hpp"
#include "steppers/StepperFactory.hpp"
#include "ui/PotentialsView.hpp"
#include "ui/active_simulation/ActiveSimulationPanel.hpp"
#include "ui/analysis/SimulationAnalysisManager.hpp"

using namespace boost::interprocess;

void glfw_error_callback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    // Initialize GLFW

    if (!glfwInit())
        return -1;
    GLFWwindow *window =
        glfwCreateWindow(1600, 800, "Atom Simulator", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

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
    if (io.ConfigFlags) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImPlot::CreateContext();
    const ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    // Initialize Simulation
    {
        auto stepperFactory = std::make_shared<StepperFactory>();

        ServiceLocator::getInstance().registerService<StepperFactory>(
            stepperFactory);
    }

    ServiceLocator::getInstance().registerService<ISimulator>(
        std::make_shared<Simulator>(4));

    ServiceLocator::getInstance().registerService<DataSource>(
        std::make_shared<DataSource>());

    ServiceLocator::getInstance().registerService<PotentialRepository>(
        std::make_shared<PotentialRepositoryImpl>());

    ServiceLocator::getInstance().registerService<SimulationRepository>(
        std::make_shared<SimulationRepositoryImpl>());

    ServiceLocator::getInstance().registerService<OngoingSimulationManager>(
        std::make_shared<OngoingSimulationManager>());

    ServiceLocator::getInstance().registerService<ArchivedSimulationManager>(
        std::make_shared<ArchivedSimulationManager>());

    ServiceLocator::getInstance().registerService<SimulationService>(
        std::make_shared<SimulationServiceImpl>());

    // Initialize UI Components
    ui::active_simulation::ActiveSimulationPanel explorer;

    ui::analysis::SimulationAnalysisManager simulationAnalysisManager;

    ui::PotentialsView potentialsView;

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

            if (ImGui::BeginTabItem("Analysis")) {
                simulationAnalysisManager.render();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Potentials")) {
                potentialsView.render();
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
        if (io.ConfigFlags) {
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
