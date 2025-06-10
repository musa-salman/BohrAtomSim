#include <unordered_map>

#include "physics/math/math_utils.hpp"

namespace physics::math {
std::unordered_map<std::string, std::vector<double>>
polar2cartesian(const std::vector<double> &r_data,
                const std::vector<double> &phi_data) {
    std::unordered_map<std::string, std::vector<double>> cartesian_data;
    cartesian_data.emplace("x", std::vector<double>());
    cartesian_data.emplace("y", std::vector<double>());

    cartesian_data["x"].reserve(r_data.size());
    cartesian_data["y"].reserve(r_data.size());

    for (size_t i = 0; i < r_data.size(); ++i) {
        double x = r_data[i] * std::cos(phi_data[i]);
        double y = r_data[i] * std::sin(phi_data[i]);

        cartesian_data["x"].push_back(x);
        cartesian_data["y"].push_back(y);
    }

    return cartesian_data;
}

std::unordered_map<std::string, std::vector<double>>
spherical2cartesian(const std::vector<double> &r_data,
                    const std::vector<double> &theta_data,
                    const std::vector<double> &phi_data) {
    std::unordered_map<std::string, std::vector<double>> cartesian_data;
    cartesian_data.emplace("x", std::vector<double>());
    cartesian_data.emplace("y", std::vector<double>());
    cartesian_data.emplace("z", std::vector<double>());

    cartesian_data["x"].reserve(r_data.size());
    cartesian_data["y"].reserve(r_data.size());
    cartesian_data["z"].reserve(r_data.size());

    for (size_t i = 0; i < r_data.size(); ++i) {
        double x = r_data[i] * std::sin(theta_data[i]) * std::cos(phi_data[i]);
        double y = r_data[i] * std::sin(theta_data[i]) * std::sin(phi_data[i]);
        double z = r_data[i] * std::cos(theta_data[i]);

        cartesian_data["x"].push_back(x);
        cartesian_data["y"].push_back(y);
        cartesian_data["z"].push_back(z);
    }

    return cartesian_data;
}
} // namespace physics::math
