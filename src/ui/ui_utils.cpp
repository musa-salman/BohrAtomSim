
#include <string>

#include "simulator_runner/Simulation.hpp"
#include "steppers/StateBuilder.hpp"
#include "ui/ui_utils.hpp"

namespace ui {
std::string createFormattedSimulationName(const Simulation &simulation) {
    std::string filename;
    filename += simulation.getIs3D() ? "3d" : "2d";
    filename += "_";
    filename += simulation.getIsRelativistic() ? "rel" : "nr";

    if (simulation.getIsQuantized()) {
        StateBuilder stateBuilder;
        stateBuilder.set3D(simulation.getIs3D());
        stateBuilder.quantizeFromR0V0(simulation.getR0(), simulation.getV0());

        filename += "_";
        filename += std::to_string(
            static_cast<int>(stateBuilder.getPrincipalQuantumNumber()));
        filename += "_";
        filename += std::to_string(
            static_cast<int>(stateBuilder.getAngularQuantumNumber()));

        if (simulation.getIs3D()) {
            filename += "_";
            filename += std::to_string(
                static_cast<int>(stateBuilder.getMagneticQuantumNumber()));
        }
    }

    if (simulation.getId() != 0) {
        filename += "_";
        filename += "id" + std::to_string(simulation.getId());
    }

    return filename;
}
} // namespace ui