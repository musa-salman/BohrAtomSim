#include "data_expr/Interpreter.hpp"
#include "data_expr/Parser.hpp"
#include "data_expr/Tokenizer.hpp"
#include "dataset/Dataset.hpp"
#include "imgui.h"
#include "implot.h"

#include "data_expr/AstPrinter.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/ArchivedSimulationManager.hpp"
#include "view/SimulationAnalyzer.hpp"
#include <memory>

SimulationAnalyzer::SimulationAnalyzer(
    const std::shared_ptr<Simulation> simulation)
    : simulation(simulation) {}

void SimulationAnalyzer::render() {
    ImGui::BeginTabBar("SimulationAnalyzerTabBar", ImGuiTabBarFlags_None);
    if (ImGui::BeginTabItem("Details")) {
        renderSimulationDetails();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Visualizer")) {
        renderVisualizer();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Plotter")) {
        renderPlotter();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Datasets Viewer")) {
        renderDatasetsViewer();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}

void SimulationAnalyzer::renderSimulationDetails() {
    if (!isInitialized) {
        const auto &dataset = ServiceLocator::getInstance()
                                  .get<ArchivedSimulationManager>()
                                  ->getSimulation(simulation->getId());

        filteredDatasetView.setBaseDataset(dataset);
        bitVector = std::make_unique<BitVector>(dataset.getRowCount());
        bitVector->setAll(true);
        filteredDatasetView.setMask(*bitVector);

        trajectoryData = polar2cartesian(dataset.get("r"), dataset.get("phi"));
        plotSelection.emplace("trajectories", false);
        isInitialized = true;
    }

    ImGui::BeginGroup();
    {
        Simulation::SimulationStatus status = simulation->status;

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Simulation Details");
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);

        ImGui::BeginGroup();
        ImGui::TextDisabled("General Info");
        if (ImGui::BeginTable("GeneralInfoTable", 2,
                              ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("ID");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%zu", simulation->getId());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", simulation->getName().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Status");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", status == Simulation::SimulationStatus::COMPLETED
                                  ? "Completed"
                              : status == Simulation::SimulationStatus::RUNNING
                                  ? "Running"
                              : status == Simulation::SimulationStatus::PAUSED
                                  ? "Paused"
                                  : "Unknown");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Time Step");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.5e", simulation->getDeltaTime());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Duration");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f", simulation->getTotalDuration());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Record Interval");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%hu", simulation->getRecordInterval());

            ImGui::EndTable();
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        ImGui::BeginGroup();
        ImGui::TextDisabled("Initial Conditions");
        if (ImGui::BeginTable("InitialConditionsTable", 2,
                              ImGuiTableFlags_SizingStretchSame)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Position (x, y)");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.2e, %.2e)", simulation->getR0X(),
                        simulation->getR0Y());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Velocity (x, y)");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("(%.2e, %.2e)", simulation->getV0X(),
                        simulation->getV0Y());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Angle θ");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getThetaRV());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Radius");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getR0());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Velocity Mag.");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getV0());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Angular Momentum");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2e", simulation->getV0() * simulation->getR0() *
                                    sin(simulation->getThetaRV()));

            ImGui::EndTable();
        }
        ImGui::EndGroup();

        ImGui::Columns(1);

        // Buttons
        ImGui::Separator();
        ImGui::TextDisabled("Actions");
        ImGui::BeginGroup();
        if (ImGui::Button("Delete")) {
            // TODO: Delete logic here
        }
        ImGui::SameLine();
        if (ImGui::Button("Export")) {
            // TODO: Export logic here
        }
        ImGui::EndGroup();
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderVisualizer() {
    const auto &dataset = ServiceLocator::getInstance()
                              .get<ArchivedSimulationManager>()
                              ->getSimulation(simulation->getId());

    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Trajectories");
        ImGui::Separator();

        ImGui::InputText("Filter", filter, sizeof(filter));
        ImGui::SameLine();
        if (ImGui::Button("Apply")) {
            if (std::string(filter).empty()) {
                filteredDatasetView.setBaseDataset(dataset);
                bitVector = std::make_unique<BitVector>(dataset.getRowCount());
                bitVector->setAll(true);
                filteredDatasetView.setMask(*bitVector);
                errorMessageExpr.clear();
            } else {
                try {
                    Interpreter interpreter(dataset);

                    Tokenizer tokenizer(filter);
                    Parser parser(tokenizer.scanTokens());
                    AstPrinter astPrinter;
                    std::shared_ptr<Expr> expr = parser.parseExpression();
                    filterExpr = astPrinter.print(expr);
                    auto result = interpreter.evaluate(expr);

                    if (std::holds_alternative<std::unique_ptr<BitVector>>(
                            result)) {
                        bitVector = std::move(
                            std::get<std::unique_ptr<BitVector>>(result));
                        filteredDatasetView.setBaseDataset(dataset);
                        filteredDatasetView.setMask(*bitVector);
                        errorMessageExpr.clear();
                    } else {
                        errorMessageExpr = "Invalid filter expression";
                    }
                } catch (const std::exception &e) {
                    errorMessageExpr = e.what();
                }
            }
        }
        if (!errorMessageExpr.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s",
                               errorMessageExpr.c_str());
        } else
            ImGui::Text("Parsed Expression: %s", filterExpr.c_str());

        ImGui::Text("Select Datasets to Plot:");
        ImGui::PushID("plot_selection");
        for (const auto &name : dataset.getColumnsNames()) {
            if (name == "t")
                continue;

            if (!plotSelection.contains(name)) {
                plotSelection[name] = false;
            }

            ImGui::SameLine();
            ImGui::Checkbox(name.c_str(), &plotSelection[name]);
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show Trajectories", &plotSelection["trajectories"]);
        ImGui::PopID();

        const auto &t_data = filteredDatasetView.get("t");

        float minT;
        float maxT;
        // static float currentT = 800;
        // static uint32_t currentIndex = 0;
        if (t_data.size() == 0) {
            minT = 0;
            maxT = 800;
        } else {
            minT = t_data.front();
            maxT = t_data.back();
        }

        // if (ImGui::DragFloat("Time Range (Is it needed?)", &currentT, 1.0f,
        //                      minT, maxT, "%.7e",
        //                      ImGuiSliderFlags_AlwaysClamp)) {
        //     auto it = std::lower_bound(t_data.begin(), t_data.end(),
        //     currentT); if (it != t_data.end())
        //         currentIndex = std::distance(t_data.begin(), it);
        //     else
        //         currentIndex = t_data.size() - 1;
        // }

        // ImGui::Text(
        //     "At Time %.3e: position = (%.4e, %.4e), radius = %.5e, r_dot = "
        //     "%7e, r_ddot = %.7e, phi = %.7e, phi_dot = %.7e, gamma = %.5e",
        //     currentT, trajectoryData.at("x")[currentIndex],
        //     trajectoryData.at("y")[currentIndex],
        //     dataset.get("r")[currentIndex],
        //     dataset.get("r_dot")[currentIndex],
        //     dataset.get("r_ddot")[currentIndex],
        //     dataset.get("phi")[currentIndex],
        //     dataset.get("phi_dot")[currentIndex],
        //     dataset.get("gamma")[currentIndex]);

        if (plotSelection["trajectories"]) {
            if (ImPlot::BeginPlot("Trajectories")) {
                ImPlot::SetupAxes("X", "Y");

                ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

                ImPlot::PlotLine("Trajectory", trajectoryData.at("x").data(),
                                 trajectoryData.at("y").data(),
                                 trajectoryData.at("x").size());
                ImPlot::EndPlot();
            }
        }

        for (const auto &name : dataset.getColumnsNames()) {
            if (name == "t" || !plotSelection[name])
                continue;

            if (ImPlot::BeginPlot(name.c_str())) {
                ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit,
                                  ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxisLimits(ImAxis_X1, minT, maxT, ImGuiCond_Once);
                ImPlot::PlotScatter(name.c_str(), t_data.data(),
                                    filteredDatasetView.get(name).data(),
                                    filteredDatasetView.get(name).size());

                ImPlot::EndPlot();
            }
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderPlotter() {
    ImGui::BeginGroup();
    {

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Plotter");
        ImGui::Separator();

        ImGui::InputText("X Expression", xExpr, sizeof(xExpr));

        ImGui::InputText("Y Expression", yExpr, sizeof(yExpr));
        if (ImGui::Button("Apply")) {
            try {
                const auto &dataset = ServiceLocator::getInstance()
                                          .get<ArchivedSimulationManager>()
                                          ->getSimulation(simulation->getId());
                // X expression
                {
                    Interpreter interpreter(dataset);

                    Tokenizer tokenizer(xExpr);
                    Parser parser(tokenizer.scanTokens());
                    AstPrinter astPrinter;
                    std::shared_ptr<Expr> expr = parser.parseExpression();
                    xExprStr = astPrinter.print(expr);
                    auto result = interpreter.evaluate(expr);

                    if (std::holds_alternative<
                            std::unique_ptr<std::vector<double>>>(result)) {
                        xData = std::move(
                            std::get<std::unique_ptr<std::vector<double>>>(
                                result));
                        errorMessagePlot.clear();
                    } else {
                        errorMessagePlot = "Invalid X expression";
                    }
                }

                // Y expression
                if (errorMessagePlot.empty()) {
                    Interpreter interpreter(dataset);

                    Tokenizer tokenizer(yExpr);
                    Parser parser(tokenizer.scanTokens());
                    AstPrinter astPrinter;
                    std::shared_ptr<Expr> expr = parser.parseExpression();
                    yExprStr = astPrinter.print(expr);
                    auto result = interpreter.evaluate(expr);

                    if (std::holds_alternative<
                            std::unique_ptr<std::vector<double>>>(result)) {
                        yData = std::move(
                            std::get<std::unique_ptr<std::vector<double>>>(
                                result));
                        errorMessagePlot.clear();
                    } else {
                        errorMessagePlot = "Invalid Y expression";
                    }
                }
            } catch (const std::exception &e) {
                errorMessagePlot = e.what();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            xData.reset();
            yData.reset();
            xExprStr.clear();
            yExprStr.clear();
            yExpr[0] = '\0';
            xExpr[0] = '\0';
            errorMessagePlot.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            // TODO: Save logic here
        }
        ImGui::SameLine();
        if (ImGui::Button("Export")) {
            // TODO: Export logic here
        }

        if (ImPlot::BeginPlot("Plotter")) {
            ImPlot::SetupAxes("X", "Y");

            ImPlot::SetupAxisLimits(ImAxis_X1, -20, 20, ImGuiCond_Once);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -20, 20, ImGuiCond_Once);

            if (errorMessagePlot.empty()) {
                if (xData && yData) {
                    ImPlot::PlotLine("Plot", xData->data(), yData->data(),
                                     static_cast<int>(xData->size()));
                }
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s",
                                   errorMessagePlot.c_str());
            }
            ImPlot::EndPlot();
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderDatasetsViewer() {
    const auto &datasets = ServiceLocator::getInstance()
                               .get<ArchivedSimulationManager>()
                               ->getSimulation(simulation->getId());

    ImGui::BeginGroup();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Datasets Viewer");
    ImGui::Separator();
    const size_t colCount = datasets.getColumnCount();

    ImGui::InputText("Filter", filter, sizeof(filter));
    ImGui::SameLine();
    if (ImGui::Button("Apply")) {
        if (std::string(filter).empty()) {
            filteredDatasetView.setBaseDataset(datasets);
            bitVector = std::make_unique<BitVector>(datasets.getRowCount());
            bitVector->setAll(true);
            filteredDatasetView.setMask(*bitVector);
            errorMessageExpr.clear();
        } else {
            try {
                Interpreter interpreter(datasets);

                Tokenizer tokenizer(filter);
                Parser parser(tokenizer.scanTokens());
                AstPrinter astPrinter;
                std::shared_ptr<Expr> expr = parser.parseExpression();
                filterExpr = astPrinter.print(expr);
                auto result = interpreter.evaluate(expr);

                if (std::holds_alternative<std::unique_ptr<BitVector>>(
                        result)) {
                    bitVector =
                        std::move(std::get<std::unique_ptr<BitVector>>(result));
                    filteredDatasetView.setBaseDataset(datasets);
                    filteredDatasetView.setMask(*bitVector);
                    errorMessageExpr.clear();
                } else {
                    errorMessageExpr = "Invalid filter expression";
                }
            } catch (const std::exception &e) {
                errorMessageExpr = e.what();
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        filteredDatasetView.setBaseDataset(datasets);
        bitVector = std::make_unique<BitVector>(datasets.getRowCount());
        bitVector->setAll(true);
        filteredDatasetView.setMask(*bitVector);
        filterExpr.clear();
        errorMessageExpr.clear();
    }

    ImGui::SameLine();
    if (ImGui::Button("Export")) {
        // TODO: Export logic here
    }

    if (!errorMessageExpr.empty()) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s",
                           errorMessageExpr.c_str());
    } else
        ImGui::Text("Parsed Expression: %s", filterExpr.c_str());
    if (ImGui::BeginTable("HeaderTable", colCount, ImGuiTableFlags_Borders)) {
        for (const auto &key : datasets.getColumnsNames())
            ImGui::TableSetupColumn(key.c_str());

        ImGui::TableHeadersRow();
        ImGui::EndTable();
    }

    if (ImGui::BeginChild("ScrollableData", ImVec2(0, 400), false)) {
        if (ImGui::BeginTable("DatasetsTable", colCount,
                              ImGuiTableFlags_Borders |
                                  ImGuiTableFlags_RowBg)) {
            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(filteredDatasetView.getRowCount()));
            while (clipper.Step()) {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd;
                     ++row) {
                    ImGui::TableNextRow();

                    int col = 0;
                    for (const auto &key : datasets.getColumnsNames()) {
                        const auto &vec = filteredDatasetView.get(key);
                        ImGui::TableSetColumnIndex(col++);
                        if (row < static_cast<int>(vec.size()))
                            ImGui::Text("%.10e", vec[row]);
                        else
                            ImGui::Text("NaN");
                    }
                }
            }

            ImGui::EndTable();
        }
    } else {
        ImGui::Text("No dataset loaded.");
    }
    ImGui::EndChild();

    ImGui::EndGroup();
}
