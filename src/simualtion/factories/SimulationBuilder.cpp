#include "simulation/factories/SimulationBuilder.hpp"
#include "physics/math/QuantumUtils.hpp"
#include "physics/math/Vector3.hpp"
#include "physics/math/VectorOps.hpp"
#include <unordered_map>

namespace simulation::factories {

using namespace physics::math;

SimulationBuilder::SimulationBuilder()
    : m_principal(3), m_angular(2), m_magnetic(2),
      m_simulation("New Simulation", 0, true, false, false,
                   Vector3{2.291796067500631, 0, 0},
                   Vector3{0, 0.872677996249964890, 0}, model::Potential(),
                   1e-6, 1000, 800, -1) {}

void SimulationBuilder::setR0(const Vector3 &r) { m_simulation.setR0(r); }

void SimulationBuilder::setV0(const Vector3 &v) { m_simulation.setV0(v); }

const Vector3 &SimulationBuilder::getR0() const { return m_simulation.getR0(); }

const Vector3 &SimulationBuilder::getV0() const { return m_simulation.getV0(); }

void SimulationBuilder::setR0Magnitude(scalar r0_magnitude) {
    Vector3 r0_unit = normalized(m_simulation.getR0());
    m_simulation.setR0(r0_unit * r0_magnitude);
}

void SimulationBuilder::setV0Magnitude(scalar v0_magnitude) {
    Vector3 v0_unit = normalized(m_simulation.getV0());
    m_simulation.setV0(v0_unit * v0_magnitude);
}

void SimulationBuilder::setRelativistic(bool is_rel) {
    m_simulation.setRelativistic(is_rel);
}

void SimulationBuilder::setQuantized(bool is_quant) {
    m_simulation.setQuantized(is_quant);

    if (is_quant)
        _quantize();
}

void SimulationBuilder::set3D(bool is_3d) {
    m_simulation.set3D(is_3d);
    if (!is_3d) {
        if (m_simulation.isQuantized()) {
            this->setMagneticQuantumNumber(m_angular);
        } else {
            const auto &initialPosition = m_simulation.getR0();
            const auto &initialVelocity = m_simulation.getV0();
            setR0(Vector3{initialPosition.x, initialPosition.y, 0});
            setV0(Vector3{initialVelocity.x, initialVelocity.y, 0});
        }
    }
}

void SimulationBuilder::setPrincipalQuantumNumber(quantum_principle principal) {
    this->m_principal = principal;
    _quantize();
}

void SimulationBuilder::setAngularQuantumNumber(quantum_angular angular) {
    this->m_angular = angular;
    if (!m_simulation.is3D()) {
        setMagneticQuantumNumber(angular);
    } else
        _quantize();
}

void SimulationBuilder::setMagneticQuantumNumber(quantum_magnetic magnetic) {
    this->m_magnetic = magnetic;
    _quantize();
}

bool SimulationBuilder::isRelativistic() const {
    return m_simulation.isRelativistic();
}

bool SimulationBuilder::isQuantized() const {
    return m_simulation.isQuantized();
}

bool SimulationBuilder::is3D() const { return m_simulation.is3D(); }

quantum_principle SimulationBuilder::getPrincipalQuantumNumber() const {
    return m_principal;
}

quantum_angular SimulationBuilder::getAngularQuantumNumber() const {
    return m_angular;
}

quantum_magnetic SimulationBuilder::getMagneticQuantumNumber() const {
    return m_magnetic;
}

void SimulationBuilder::quantizeFromR0V0(const Vector3 &r0, const Vector3 &v0) {
    const scalar initialRadiusMagnitude = computeMagnitude(r0);

    quantum_principle principal;
    quantum_angular angular;
    quantum_magnetic magnetic;

    if (m_simulation.is3D()) {
        if (std::abs(r0.x) > 1e-14) {
            magnetic = static_cast<quantum_magnetic>(
                std::round(v0.y * initialRadiusMagnitude));
            angular = static_cast<quantum_angular>(
                std::round(magnetic * initialRadiusMagnitude / r0.x));
        } else {
            magnetic = 0;
            angular = static_cast<quantum_angular>(
                std::round(v0.y * initialRadiusMagnitude));
        }
    } else {
        angular = static_cast<quantum_angular>(
            std::round(v0.y * initialRadiusMagnitude));
        magnetic = angular;
    }

    principal = static_cast<quantum_principle>(
        std::round(initialRadiusMagnitude *
                   physics::math::sqrt(
                       -1 / (SQUARE(angular) - 2 * initialRadiusMagnitude))));

    setPrincipalQuantumNumber(principal);
    setAngularQuantumNumber(angular);
    setMagneticQuantumNumber(magnetic);
}

void SimulationBuilder::_quantize() {
    const RadialBounds bounds = computeRadialLimits(m_principal, m_angular);
    const scalar initialRadiusMagnitude = bounds.r_min;
    const scalar initialTheta = computeInitialTheta(m_magnetic, m_angular);

    // phi_0 = 0
    setR0(Vector3{
        initialRadiusMagnitude *
            physics::math::sin(
                initialTheta), // r0_magnitude * sin(theta_0) * cos(phi_0)
        0,                     // r0_magnitude * sin(theta_0) * sin(phi_0)
        initialRadiusMagnitude * physics::math::cos(initialTheta)});

    // 2D case
    // M * |r0| * |v0| * sin(pi/2) = angular momentum
    // M = 1 a.u of mass, sin(pi/2) = 1
    // |v0| = angular / |r0|
    // 3D case
    // M * |r0| * |v0| * sin(theta_0) =  magnetic momentum
    if (!m_simulation.is3D()) {
        setV0(Vector3{0, // v0_magnitude * sin(theta_0) * cos(phi_0)
                      m_angular /
                          initialRadiusMagnitude, // v0_magnitude * sin(theta_0)
                                                  // * sin(phi_0)
                      0});                        // v0_magnitude * cos(theta_0)
    } else {
        if (m_magnetic == 0) {
            setV0(Vector3{
                0, // v0_magnitude * physics::math::sin(theta_0) * cos(phi_0)
                m_angular / initialRadiusMagnitude,
                0}); // v0_magnitude * cos(theta_0)
        } else
            setV0(
                Vector3{0, // v0_magnitude * sin(theta_0) * cos(phi_0)
                        m_magnetic /
                            initialRadiusMagnitude, // v0_magnitude *
                                                    // sin(theta_0) * sin(phi_0)
                        0}); // v0_magnitude * cos(theta_0)
    }
}

scalar SimulationBuilder::getAngularMomentum() const {
    const auto &initialRadius = m_simulation.getR0();
    const auto &initialVelocity = m_simulation.getV0();

    const scalar angle = computeAngleBetween(initialRadius, initialVelocity);
    const scalar initialRadiusMagnitude = computeMagnitude(initialRadius);
    const scalar initialVelocityMagnitude = computeMagnitude(initialVelocity);

    return initialRadiusMagnitude * initialVelocityMagnitude *
           physics::math::sin(angle);
}

void SimulationBuilder::setSimulation(const model::Simulation &simulation) {
    m_simulation = simulation;

    if (m_simulation.isQuantized()) {
        _quantize();
    }
}
model::Simulation &SimulationBuilder::getSimulation() { return m_simulation; }

model::Simulation SimulationBuilder::build() const { return m_simulation; }
void SimulationBuilder::setName(const std::string &name) {
    m_simulation.setName(name);
}

std::string SimulationBuilder::getName() const {
    return m_simulation.getName();
}

void SimulationBuilder::setDeltaTime(scalar delta_time) {
    m_simulation.setDeltaTime(delta_time);
}

scalar SimulationBuilder::getDeltaTime() const {
    return m_simulation.getDeltaTime();
}

void SimulationBuilder::setTotalDuration(scalar total_duration) {
    m_simulation.setTotalDuration(total_duration);
}

scalar SimulationBuilder::getTotalDuration() const {
    return m_simulation.getTotalDuration();
}

void SimulationBuilder::setRecordInterval(uint record_interval) {
    m_simulation.setRecordInterval(record_interval);
}

uint SimulationBuilder::getRecordInterval() const {
    return m_simulation.getRecordInterval();
}

int SimulationBuilder::getRLocalMaxCountLimit() const {
    return m_simulation.getRLocalMaxCountLimit();
}

void SimulationBuilder::setRLocalMaxCountLimit(int count) {
    m_simulation.setRLocalMaxCountLimit(count);
}

void SimulationBuilder::setNotLimitedRLocalMaxCount() {
    m_simulation.setRLocalMaxCountLimit(-1);
}

const std::unordered_map<std::string, scalar> &
SimulationBuilder::getConstantValues() const {
    return m_simulation.getConstantValues();
}

void SimulationBuilder::setPotential(const model::Potential &potential) {
    m_simulation.setPotential(potential);
}

const model::Potential &SimulationBuilder::getPotential() const {
    return m_simulation.getPotential();
}

void SimulationBuilder::setConstantValue(const std::string &key, scalar value) {
    m_simulation.setConstantValue(key, value);
}

scalar SimulationBuilder::getConstantValue(const std::string &key) const {
    return m_simulation.getConstantValue(key);
}

void SimulationBuilder::removeConstantValue(const std::string &key) {
    m_simulation.removeConstantValue(key);
}
} // namespace simulation::factories