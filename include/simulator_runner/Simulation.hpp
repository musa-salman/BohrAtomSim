#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include "atom/atom_bohr_sim.h"
#include "math_utils.hpp"
#include "orbital_math.h"
#include "utils/iterator.h"

class Simulation {
    std::string name;
    size_t id;

    unsigned short record_interval;
    double delta_time;
    double total_duration;

    double r_0;
    double v_0;

    // is the angle between vector r and v
    double theta_rv;

    double r[2];
    double v[2];

  public:
    struct motion_step_2d initial_motion_step;

    enum class SimulationStatus { IDLE, RUNNING, PAUSED, COMPLETED };

    SimulationStatus status = SimulationStatus::IDLE;

    Simulation(const std::string &name, unsigned short record_interval,
               float total_duration, double time_interval, double r_0,
               double v_0, double theta_rv)
        : name(name), record_interval(record_interval),
          delta_time(time_interval), total_duration(total_duration), r_0(r_0),
          v_0(v_0), theta_rv(theta_rv) {

        if (name.empty()) {
            throw std::invalid_argument("name cannot be empty");
        }

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
        setR0(r_0);
        setV0(v_0);
        setThetaRV(theta_rv);
    }

    Simulation(const Simulation &simulation)
        : name(simulation.name), id(simulation.id),
          record_interval(simulation.record_interval),
          delta_time(simulation.delta_time),
          total_duration(simulation.total_duration), r_0(simulation.r_0),
          v_0(simulation.v_0), theta_rv(simulation.theta_rv) {
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
        setR0(r_0);
        setV0(v_0);
        setThetaRV(theta_rv);
    }
    Simulation(const std::string &name, size_t id,
               unsigned short record_interval, float total_duration,
               double time_interval, double r_0, double v_0, double theta_rv)
        : name(name), id(id), record_interval(record_interval),
          delta_time(time_interval), total_duration(total_duration), r_0(r_0),
          v_0(v_0), theta_rv(theta_rv) {
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
        setR0(r_0);
        setV0(v_0);
        setThetaRV(theta_rv);
    }

    Simulation()
        : name(""), record_interval(10000), delta_time(1e-7),
          total_duration(800.0), r_0(2.2917960675006309107), v_0(0.87267799625),
          theta_rv(HALF_PI) {
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
        setR0(r_0);
        setV0(v_0);
        setThetaRV(theta_rv);
    }

    void quantize(uint8_t principle, uint8_t azimuthal) {

        struct radial_bounds bounds =
            compute_radial_limits(principle, azimuthal);

        r_0 = bounds.r_min;
        theta_rv = HALF_PI;
        v_0 = azimuthal / r_0;

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);

        setR0(r_0);
        setV0(v_0);
        setThetaRV(theta_rv);
    }

    void setR0(double r0) {
        r_0 = r0;
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);

        r[0] = r_0 * cos(theta_rv);
        r[1] = r_0 * sin(theta_rv);
    }

    void setR0X(double r0_x) {
        r[0] = r0_x;
        r_0 = sqrt(r[0] * r[0] + r[1] * r[1]);

        theta_rv = computeAngleVec2D(r, v);

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    void setR0Y(double r0_y) {
        r[1] = r0_y;
        r_0 = sqrt(r[0] * r[0] + r[1] * r[1]);

        theta_rv = computeAngleVec2D(r, v);

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    void setV0(double v0) {
        v_0 = v0;
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);

        v[0] = v_0 * cos(theta_rv);
        v[1] = v_0 * sin(theta_rv);
    }

    void setV0X(double v0_x) {
        v[0] = v0_x;
        v_0 = sqrt(v[0] * v[0] + v[1] * v[1]);

        theta_rv = computeAngleVec2D(r, v);

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    void setV0Y(double v0_y) {
        v[1] = v0_y;
        v_0 = sqrt(v[0] * v[0] + v[1] * v[1]);

        theta_rv = computeAngleVec2D(r, v);

        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);
    }

    void setThetaRV(double theta_rv) {
        this->theta_rv = theta_rv;
        init_motion_step(&initial_motion_step, r_0, v_0, theta_rv);

        r[0] = r_0 * cos(theta_rv);
        r[1] = r_0 * sin(theta_rv);

        v[0] = v_0 * cos(theta_rv);
        v[1] = v_0 * sin(theta_rv);
    }

    void setRecordInterval(unsigned short interval) {
        record_interval = interval;
    }

    void setTotalDuration(double duration) {
        if (duration <= 0) {
            throw std::invalid_argument("Total duration must be positive");
        }

        total_duration = duration;
    }

    void setDeltaTime(double time_interval) {
        if (time_interval > 0)
            delta_time = time_interval;
    }

    void setName(const std::string &name) { this->name = name; }

    void setId(size_t id) { this->id = id; }

    void setStatus(SimulationStatus status) { this->status = status; }

    std::string getName() const { return name; }

    size_t getId() const { return id; }

    unsigned short getRecordInterval() const { return record_interval; }

    double getDeltaTime() const { return delta_time; }

    double getTotalDuration() const { return total_duration; }

    double getR0() const { return r_0; }

    double getV0() const { return v_0; }

    double getThetaRV() const { return theta_rv; }

    double getR0X() const { return r[0]; }

    double getR0Y() const { return r[1]; }

    double getV0X() const { return v[0]; }

    double getV0Y() const { return v[1]; }
};

#endif // SIMULATION_HPP