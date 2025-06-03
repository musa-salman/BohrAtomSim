#ifndef STATE_BUILDER_HPP
#define STATE_BUILDER_HPP

#include <expected>

#include "eom/utils.hpp"

class StateBuilder {
  private:
    eom::Vector3 r0;
    eom::Vector3 v0;

    bool is_relativistic;
    bool is_quantized;
    bool is_3d;

    quantum_principle principal;
    quantum_angular angular;
    quantum_magnetic magnetic;

    void quantize();

  public:
    void quantizeFromR0V0(const eom::Vector3 &r0, const eom::Vector3 &v0);

    void setR0(const eom::Vector3 &r);
    void setV0(const eom::Vector3 &v);
    void setR0Magnitude(scalar r0_magnitude);
    void setV0Magnitude(scalar v0_magnitude);

    void setRelativistic(bool is_rel);
    void setQuantized(bool is_quant);
    void set3D(bool is_3d);

    void setPrincipalQuantumNumber(quantum_principle principal);
    void setAngularQuantumNumber(quantum_angular angular);
    void setMagneticQuantumNumber(quantum_magnetic magnetic);

    const eom::Vector3 &getR0() const;
    const eom::Vector3 &getV0() const;

    bool isRelativistic() const;
    bool isQuantized() const;
    bool is3D() const;

    quantum_principle getPrincipalQuantumNumber() const;
    quantum_angular getAngularQuantumNumber() const;
    quantum_magnetic getMagneticQuantumNumber() const;

    scalar getAngularMomentum() const;
};

#endif // STATE_BUILDER_HPP