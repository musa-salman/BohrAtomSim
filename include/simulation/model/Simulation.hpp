#pragma once

#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "Potential.hpp"
#include "physics/math/Vector3.hpp"

namespace simulation::model {

class Simulation {
  public:
    enum class SimulationStatus { READY, RUNNING, PAUSED, COMPLETED, QUEUED };

    SimulationStatus status;

    Simulation();
    Simulation(std::string name, size_t id, bool isRelativistic = true,
               bool isQuantized = false, bool is3D = false,
               physics::math::Vector3 r0 = {0, 0, 0},
               physics::math::Vector3 v0 = {0, 0, 0},
               Potential potential = Potential(),
               physics::math::scalar delta_time = 1e-6,
               uint record_interval = 1000,
               physics::math::scalar total_duration = 800,
               int rLocalMaxCount = 10);

    Simulation(const Simulation &other);
    Simulation &operator=(const Simulation &other);
    Simulation(Simulation &&other) noexcept;
    Simulation &operator=(Simulation &&other) noexcept;

    const std::unordered_map<std::string, physics::math::scalar> &
    getConstantValues() const;
    void setConstantValues(
        const std::unordered_map<std::string, physics::math::scalar> &values);
    void setConstantValues(const std::string &jsonString);
    void clearConstantValues();
    bool hasConstantValue(const std::string &key) const;
    void removeConstantValue(const std::string &key);
    void setConstantValue(const std::string &key, physics::math::scalar value);
    physics::math::scalar getConstantValue(const std::string &key) const;

    const physics::math::Vector3 &getR0() const;
    const physics::math::Vector3 &getV0() const;
    void setR0(const physics::math::Vector3 &r0);
    void setV0(const physics::math::Vector3 &v0);

    SimulationStatus getStatus() const;
    void setName(const std::string &name);
    void setId(size_t id);
    void setStatus(SimulationStatus status);
    std::string getName() const;
    size_t getId() const;

    const Potential &getPotential() const;
    void setPotential(const Potential &potential);
    std::string serializeConstants() const;

    physics::math::scalar getDeltaTime() const;
    physics::math::scalar getTotalDuration() const;
    uint getRecordInterval() const;
    void setDeltaTime(physics::math::scalar delta_time);
    void setTotalDuration(physics::math::scalar total_duration);
    void setRecordInterval(uint record_interval);

    int getRLocalMaxCountLimit() const;
    void setRLocalMaxCountLimit(int count);

    bool isRelativistic() const;
    bool isQuantized() const;
    bool is3D() const;
    void setRelativistic(bool is_rel);
    void setQuantized(bool is_quant);
    void set3D(bool is_3d);

  private:
    std::string m_name;
    size_t m_id;

    physics::math::Vector3 m_r0;
    physics::math::Vector3 m_v0;

    physics::math::scalar m_deltaTime;
    physics::math::scalar m_totalDuration;
    int m_rLocalMaxCountLimit;
    uint m_recordInterval;

    Potential m_potential;

    std::unordered_map<std::string, physics::math::scalar> m_constantValues;

    bool m_isRelativistic;
    bool m_isQuantized;
    bool m_is3D;
};

} // namespace simulation::model
