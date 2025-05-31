#ifndef STAT3D_HPP
#define STAT3D_HPP

#include <array>
#include <optional>

#include "State.hpp"
#include "eom/polar.hpp"
#include "eom/polar_rel.hpp"
#include "eom/spherical.hpp"
#include "eom/spherical_rel.hpp"
#include "eom/utils.hpp"
#include "steppers/Potential.hpp"

struct State3DBase : State {
    State3DBase(const eom::Vector3 &r0, const eom::Vector3 &v0)
        : r(eom::spherical::compute_r_0(r0)),
          r_dot(eom::spherical::compute_r_dot_0(r0, v0)),
          theta(eom::spherical::compute_theta_0(r0)),
          theta_dot(eom::spherical::compute_theta_dot_0(r0, v0)),
          phi(eom::spherical::compute_phi_0(r0)),
          p_phi(eom::spherical::compute_p_phi(r0, v0)) {}

    template <typename EOM>
    SIM_INLINE inline void integrate_common(scalar delta_time,
                                            const PotentialEvaluator &m_du) {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= TWO_PI * static_cast<scalar>(phi >= TWO_PI);

        EOM::update(*this, m_du);
    }

    scalar r, r_dot, r_ddot;
    scalar theta, theta_dot, theta_ddot;
    scalar phi, phi_dot;

    scalar gamma; // Only used in relativistic case

    scalar p_phi;
};

template <bool IsRel, bool IsQuant, typename Pot> struct State3D {};

template <typename Pot> struct State3D<false, false, Pot> : State3DBase {
    static constexpr std::array<std::string_view, 9> field_names = {
        "t",         "r",          "r_dot", "r_ddot", "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot"};

    template <typename Factory>
    State3D(const eom::Vector3 &r0, const eom::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dnr = eom::spherical::non_rel;
        phi_dot = eom_3dnr::compute_phi_dot(r, theta, p_phi);
        theta_ddot =
            eom_3dnr::compute_theta_ddot(r, r_dot, theta, theta_dot, phi_dot);
        r_ddot = eom_3dnr::compute_r_ddot(r, theta, theta_dot, phi_dot,
                                          m_du.dU_dr());
    }

    State3D(const State3D &other)
        : State3DBase(other), m_du(other.m_du.rebind(r)) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= TWO_PI * static_cast<scalar>(phi >= TWO_PI);

        namespace eom_3dnr = eom::spherical::non_rel;
        phi_dot = eom_3dnr::compute_phi_dot(r, theta, p_phi);
        r_ddot = eom_3dnr::compute_r_ddot(r, theta, theta_dot, phi_dot,
                                          m_du.dU_dr());
        theta_ddot =
            eom_3dnr::compute_theta_ddot(r, r_dot, theta, theta_dot, phi_dot);

        return false;
    }

    SIM_CONST SIM_INLINE inline const std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot);
    };

  private:
    Pot m_du;
};

template <typename Pot>
struct State3D<true, false, Pot> : State3DBase, Relativistic {
    static constexpr std::array<std::string_view, 10> field_names = {
        "t",         "r",          "r_dot", "r_ddot",  "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot", "gamma"};

    template <typename Factory>
    State3D(const eom::Vector3 &r0, const eom::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dr = eom::spherical::rel;
        gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);
        phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
        theta_ddot = eom_3dr::compute_theta_ddot(r, r_dot, theta, theta_dot,
                                                 phi_dot, gamma, m_du.dU_dr());
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());
    }

    State3D(const State3D &other)
        : State3DBase(other), m_du(other.m_du.rebind(r)) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= TWO_PI * static_cast<scalar>(phi >= TWO_PI);

        namespace eom_3dr = eom::spherical::rel;
        gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);
        phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());
        theta_ddot = eom_3dr::compute_theta_ddot(r, r_dot, theta, theta_dot,
                                                 phi_dot, gamma, m_du.dU_dr());

        return false;
    }

    SIM_CONST SIM_INLINE inline const std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, gamma);
    }

  private:
    Pot m_du;
};

template <typename Pot>
struct State3D<false, true, Pot> : State3DBase, Quantized {
    static constexpr std::array<std::string_view, 10> field_names = {
        "t",         "r",          "r_dot", "r_ddot",  "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot", "delta_psi"};

    template <typename Factory>
    State3D(const eom::Vector3 &r0, const eom::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dnr = eom::spherical::non_rel;
        m_theta0 = eom::sin(theta);
        if (std::abs(m_theta0) <= 1e-6) {
            phi = HALF_PI;
            phi_dot = 0;
            theta_ddot = 0;

            theta_dot = eom::polar::non_rel::compute_psi_dot(r, p_phi);
        } else {
            phi_dot = eom_3dnr::compute_phi_dot(r, theta, p_phi);
            theta_ddot = eom_3dnr::compute_theta_ddot(r, r_dot, theta,
                                                      theta_dot, phi_dot);
        }
        r_ddot = eom_3dnr::compute_r_ddot(r, theta, theta_dot, phi_dot,
                                          m_du.dU_dr());
    }

    State3D(const State3D &other)
        : State3DBase(other), m_du(other.m_du.rebind(r)),
          m_theta0(other.m_theta0), prev_r_dot(other.prev_r_dot),
          is_max(other.is_max), sign(other.sign), theta_flag(other.theta_flag),
          prev_max_vec(other.prev_max_vec) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
            m_theta0 = other.m_theta0;
            prev_r_dot = other.prev_r_dot;
            is_max = other.is_max;
            sign = other.sign;
            theta_flag = other.theta_flag;
            prev_max_vec = other.prev_max_vec;
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= TWO_PI * static_cast<scalar>(phi >= TWO_PI);

        namespace eom_3dnr = eom::spherical::non_rel;
        if (std::abs(m_theta0) <= 1e-6) {
            theta_dot = sign * eom::polar::non_rel::compute_psi_dot(r, p_phi);

            // TODO: make this branchless
            if (theta >= SIM_PI && !theta_flag) {
                theta_flag = true;
                sign = -1;
                phi = -phi;
            } else if (theta <= 0 && theta_flag) {
                theta_flag = false;
                sign = 1;
                phi = -phi;
            }
        } else {
            phi_dot = eom_3dnr::compute_phi_dot(r, theta, p_phi);
            theta_ddot = eom_3dnr::compute_theta_ddot(r, r_dot, theta,
                                                      theta_dot, phi_dot);
        }

        r_ddot = eom_3dnr::compute_r_ddot(r, theta, theta_dot, phi_dot,
                                          m_du.dU_dr());
        bool isMustRecord = false;
        if (r_dot * prev_r_dot < 0) { // TODO: try to make this branchless
            is_max = !is_max;

            if (is_max) {
                eom::Vector3 curr_max_vec =
                    eom::spherical_to_cartesian(r, theta, phi);

                if (prev_max_vec) {
                    delta_psi += eom::compute_angular_distance(*prev_max_vec,
                                                               curr_max_vec);
                    isMustRecord = true;
                }

                prev_max_vec = curr_max_vec;
            }
        }

        return isMustRecord;
    }

    SIM_CONST SIM_INLINE inline std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, delta_psi);
    }

  private:
    Pot m_du;
    scalar m_theta0;
    scalar prev_r_dot = 0;
    bool is_max = false;

    scalar sign = 1;
    scalar theta_flag = false;

    std::optional<eom::Vector3> prev_max_vec = std::nullopt;
};

template <typename Pot>
struct State3D<true, true, Pot> : State3DBase, Relativistic, Quantized {
    static constexpr std::array<std::string_view, 11> field_names = {
        "t",          "r",   "r_dot",   "r_ddot", "theta",    "theta_dot",
        "theta_ddot", "phi", "phi_dot", "gamma",  "delta_psi"};

    template <typename Factory>
    State3D(const eom::Vector3 &r0, const eom::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dr = eom::spherical::rel;
        m_theta0 = eom::sin(theta);
        gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);

        if (std::abs(m_theta0) <= 1e-6) {
            sign = 1;
            phi = HALF_PI;
            phi_dot = 0;
            theta_ddot = 0;

            theta_dot = eom::polar::rel::compute_psi_dot(r, gamma, p_phi);
        } else {
            phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
            theta_ddot = eom_3dr::compute_theta_ddot(
                r, r_dot, theta, theta_dot, phi_dot, gamma, m_du.dU_dr());
        }
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());
    }

    State3D(const State3D &other)
        : State3DBase(other), m_du(other.m_du.rebind(r)),
          prev_r_dot(other.prev_r_dot), is_max(other.is_max),
          m_theta0(other.m_theta0), sign(other.sign),
          theta_flag(other.theta_flag), prev_max_vec(other.prev_max_vec) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
            m_theta0 = other.m_theta0;
            prev_r_dot = other.prev_r_dot;
            is_max = other.is_max;
            sign = other.sign;
            theta_flag = other.theta_flag;
            prev_max_vec = other.prev_max_vec;
        }

        return *this;
    }

    SIM_CONST SIM_CONST SIM_INLINE inline bool
    iterate(scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= TWO_PI * static_cast<scalar>(phi >= TWO_PI);

        namespace eom_3dr = eom::spherical::rel;
        gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);

        if (std::abs(m_theta0) <= 1e-6) {
            theta_dot =
                sign * eom::polar::rel::compute_psi_dot(r, gamma, p_phi);

            // TODO: make this branchless
            if (theta >= SIM_PI && !theta_flag) {
                theta_flag = true;
                sign = -1;
                phi = -phi;
            } else if (theta <= 0 && theta_flag) {
                theta_flag = false;
                sign = 1;
                phi = -phi;
            }
        } else {
            phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
            theta_ddot = eom_3dr::compute_theta_ddot(
                r, r_dot, theta, theta_dot, phi_dot, gamma, m_du.dU_dr());
        }
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());

        bool isMustRecord = false;
        if (r_dot * prev_r_dot < 0) { // TODO: try to make this branchless
            is_max = !is_max;

            if (is_max) {
                eom::Vector3 curr_max_vec =
                    eom::spherical_to_cartesian(r, theta, phi);

                if (prev_max_vec) {
                    delta_psi += eom::compute_angular_distance(*prev_max_vec,
                                                               curr_max_vec);
                    isMustRecord = true;
                }

                prev_max_vec = curr_max_vec;
            }
        }

        return isMustRecord;
    }

    SIM_CONST SIM_INLINE inline std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, gamma, delta_psi);
    }

  private:
    Pot m_du;
    scalar prev_r_dot = 0;
    bool is_max = false;
    scalar m_theta0;

    scalar sign = 1;
    scalar theta_flag = false;

    std::optional<eom::Vector3> prev_max_vec = std::nullopt;
};

#endif // STAT3D_HPP