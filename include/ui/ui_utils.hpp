#ifndef UI_UTILS_HPP
#define UI_UTILS_HPP

#include <string>

#include "simulator_runner/Simulation.hpp"

namespace ui {

std::string createFormattedSimulationName(const Simulation &simulation);

}; // namespace ui

#endif // UI_UTILS_HPP