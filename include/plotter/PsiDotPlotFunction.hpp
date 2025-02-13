#ifndef PSI_DOT_PLOT_FUNCTION_HPP
#define PSI_DOT_PLOT_FUNCTION_HPP

#include "IPlotFunction.hpp"
#include <cmath>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

class PsiDotPlotFunction : public IPlotFunction {
  public:
    std::string name() const override;
    std::string latexName() const override;

    std::optional<std::vector<double>>
    apply(const std::shared_ptr<
          std::unordered_map<std::string, std::vector<double>>> &data)
        const override;

    static double computePsiDot(double theta_dot, double theta, double phi_dot);
};

#endif // PSI_DOT_PLOT_FUNCTION_HPP
