#include "plotter/PsiDotPlotFunction.hpp"

std::string PsiDotPlotFunction::name() const { return "Psi Dot Function"; }

std::string PsiDotPlotFunction::latexName() const { return "Ψ̇'"; }

std::optional<std::vector<double>> PsiDotPlotFunction::apply(
    const std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        &data) const {
    std::vector<double> results;

    // Ensure necessary keys exist
    if (!data->contains("theta_dot") || !data->contains("theta") ||
        !data->contains("phi_dot")) {
        return results;
    }

    const std::vector<double> &theta_dots = data->at("theta_dot");
    const std::vector<double> &thetas = data->at("theta");
    const std::vector<double> &phi_dots = data->at("phi_dot");

    size_t dataSize =
        std::min(std::min(theta_dots.size(), thetas.size()), phi_dots.size());

    results.reserve(dataSize);
    for (size_t i = 0; i < dataSize; ++i) {
        results.push_back(computePsiDot(theta_dots[i], thetas[i], phi_dots[i]));
    }

    return results;
}

double PsiDotPlotFunction::computePsiDot(double theta_dot, double theta,
                                         double phi_dot) {
    return std::sqrt(theta_dot * theta_dot +
                     std::sin(theta) * std::sin(theta) * phi_dot * phi_dot);
}
