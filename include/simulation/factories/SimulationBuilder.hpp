#pragma once

#include "physics/math/Vector3.hpp"
#include "simulation/model/Simulation.hpp"
#include <unordered_map>

namespace simulation::factories {

class SimulationBuilder {
  public:
    SimulationBuilder();

    void quantizeFromR0V0(const physics::math::Vector3 &r0,
                          const physics::math::Vector3 &v0);

    void setR0(const physics::math::Vector3 &r);
    void setV0(const physics::math::Vector3 &v);
    void setR0Magnitude(physics::math::scalar initialRadiusMagnitude);
    void setV0Magnitude(physics::math::scalar initialVelocityMagnitude);

    void setRelativistic(bool is_rel);
    void setQuantized(bool is_quant);
    void set3D(bool is_3d);

    void setPrincipalQuantumNumber(physics::math::quantum_principle principal);
    void setAngularQuantumNumber(physics::math::quantum_angular angular);
    void setMagneticQuantumNumber(physics::math::quantum_magnetic magnetic);

    const physics::math::Vector3 &getR0() const;
    const physics::math::Vector3 &getV0() const;

    bool isRelativistic() const;
    bool isQuantized() const;
    bool is3D() const;

    std::string getName() const;
    void setName(const std::string &name);

    physics::math::quantum_principle getPrincipalQuantumNumber() const;
    physics::math::quantum_angular getAngularQuantumNumber() const;
    physics::math::quantum_magnetic getMagneticQuantumNumber() const;

    physics::math::scalar getDeltaTime() const;
    physics::math::scalar getTotalDuration() const;
    uint getRecordInterval() const;
    void setDeltaTime(physics::math::scalar delta_time);
    void setTotalDuration(physics::math::scalar total_duration);
    void setRecordInterval(uint record_interval);
    int getRLocalMaxCountLimit() const;
    void setRLocalMaxCountLimit(int count);
    void setNotLimitedRLocalMaxCount();
    const std::unordered_map<std::string, physics::math::scalar> &
    getConstantValues() const;
    void setPotential(const simulation::model::Potential &potential);
    const simulation::model::Potential &getPotential() const;
    void setConstantValue(const std::string &key, physics::math::scalar value);
    physics::math::scalar getConstantValue(const std::string &key) const;
    void removeConstantValue(const std::string &key);

    physics::math::scalar getAngularMomentum() const;

    simulation::model::Simulation &getSimulation();

    simulation::model::Simulation build() const;

  private:
    physics::math::quantum_principle m_principal;
    physics::math::quantum_angular m_angular;
    physics::math::quantum_magnetic m_magnetic;

    simulation::model::Simulation m_simulation;

    void _quantize();
};

} // namespace simulation::factories
