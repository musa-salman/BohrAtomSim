#include "simulation/factories/StateBuilder.hpp"
#include "physics/math/QuantumUtils.hpp"
#include "physics/math/Vector3.hpp"
#include "physics/math/VectorOps.hpp"

namespace simulation::factories {

using namespace physics::math;

void StateBuilder::setR0(const Vector3 &r) { r0 = r; }

void StateBuilder::setV0(const Vector3 &v) { v0 = v; }

const Vector3 &StateBuilder::getR0() const { return r0; }

const Vector3 &StateBuilder::getV0() const { return v0; }

void StateBuilder::setR0Magnitude(scalar r0_magnitude) {
    Vector3 r0_unit = normalized(r0);
    r0 = r0_unit * r0_magnitude;
}

void StateBuilder::setV0Magnitude(scalar v0_magnitude) {
    Vector3 v0_unit = normalized(v0);
    v0 = v0_unit * v0_magnitude;
}

void StateBuilder::setRelativistic(bool is_rel) { is_relativistic = is_rel; }

void StateBuilder::setQuantized(bool is_quant) {
    is_quantized = is_quant;

    if (is_quantized)
        quantize();
}

void StateBuilder::set3D(bool is_3d) {
    this->is_3d = is_3d;
    if (!is_3d) {
        if (is_quantized) {
            this->setMagneticQuantumNumber(angular);
        } else {
            setR0(Vector3{r0.x, r0.y, 0});
            setV0(Vector3{v0.x, v0.y, 0});
        }
    }
}

void StateBuilder::setPrincipalQuantumNumber(quantum_principle principal) {
    this->principal = principal;
    quantize();
}

void StateBuilder::setAngularQuantumNumber(quantum_angular angular) {
    this->angular = angular;
    if (!is_3d) {
        setMagneticQuantumNumber(angular);
    } else
        quantize();
}

void StateBuilder::setMagneticQuantumNumber(quantum_magnetic magnetic) {
    this->magnetic = magnetic;
    quantize();
}

bool StateBuilder::isRelativistic() const { return is_relativistic; }

bool StateBuilder::isQuantized() const { return is_quantized; }

bool StateBuilder::is3D() const { return is_3d; }

quantum_principle StateBuilder::getPrincipalQuantumNumber() const {
    return principal;
}

quantum_angular StateBuilder::getAngularQuantumNumber() const {
    return angular;
}

quantum_magnetic StateBuilder::getMagneticQuantumNumber() const {
    return magnetic;
}

void StateBuilder::quantizeFromR0V0(const Vector3 &r0, const Vector3 &v0) {
    const scalar r0_magnitude = magnitude(r0);

    quantum_principle _principal;
    quantum_angular _angular;
    quantum_magnetic _magnetic;

    if (is_3d) {
        if (std::abs(r0.x) > 1e-14) {
            _magnetic =
                static_cast<quantum_magnetic>(std::round(v0.y * r0_magnitude));
            _angular = static_cast<quantum_angular>(
                std::round(_magnetic * r0_magnitude / r0.x));
        } else {
            _magnetic = 0;
            _angular =
                static_cast<quantum_angular>(std::round(v0.y * r0_magnitude));
        }
    } else {
        _angular =
            static_cast<quantum_angular>(std::round(v0.y * r0_magnitude));
        _magnetic = _angular;
    }

    _principal = static_cast<quantum_principle>(std::round(
        r0_magnitude *
        physics::math::sqrt(-1 / (SQUARE(_angular) - 2 * r0_magnitude))));

    setPrincipalQuantumNumber(_principal);
    setAngularQuantumNumber(_angular);
    setMagneticQuantumNumber(_magnetic);
}

void StateBuilder::quantize() {
    const RadialBounds bounds = computeRadialLimits(principal, angular);
    const scalar r0_magnitude = bounds.r_min;
    const scalar theta_0 = computeInitialTheta(magnetic, angular);

    // phi_0 = 0
    setR0(Vector3{r0_magnitude *
                      physics::math::sin(
                          theta_0), // r0_magnitude * sin(theta_0) * cos(phi_0)
                  0,                // r0_magnitude * sin(theta_0) * sin(phi_0)
                  r0_magnitude * physics::math::cos(theta_0)});

    // 2D case
    // M * |r0| * |v0| * sin(pi/2) = angular momentum
    // M = 1 a.u of mass, sin(pi/2) = 1
    // |v0| = angular / |r0|
    // 3D case
    // M * |r0| * |v0| * sin(theta_0) =  magnetic momentum

    if (!is_3d) {
        setV0(Vector3{
            0,                      // v0_magnitude * sin(theta_0) * cos(phi_0)
            angular / r0_magnitude, // v0_magnitude * sin(theta_0) * sin(phi_0)
            0});                    // v0_magnitude * cos(theta_0)
    } else {
        if (magnetic == 0) {
            setV0(Vector3{
                0, // v0_magnitude * physics::math::sin(theta_0) * cos(phi_0)
                angular / r0_magnitude, 0}); // v0_magnitude * cos(theta_0)
        } else
            setV0(Vector3{
                0, // v0_magnitude * sin(theta_0) * cos(phi_0)
                magnetic /
                    r0_magnitude, // v0_magnitude * sin(theta_0) * sin(phi_0)
                0});              // v0_magnitude * cos(theta_0)
    }
}

scalar StateBuilder::getAngularMomentum() const {
    const scalar angle = angle_with(r0, v0);
    const scalar r0_magnitude = magnitude(r0);
    const scalar v0_magnitude = magnitude(v0);

    return r0_magnitude * v0_magnitude * physics::math::sin(angle);
}

} // namespace simulation::factories