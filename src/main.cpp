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
#include <memory>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "hdf5/SimulationHDF5.hpp"
#include "hdf5/SimulationManager.hpp"
#include "simulator_runner/Simulator.hpp"
#include "spherical/spherical_sim.hpp"
#include "utils/iterator.h"
#include "view/AddSimulationDialog.hpp"
#include "view/Component.hpp"
#include "view/Simulation.hpp"
#include "view/SimulationCard.hpp"
#include "view/SimulationExplorer.hpp"

using namespace boost::interprocess;

void record(void *record_in, const char *name, const sim_itr *iter) {
    auto record = static_cast<SimulationManager *>(record_in);
    record->appendData(name, {{"t", iter->dt},
                              {"r", iter->r},
                              {"r_dot", iter->r_dot},
                              {"r_dot_dot", iter->r_dot_dot},
                              {"phi", iter->phi},
                              {"phi_dot", iter->phi_dot},
                              {"theta", iter->theta},
                              {"theta_dot", iter->theta_dot},
                              {"theta_dot_dot", iter->theta_dot_dot}});
}

void glfw_error_callback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void loadFonts(ImGuiIO &io) {
    // Path to your Font Awesome font file
    static const std::array<ImWchar, 3> icons_ranges = {ICON_MIN_FA,
                                                        ICON_MAX_FA, 0};

    ImFontConfig icons_config;
    icons_config.MergeMode = true; // Merge into default font
    icons_config.PixelSnapH = true;

    // Load default font
    io.Fonts->AddFontDefault();

    // Load Font Awesome font
    io.Fonts->AddFontFromFileTTF("assets/fonts/fontawesome-webfont.ttf", 16.0f,
                                 &icons_config, icons_ranges.data());
}

int main() {
    // Initialize GLFW
    if (!glfwInit())
        return -1;
    GLFWwindow *window =
        glfwCreateWindow(800, 600, "Atom Simulator", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports
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

    // loadFonts(io);

    // Initialize Simulation
    SimulationDataWriter db("simulation.h5");
    SimulationManager manager(db);
    Simulator simulator(2);

    // Initialize Components
    std::vector<Component *> components;

    SimulationCard card([&simulator, &manager](const Simulation *simulation) {
        record_handler rh = {.name = simulation->name,
                             .record_in = &manager,
                             .record = record,
                             .record_interval = simulation->record_interval,
                             .delta_psi_mode = simulation->delta_psi_mode};

        auto ctx = std::make_shared<sim_ctx>();

        *ctx = {
            .record_handler = rh,
            .revolutions = simulation->revolutions,
            .time_interval = simulation->time_interval,
        };

        simulator.simulateOrbit(ctx, simulation->orbit,
                                simulate_spherical_orbit);
    });

    components.push_back(&card);

    SimulationExplorer explorer(manager, [&card](Simulation &simulation) {
        card.setSimulation(simulation);
    });

    components.push_back(&explorer);

    AddSimulationDialog add_simulation_interface(
        [&manager](const Simulation &simulation) {
            manager.addSimulation(simulation);
        });

    components.push_back(&add_simulation_interface);

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto component : components) {
            component->draw();
        }

        // Render GUI
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle Viewports
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