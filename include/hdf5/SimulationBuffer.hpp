#ifndef SIMULATION_BUFFER_HPP
#define SIMULATION_BUFFER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "utils/types.h"

class SimulationBuffer {
  private:
    std::unordered_map<std::string, std::vector<scalar>> field_data;

  public:
    SimulationBuffer() = delete;

    explicit SimulationBuffer(const std::vector<std::string> &fields);

    void appendData(const std::unordered_map<std::string, scalar> &data);

    std::vector<scalar> getFieldData(const std::string &field) const;

    std::unordered_map<std::string, std::vector<scalar>> getAllData() const;

    void clearData();
};

#endif // SIMULATION_BUFFER_HPP
