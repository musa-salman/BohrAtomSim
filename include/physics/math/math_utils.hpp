#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

namespace physics::math {

std::unordered_map<std::string, std::vector<double>>
polar2cartesian(const std::vector<double> &r_data,
                const std::vector<double> &phi_data);

std::unordered_map<std::string, std::vector<double>>
spherical2cartesian(const std::vector<double> &r_data,
                    const std::vector<double> &theta_data,
                    const std::vector<double> &phi_data);
} // namespace physics::math

#endif // MATH_UTILS_HPP