#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <string>

#include "eom/utils.hpp"
#include "steppers/Potential.hpp"

class Simulation {
    std::string name;
    size_t id;

    eom::Vector3 r0;
    eom::Vector3 v0;

    scalar delta_time;
    scalar total_duration;
    uint record_interval;

    Potential potential;

    std::unordered_map<std::string, scalar> constantValues;

    bool isRelativistic = false;
    bool isQuantized = true;
    bool is3D = true;

  public:
    enum class SimulationStatus { IDLE, RUNNING, PAUSED, COMPLETED };

    SimulationStatus status = SimulationStatus::IDLE;

    Simulation() = default;

    Simulation(std::string name, size_t id, bool isRelativistic = true,
               bool isQuantized = false, bool is3D = false,
               eom::Vector3 r0 = {0, 0, 0}, eom::Vector3 v0 = {0, 0, 0},
               Potential potential = Potential(), scalar delta_time = 1e-7,
               uint record_interval = 10000, scalar total_duration = 800)
        : name(std::move(name)), id(id), r0(r0), v0(v0), delta_time(delta_time),
          total_duration(total_duration), record_interval(record_interval),
          potential(std::move(potential)), isRelativistic(isRelativistic),
          isQuantized(isQuantized), is3D(is3D), status(SimulationStatus::IDLE) {
    }

    Simulation(const Simulation &simulation)
        : name(simulation.name), id(simulation.id) {
        r0 = simulation.r0;
        v0 = simulation.v0;
        potential = simulation.potential;
        status = simulation.status;
        total_duration = simulation.total_duration;
        delta_time = simulation.delta_time;
        record_interval = simulation.record_interval;
        constantValues = simulation.constantValues;
        isRelativistic = simulation.isRelativistic;
        isQuantized = simulation.isQuantized;
        is3D = simulation.is3D;
    }

    Simulation &operator=(const Simulation &simulation) {
        if (this != &simulation) {
            name = simulation.name;
            id = simulation.id;
            potential = simulation.potential;
            status = simulation.status;
            r0 = simulation.r0;
            v0 = simulation.v0;
            delta_time = simulation.delta_time;
            total_duration = simulation.total_duration;
            record_interval = simulation.record_interval;
            constantValues = simulation.constantValues;
            isRelativistic = simulation.isRelativistic;
            isQuantized = simulation.isQuantized;
            is3D = simulation.is3D;
        }
        return *this;
    }

    Simulation(Simulation &&simulation) noexcept
        : name(std::move(simulation.name)), id(simulation.id),
          r0(std::move(simulation.r0)), v0(std::move(simulation.v0)),
          delta_time(simulation.delta_time),
          total_duration(simulation.total_duration),
          record_interval(simulation.record_interval),
          potential(std::move(simulation.potential)),
          constantValues(std::move(simulation.constantValues)),
          isRelativistic(simulation.isRelativistic),
          isQuantized(simulation.isQuantized), is3D(simulation.is3D),
          status(simulation.status) {}

    const std::unordered_map<std::string, scalar> &getConstantValues() const {
        return constantValues;
    }

    Simulation &operator=(Simulation &&simulation) noexcept {
        if (this != &simulation) {
            name = std::move(simulation.name);
            id = simulation.id;
            r0 = std::move(simulation.r0);
            v0 = std::move(simulation.v0);
            potential = std::move(simulation.potential);
            status = simulation.status;
            total_duration = simulation.total_duration;
            delta_time = simulation.delta_time;
            record_interval = simulation.record_interval;
            constantValues = std::move(simulation.constantValues);
            simulation.id = id;
            is3D = simulation.is3D;
            isRelativistic = simulation.isRelativistic;
            isQuantized = simulation.isQuantized;
        }
        return *this;
    }

    void
    setConstantValues(const std::unordered_map<std::string, scalar> &values) {
        constantValues = values;
    }

    void setConstantValues(const std::string &jsonString) {
        nlohmann::json constantsJson = nlohmann::json::parse(jsonString);
        for (const auto &[key, value] : constantsJson.items()) {
            constantValues[key] = value.get<scalar>();
        }
    }

    void clearConstantValues() { constantValues.clear(); }

    bool hasConstantValue(const std::string &key) const {
        return constantValues.find(key) != constantValues.end();
    }

    void removeConstantValue(const std::string &key) {
        constantValues.erase(key);
    }

    void setConstantValue(const std::string &key, scalar value) {
        constantValues[key] = value;
    }

    scalar getConstantValue(const std::string &key) const {
        auto it = constantValues.find(key);
        if (it != constantValues.end()) {
            return it->second;
        }
        return -9999e99;
    }

    const eom::Vector3 &getR0() const { return r0; }

    const eom::Vector3 &getV0() const { return v0; }

    void setR0(const eom::Vector3 &r0) { this->r0 = r0; }

    void setV0(const eom::Vector3 &v0) { this->v0 = v0; }

    SimulationStatus getStatus() const { return status; }

    void setName(const std::string &name) { this->name = name; }

    void setId(size_t id) { this->id = id; }

    void setStatus(SimulationStatus status) { this->status = status; }

    std::string getName() const { return name; }

    size_t getId() const { return id; }

    const Potential &getPotential() const { return potential; }

    void setPotential(const Potential &potential) {
        this->potential = potential;
        this->clearConstantValues();
        this->setConstantValues(potential.getConstants());
    }

    std::string serializeConstants() const {
        nlohmann::json constantsJson;
        for (const auto &[key, value] : constantValues) {
            constantsJson[key] = value;
        }
        return constantsJson.dump();
    }

    scalar getDeltaTime() const { return delta_time; }

    scalar getTotalDuration() const { return total_duration; }

    uint getRecordInterval() const { return record_interval; }

    void setDeltaTime(scalar delta_time) { this->delta_time = delta_time; }

    void setTotalDuration(scalar total_duration) {
        this->total_duration = total_duration;
    }

    void setRecordInterval(uint record_interval) {
        this->record_interval = record_interval;
    }

    bool getIsRelativistic() const { return isRelativistic; }

    bool getIsQuantized() const { return isQuantized; }

    bool getIs3D() const { return is3D; }

    void setRelativistic(bool is_rel) { isRelativistic = is_rel; }

    void setQuantized(bool is_quant) { isQuantized = is_quant; }

    void set3D(bool is_3d) { is3D = is_3d; }
};

#endif // SIMULATION_HPP