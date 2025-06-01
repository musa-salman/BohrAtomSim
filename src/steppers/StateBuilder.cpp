#include "steppers/StateBuilder.hpp"
#include "eom/spherical.hpp"
#include "eom/utils.hpp"

void StateBuilder::setR0(const eom::Vector3 &r) { r0 = r; }

void StateBuilder::setV0(const eom::Vector3 &v) { v0 = v; }

const eom::Vector3 &StateBuilder::getR0() const { return r0; }

const eom::Vector3 &StateBuilder::getV0() const { return v0; }

void StateBuilder::setR0Magnitude(scalar r0_magnitude) {
    eom::Vector3 r0_unit = r0.normalized();
    r0 = r0_unit * r0_magnitude;
}

void StateBuilder::setV0Magnitude(scalar v0_magnitude) {
    eom::Vector3 v0_unit = v0.normalized();
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
            setR0(eom::Vector3{r0.x, r0.y, 0});
            setV0(eom::Vector3{v0.x, v0.y, 0});
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

void StateBuilder::quantize() {
    const eom::radial_bounds bounds =
        eom::compute_radial_limits(principal, angular);
    const scalar r0_magnitude = bounds.r_min;
    const scalar theta_0 = eom::spherical::compute_theta_0(magnetic, angular);

    // phi_0 = 0
    setR0(eom::Vector3{
        r0_magnitude *
            eom::sin(theta_0), // r0_magnitude * eom::sin(theta_0) * cos(phi_0)
        0,                     // r0_magnitude * eom::sin(theta_0) * sin(phi_0)
        r0_magnitude * eom::cos(theta_0)});

    // 2D case
    // M * |r0| * |v0| * sin(pi/2) = angular momentum
    // M = 1 a.u of mass, sin(pi/2) = 1
    // |v0| = angular / |r0|
    // 3D case
    // M * |r0| * |v0| * sin(theta_0) =  magnetic momentum

    if (!is_3d) {
        setV0(eom::Vector3{
            0, // v0_magnitude * eom::sin(theta_0) * cos(phi_0)
            angular /
                r0_magnitude, // v0_magnitude * eom::sin(theta_0) * sin(phi_0)
            0});              // v0_magnitude * eom::cos(theta_0)
        return;
    } else {

        setV0(eom::Vector3{
            0, // v0_magnitude * eom::sin(theta_0) * cos(phi_0)
            magnetic /
                r0_magnitude, // v0_magnitude * eom::sin(theta_0) * sin(phi_0)
            0});              // v0_magnitude * eom::cos(theta_0)
    }
}

scalar StateBuilder::getAngularMomentum() const {
    const scalar angle = r0.angle_with(v0);
    const scalar r0_magnitude = r0.magnitude();
    const scalar v0_magnitude = v0.magnitude();

    return r0_magnitude * v0_magnitude * eom::sin(angle);
}
