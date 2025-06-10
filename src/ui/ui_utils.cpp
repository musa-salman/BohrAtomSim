
#include <string>

#include "simulation/factories/SimulationBuilder.hpp"
#include "simulation/model/Simulation.hpp"
#include "ui/ui_utils.hpp"

namespace ui {
using namespace simulation::factories;
using namespace simulation::model;

std::string createFormattedSimulationName(const Simulation &simulation) {
    std::string filename;
    filename += simulation.is3D() ? "3d" : "2d";
    filename += "_";
    filename += simulation.isRelativistic() ? "rel" : "nr";

    if (simulation.isQuantized()) {
        SimulationBuilder simulationBuilder;
        simulationBuilder.set3D(simulation.is3D());
        simulationBuilder.quantizeFromR0V0(simulation.getR0(),
                                           simulation.getV0());

        filename += "_";
        filename += std::to_string(
            static_cast<int>(simulationBuilder.getPrincipalQuantumNumber()));
        filename += "_";
        filename += std::to_string(
            static_cast<int>(simulationBuilder.getAngularQuantumNumber()));

        if (simulation.is3D()) {
            filename += "_";
            filename += std::to_string(
                static_cast<int>(simulationBuilder.getMagneticQuantumNumber()));
        }
    }

    if (simulation.getId() != 0) {
        filename += "_";
        filename += "id" + std::to_string(simulation.getId());
    }

    return filename;
}
} // namespace ui