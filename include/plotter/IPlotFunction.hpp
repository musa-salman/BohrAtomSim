#ifndef IPLOTFUNCTION_HPP
#define IPLOTFUNCTION_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IPlotFunction {
  public:
    virtual ~IPlotFunction() = default;

    virtual std::string name() const = 0;
    virtual std::string latexName() const = 0;

    virtual std::optional<std::vector<double>>
    apply(const std::shared_ptr<
          std::unordered_map<std::string, std::vector<double>>> &data)
        const = 0;
};

#endif // IPLOTFUNCTION_HPP
