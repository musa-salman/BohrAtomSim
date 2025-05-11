#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::vector<double>>
polar2cartesian(const std::vector<double> &r_data,
                const std::vector<double> &phi_data);

double computeAngleVec2D(double u[2], double v[2]);

#endif // MATH_UTILS_HPP