#include <memory>

#include <ImGuiFileDialog.h>
#include <imgui.h>
#include <implot.h>
#include <string>

#include "data_expr/Interpreter.hpp"
#include "data_expr/Parser.hpp"
#include "data_expr/Tokenizer.hpp"
#include "dataset/Dataset.hpp"

#include "exporters/CSVExporter.hpp"
#include "exporters/HDF5Exporter.hpp"

#include "data_expr/AstPrinter.hpp"
#include "math_utils.hpp"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/SimulationService.hpp"
#include "steppers/StateBuilder.hpp"
#include "view/SimulationAnalyzer.hpp"

SimulationAnalyzer::SimulationAnalyzer(
    const Simulation &simulation,
    const std::function<void(size_t)> onDeleteCallback)
    : simulation(simulation), onDeleteCallback(std::move(onDeleteCallback)),
      simulationService(
          ServiceLocator::getInstance().get<SimulationService>()) {}

void SimulationAnalyzer::render() {
    ImGui::BeginTabBar("##SimulationAnalyzerTabBar", ImGuiTabBarFlags_None);

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
        const auto &dataset =
            simulationService.getSimulationResult(simulation.getId());

        filteredDatasetView.setBaseDataset(dataset);
        bitVector = std::make_unique<BitVector>(dataset.getRowCount());
        bitVector->setAll(true);
        filteredDatasetView.setMask(*bitVector);

        auto hasPsi = dataset.getColumnsNames().end() !=
                      std::find(dataset.getColumnsNames().begin(),
                                dataset.getColumnsNames().end(), "psi");
        if (hasPsi) {
            trajectoryData =
                polar2cartesian(dataset.get("r"), dataset.get("psi"));
        } else {
            trajectoryData = spherical2cartesian(
                dataset.get("r"), dataset.get("theta"), dataset.get("phi"));
        }
        plotSelection.emplace("trajectories", false);
        isInitialized = true;
    }

    ImGui::BeginGroup();
    {
        Simulation::SimulationStatus status = simulation.status;

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Simulation Details");
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);

        ImGui::Text("ID: %zu", simulation.getId());
        ImGui::NextColumn();
        ImGui::Text("Name: %s", simulation.getName().c_str());
        ImGui::NextColumn();
        ImGui::Text("Status: %s",
                    status == Simulation::SimulationStatus::RUNNING ? "Running"
                    : status == Simulation::SimulationStatus::COMPLETED
                        ? "Completed"
                        : "Unknown");
        ImGui::NextColumn();
        // ImGui::Text("Stepper Type: %s",
        //             simulation.getStepperType() == StepperType::Stepper2D
        //                 ? "2D"
        //                 : "Unknown");
        // const auto &params = simulation.getParams();

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TextDisabled("Actions");
        if (ImGui::Button("Delete")) {
            onDeleteCallback(simulation.getId());
        }
    }
    ImGui::EndGroup();
}

void SimulationAnalyzer::renderVisualizer() {
    const auto &dataset =
        simulationService.getSimulationResult(simulation.getId());

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
                const auto &dataset =
                    simulationService.getSimulationResult(simulation.getId());
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
    const auto &datasets =
        simulationService.getSimulationResult(simulation.getId());

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
        filter[0] = '\0';
        filteredDatasetView.setMask(*bitVector);
        filterExpr.clear();
        errorMessageExpr.clear();
    }

    ImGui::SameLine();
    renderExportDatasetOptions();

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

std::string SimulationAnalyzer::formatSimulationExportName() const {
    std::string dimensions = simulation.getIs3D() ? "3d" : "2d";
    std::string model = simulation.getIsRelativistic() ? "rel" : "nr";

    std::string n, k, m;
    if (simulation.getIsQuantized()) {
        StateBuilder stateBuilder;
        stateBuilder.setRelativistic(simulation.getIsRelativistic());
        stateBuilder.setQuantized(simulation.getIsQuantized());
        stateBuilder.set3D(simulation.getIs3D());
        stateBuilder.setR0(simulation.getR0());
        stateBuilder.setV0(simulation.getV0());

        n = static_cast<int>(stateBuilder.getPrincipalQuantumNumber());
        k = static_cast<int>(stateBuilder.getAngularQuantumNumber());

        if (simulation.getIs3D()) {
            m = static_cast<int>(stateBuilder.getMagneticQuantumNumber());
        }
    }

    std::string additional = "_id" + std::to_string(simulation.getId());

    std::string filename = dimensions + "_" + model;

    if (simulation.getIsQuantized()) {
        filename += "_" + n + "_" + k;
        if (simulation.getIs3D()) {
            filename += "_" + m;
        }
    }

    filename += "_" + additional + ".csv";
    return filename;
}

void SimulationAnalyzer::renderExportDatasetOptions() {
    if (ImGui::Button("Export")) {
        IGFD::FileDialogConfig config;
        config.path = ".";

        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseFileDlgKey", "Choose File", ".h5,.csv", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {

            const std::string filePath =
                ImGuiFileDialog::Instance()->GetCurrentPath() + "/" +
                ImGuiFileDialog::Instance()->GetCurrentFileName();
            const std::string extension =
                ImGuiFileDialog::Instance()->GetCurrentFilter();

            if (extension == ".csv") {
                std::unique_ptr<Dataset> dataset =
                    filteredDatasetView.toDataset();
                auto csvExporter = std::make_unique<CSVExporter>();
                csvExporter->exportData(filePath, *dataset);
            } else if (extension == ".h5") {
                std::unique_ptr<Dataset> dataset =
                    filteredDatasetView.toDataset();
                auto hdf5Exporter = std::make_unique<HDF5Exporter>();
                hdf5Exporter->exportData(filePath, *dataset);
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid file type");
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}
