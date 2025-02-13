#ifndef COLUMN_EXTRACTION_FUNCTION_HPP
#define COLUMN_EXTRACTION_FUNCTION_HPP

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "IPlotFunction.hpp"

class ColumnExtractionFunction : public IPlotFunction {
  public:
    explicit ColumnExtractionFunction(const std::string &columnName);

    std::string name() const override;

    std::string latexName() const override;

    std::optional<std::vector<double>>
    apply(const std::shared_ptr<
          std::unordered_map<std::string, std::vector<double>>> &data)
        const override;

  private:
    std::string columnName;
};

#endif // COLUMN_EXTRACTION_FUNCTION_HPP
