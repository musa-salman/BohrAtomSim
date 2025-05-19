#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "atom/atom_bohr_sim.h"
#include "orbital_math.h"
#include "steppers/SimulationStepper.hpp"
#include "utils/iterator.h"

class Simulation {
    std::string name;
    size_t id;
    StepperType stepperType;

    std::unordered_map<std::string, ParameterValue> params;

  public:
    enum class SimulationStatus { IDLE, RUNNING, PAUSED, COMPLETED };

    SimulationStatus status = SimulationStatus::IDLE;

    Simulation() = default;

    Simulation(const std::string &name, size_t id,
               const std::unordered_map<std::string, ParameterValue> &params)
        : name(name), id(id), params(params) {}

    Simulation(const Simulation &simulation)
        : name(simulation.name), id(simulation.id) {
        stepperType = simulation.stepperType;
        params = simulation.params;
        status = simulation.status;
    }

    Simulation &operator=(const Simulation &simulation) {
        if (this != &simulation) {
            name = simulation.name;
            id = simulation.id;
            status = simulation.status;
            stepperType = simulation.stepperType;
            params = simulation.params;
        }
        return *this;
    }

    Simulation(Simulation &&simulation) noexcept
        : name(std::move(simulation.name)), id(simulation.id),
          stepperType(simulation.stepperType),
          params(std::move(simulation.params)), status(simulation.status) {}

    template <typename T> void setParam(const std::string &key, T &&value) {
        params[key] = std::forward<T>(value);
    }

    template <typename T> T &getParam(const std::string &key) {
        return std::any_cast<T &>(params.at(key));
    }

    bool hasParam(const std::string &key) const {
        return params.find(key) != params.end();
    }

    std::unordered_map<std::string, ParameterValue> &getParams() {
        return params;
    }

    void setName(const std::string &name) { this->name = name; }

    void setId(size_t id) { this->id = id; }

    void setStatus(SimulationStatus status) { this->status = status; }

    void setStepperType(StepperType stepperType) {
        this->stepperType = stepperType;
    }

    void setParams(const std::unordered_map<std::string, ParameterValue> &p) {
        params = p;
    }

    void setParams(nlohmann::json &j) {
        for (auto &[key, value] : j.items()) {
            if (value.is_number_float()) {
                params[key] = value.get<double>();
            } else if (value.is_number_integer()) {
                params[key] = value.get<int>();
            } else if (value.is_string()) {
                params[key] = value.get<std::string>();
            } else if (value.is_boolean()) {
                params[key] = value.get<bool>();
            }
        }
    }

    std::string getName() const { return name; }

    size_t getId() const { return id; }

    StepperType getStepperType() const { return stepperType; }

    std::string serializeParams() const {
        // Serialize the parameters to a string json use nlohmann::json
        nlohmann::json j;
        for (const auto &[key, value] : params) {
            if (value.type() == typeid(double)) {
                j[key] = std::any_cast<double>(value);
            } else if (value.type() == typeid(int)) {
                j[key] = std::any_cast<int>(value);
            } else if (value.type() == typeid(std::string)) {
                j[key] = std::any_cast<std::string>(value);
            } else if (value.type() == typeid(bool)) {
                j[key] = std::any_cast<bool>(value);
            }
        }
        return j.dump();
    }
};

#endif // SIMULATION_HPP