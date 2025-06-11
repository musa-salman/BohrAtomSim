#include "simulation/model/Simulation.hpp"

namespace simulation::model {

using namespace physics::math;

Simulation::Simulation()
    : status(SimulationStatus::READY), m_name("New Simulation"), m_id(0),
      m_r0({2.291796067500631, 0, 0}), m_v0({0, 0.872677996249964890, 0}),
      m_deltaTime(1e-6), m_totalDuration(800), m_rLocalMaxCountLimit(-1),
      m_recordInterval(1000), m_potential(), m_isRelativistic(true),
      m_isQuantized(false), m_is3D(false) {}

Simulation::Simulation(std::string name, size_t id, bool isRelativistic,
                       bool isQuantized, bool is3D, Vector3 r0, Vector3 v0,
                       Potential potential, scalar delta_time,
                       uint record_interval, scalar total_duration,
                       int rLocalMaxCount)
    : status(SimulationStatus::READY), m_name(std::move(name)), m_id(id),
      m_r0(std::move(r0)), m_v0(std::move(v0)), m_deltaTime(delta_time),
      m_totalDuration(total_duration), m_rLocalMaxCountLimit(rLocalMaxCount),
      m_recordInterval(record_interval), m_potential(std::move(potential)),
      m_isRelativistic(isRelativistic), m_isQuantized(isQuantized),
      m_is3D(is3D) {}

Simulation::Simulation(const Simulation &other)
    : m_name(other.m_name), m_id(other.m_id) {
    m_r0 = other.m_r0;
    m_v0 = other.m_v0;
    m_potential = other.m_potential;
    status = other.status;
    m_totalDuration = other.m_totalDuration;
    m_deltaTime = other.m_deltaTime;
    m_recordInterval = other.m_recordInterval;
    m_constantValues = other.m_constantValues;
    m_isRelativistic = other.m_isRelativistic;
    m_isQuantized = other.m_isQuantized;
    m_is3D = other.m_is3D;
    m_rLocalMaxCountLimit = other.m_rLocalMaxCountLimit;
}

Simulation &Simulation::operator=(const Simulation &other) {
    if (this != &other) {
        m_name = other.m_name;
        m_id = other.m_id;
        m_potential = other.m_potential;
        status = other.status;
        m_r0 = other.m_r0;
        m_v0 = other.m_v0;
        m_deltaTime = other.m_deltaTime;
        m_totalDuration = other.m_totalDuration;
        m_recordInterval = other.m_recordInterval;
        m_constantValues = other.m_constantValues;
        m_isRelativistic = other.m_isRelativistic;
        m_isQuantized = other.m_isQuantized;
        m_is3D = other.m_is3D;
        m_rLocalMaxCountLimit = other.m_rLocalMaxCountLimit;
    }
    return *this;
}

Simulation::Simulation(Simulation &&other) noexcept
    : status(other.status), m_name(std::move(other.m_name)), m_id(other.m_id),
      m_r0(std::move(other.m_r0)), m_v0(std::move(other.m_v0)),
      m_deltaTime(other.m_deltaTime), m_totalDuration(other.m_totalDuration),
      m_rLocalMaxCountLimit(other.m_rLocalMaxCountLimit),
      m_recordInterval(other.m_recordInterval),
      m_potential(std::move(other.m_potential)),
      m_constantValues(std::move(other.m_constantValues)),
      m_isRelativistic(other.m_isRelativistic),
      m_isQuantized(other.m_isQuantized), m_is3D(other.m_is3D) {}

Simulation &Simulation::operator=(Simulation &&other) noexcept {
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_id = other.m_id;
        m_r0 = std::move(other.m_r0);
        m_v0 = std::move(other.m_v0);
        m_potential = std::move(other.m_potential);
        status = other.status;
        m_totalDuration = other.m_totalDuration;
        m_deltaTime = other.m_deltaTime;
        m_recordInterval = other.m_recordInterval;
        m_constantValues = std::move(other.m_constantValues);
        other.m_id = m_id;
        m_is3D = other.m_is3D;
        m_isRelativistic = other.m_isRelativistic;
        m_isQuantized = other.m_isQuantized;
        m_rLocalMaxCountLimit = other.m_rLocalMaxCountLimit;
    }
    return *this;
}

const std::unordered_map<std::string, scalar> &
Simulation::getConstantValues() const {
    return m_constantValues;
}

void Simulation::setConstantValues(
    const std::unordered_map<std::string, scalar> &values) {
    m_constantValues = values;
}

void Simulation::setConstantValues(const std::string &jsonString) {
    nlohmann::json constantsJson = nlohmann::json::parse(jsonString);
    for (const auto &[key, value] : constantsJson.items()) {
        m_constantValues[key] = value.get<scalar>();
    }
}

void Simulation::clearConstantValues() { m_constantValues.clear(); }

bool Simulation::hasConstantValue(const std::string &key) const {
    return m_constantValues.find(key) != m_constantValues.end();
}

void Simulation::removeConstantValue(const std::string &key) {
    m_constantValues.erase(key);
}

void Simulation::setConstantValue(const std::string &key, scalar value) {
    m_constantValues[key] = value;
}

scalar Simulation::getConstantValue(const std::string &key) const {
    auto it = m_constantValues.find(key);
    if (it != m_constantValues.end()) {
        return it->second;
    }
    return -9999e99;
}

const Vector3 &Simulation::getR0() const { return m_r0; }
const Vector3 &Simulation::getV0() const { return m_v0; }
void Simulation::setR0(const Vector3 &r0) { this->m_r0 = r0; }
void Simulation::setV0(const Vector3 &v0) { this->m_v0 = v0; }

Simulation::SimulationStatus Simulation::getStatus() const { return status; }
void Simulation::setName(const std::string &name) { this->m_name = name; }
void Simulation::setId(size_t id) { this->m_id = id; }
void Simulation::setStatus(SimulationStatus status) { this->status = status; }
std::string Simulation::getName() const { return m_name; }
size_t Simulation::getId() const { return m_id; }

const Potential &Simulation::getPotential() const { return m_potential; }

void Simulation::setPotential(const Potential &potential) {
    this->m_potential = potential;
    this->clearConstantValues();
    this->setConstantValues(potential.getConstants());
}

std::string Simulation::serializeConstants() const {
    nlohmann::json constantsJson;
    for (const auto &[key, value] : m_constantValues) {
        constantsJson[key] = value;
    }
    return constantsJson.dump();
}

scalar Simulation::getDeltaTime() const { return m_deltaTime; }
scalar Simulation::getTotalDuration() const { return m_totalDuration; }
uint Simulation::getRecordInterval() const { return m_recordInterval; }

void Simulation::setDeltaTime(scalar delta_time) {
    this->m_deltaTime = delta_time;
}
void Simulation::setTotalDuration(scalar total_duration) {
    this->m_totalDuration = total_duration;
}
void Simulation::setRecordInterval(uint record_interval) {
    this->m_recordInterval = record_interval;
}

int Simulation::getRLocalMaxCountLimit() const { return m_rLocalMaxCountLimit; }
void Simulation::setRLocalMaxCountLimit(int count) {
    m_rLocalMaxCountLimit = count;
}

bool Simulation::isRelativistic() const { return m_isRelativistic; }
bool Simulation::isQuantized() const { return m_isQuantized; }
bool Simulation::is3D() const { return m_is3D; }

void Simulation::setRelativistic(bool is_rel) { m_isRelativistic = is_rel; }
void Simulation::setQuantized(bool is_quant) { m_isQuantized = is_quant; }
void Simulation::set3D(bool is_3d) { m_is3D = is_3d; }

} // namespace simulation::model
