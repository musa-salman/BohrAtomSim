#ifndef UI_UTILS_HPP
#define UI_UTILS_HPP

#include <string>

#include "simulation/model/Simulation.hpp"

namespace ui {

std::string
createFormattedSimulationName(const simulation::model::Simulation &simulation);

}; // namespace ui

#endif // UI_UTILS_HPP