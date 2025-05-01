#include "math_utils.hpp"

std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
polar2cartesian(
    const std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        &polar_data) {
    auto cartesian_data = std::make_shared<
        std::unordered_map<std::string, std::vector<double>>>();

    cartesian_data->emplace("x", std::vector<double>());
    cartesian_data->emplace("y", std::vector<double>());

    const auto &r_data = polar_data->at("r");
    const auto &phi_data = polar_data->at("phi");

    cartesian_data->at("x").reserve(r_data.size());
    cartesian_data->at("y").reserve(r_data.size());

    for (size_t i = 0; i < r_data.size(); ++i) {
        double x = r_data[i] * std::cos(phi_data[i]);
        double y = r_data[i] * std::sin(phi_data[i]);

        cartesian_data->at("x").push_back(x);
        cartesian_data->at("y").push_back(y);
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
