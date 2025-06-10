#pragma once

#include "physics/math/Vector3.hpp"

namespace simulation::factories {

class StateBuilder {
  private:
    physics::math::Vector3 r0;
    physics::math::Vector3 v0;

    bool is_relativistic;
    bool is_quantized;
    bool is_3d;

    physics::math::quantum_principle principal;
    physics::math::quantum_angular angular;
    physics::math::quantum_magnetic magnetic;

    void quantize();

  public:
    void quantizeFromR0V0(const physics::math::Vector3 &r0,
                          const physics::math::Vector3 &v0);

    void setR0(const physics::math::Vector3 &r);
    void setV0(const physics::math::Vector3 &v);
    void setR0Magnitude(physics::math::scalar r0_magnitude);
    void setV0Magnitude(physics::math::scalar v0_magnitude);

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

    physics::math::quantum_principle getPrincipalQuantumNumber() const;
    physics::math::quantum_angular getAngularQuantumNumber() const;
    physics::math::quantum_magnetic getMagneticQuantumNumber() const;

    physics::math::scalar getAngularMomentum() const;
};

} // namespace simulation::factories
