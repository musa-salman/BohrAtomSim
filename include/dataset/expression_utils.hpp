#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <string>
#include <vector>

#include "dataset/Dataset.hpp"

namespace dataset {

boost::dynamic_bitset<> computeMaskFromExpression(const std::string &filterExpr,
                                                  const Dataset &dataset);

std::vector<double> computeVectorFromExpression(
    const std::string &expr, const Dataset &dataset,
    const boost::container::flat_map<std::string, double> &constants = {});

} // namespace dataset
