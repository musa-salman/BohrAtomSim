#include <future>
#include <matplot/matplot.h>

#include "plotter/SimulationPlotter.hpp"

SimulationPlotter::SimulationPlotter(
    ArchivedSimulationManager &archivedSimulationManager)
    : archivedSimulationHandler(archivedSimulationManager) {}

void SimulationPlotter::addPlot(size_t simulationId,
                                std::shared_ptr<IPlotFunction> plotFunc) {
    plotRequests[simulationId].emplace_back(plotFunc);
}

void SimulationPlotter::createCombinedPlot(const std::string &outputFilePath) {
    matplot::figure(true);
    bool first = true;

    std::unordered_map<
        size_t,
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>
        cachedData;

    std::vector<std::future<std::pair<
        size_t,
        std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>>>>
        futures;

    // TODO: Uncomment this line to
    // It has error, but not this code currently not used
    // for (const auto &[simId, _] : plotRequests) {
    //     futures.push_back(std::async(std::launch::async, [this, simId]() {
    //         return std::make_pair(
    //             simId, archivedSimulationHandler.getSimulation(simId));
    //     }));
    // }

    for (auto &future : futures) {
        auto [simId, data] = future.get();
        cachedData[simId] = data;
    }

    for (const auto &[simId, transformations] : plotRequests) {
        auto it = cachedData.find(simId);
        if (it == cachedData.end() || !it->second) {
            continue; // Skip if data is missing
        }

        auto simData = it->second;

        for (const auto &transformation : transformations) {
            auto y = transformation->apply(simData);
            if (!y) {
                continue;
            }

            auto x = simData->at("t"); // Ensure 't' is a valid key

            auto h = matplot::plot(x, y.value());
            h->display_name(std::format("{}", transformation->latexName()));

            if (first) {
                matplot::hold(matplot::on);
                first = false;
            }
        }
    }
    auto lgd = matplot::legend();
    lgd->location(matplot::legend::general_alignment::bottomleft);
    lgd->box(false);

    matplot::save(outputFilePath);
}

void SimulationPlotter::clearPlots() { plotRequests.clear(); }