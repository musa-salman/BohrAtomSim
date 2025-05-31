#include <unordered_map>

#include "math_utils.hpp"

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

double computeAngleVec2D(double u[2], double v[2]) {
    double dot_product = u[0] * v[0] + u[1] * v[1];
    double magnitude_u = std::sqrt(u[0] * u[0] + u[1] * u[1]);
    double magnitude_v = std::sqrt(v[0] * v[0] + v[1] * v[1]);

    if (magnitude_u < 1e-6 || magnitude_v < 1e-6) {
        return 0.0;
    }

    double cos_theta = dot_product / (magnitude_u * magnitude_v);
    if (cos_theta < -1.0)
        cos_theta = -1.0;
    if (cos_theta > 1.0)
        cos_theta = 1.0;

    return std::acos(cos_theta);
}
