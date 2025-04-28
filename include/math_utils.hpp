#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
polar2cartesian(
    const std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        &polar_data);

#endif // MATH_UTILS_HPP