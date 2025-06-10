#pragma once

#include <array>
#include <optional>
#include <string_view>

#include "physics/eom/eom_common.hpp"
#include "physics/eom/polar.hpp"
#include "physics/eom/polar_rel.hpp"
#include "physics/eom/spherical.hpp"
#include "physics/eom/spherical_rel.hpp"
#include "physics/math/Vector3.hpp"
#include "physics/math/constants.hpp"
#include "physics/math/intrinsics.hpp"
#include "physics/potential/PotentialEvaluator.hpp"
#include "physics/state/State.hpp"

namespace physics::state {
struct State3DBase : State {
    State3DBase(const physics::math::Vector3 &r0, const math::Vector3 &v0)
        : r(eom::spherical::compute_r_0(r0)),
          r_dot(eom::spherical::compute_r_dot_0(r0, v0)),
          theta(eom::spherical::compute_theta_0(r0)),
          theta_dot(eom::spherical::compute_theta_dot_0(r0, v0)),
          phi(eom::spherical::compute_phi_0(r0)),
          p_phi(eom::spherical::compute_p_phi(r0, v0)) {}

    template <typename EOM>
    SIM_INLINE void
    integrate_common(math::scalar delta_time,
                     const potential::PotentialEvaluator &m_du) noexcept {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= math::TWO_PI * static_cast<math::scalar>(phi >= math::TWO_PI);

        EOM::update(*this, m_du);
    }

    math::scalar r, r_dot, r_ddot;
    math::scalar theta, theta_dot, theta_ddot;
    math::scalar phi, phi_dot;

    math::scalar gamma; // Only used in relativistic case

    math::scalar p_phi;
};

template <bool IsRel, bool IsQuant, typename Pot> struct State3D {};

template <typename Pot> struct State3D<false, false, Pot> : State3DBase {
    static constexpr std::array<std::string_view, 9> field_names = {
        "t",         "r",          "r_dot", "r_ddot", "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot"};

    template <typename Factory>
    State3D(const math::Vector3 &r0, const math::Vector3 &v0, Factory &&factory)
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

    SIM_CONST SIM_INLINE bool iterate(math::scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= math::TWO_PI * static_cast<math::scalar>(phi >= math::TWO_PI);

        namespace eom_3dnr = eom::spherical::non_rel;
        phi_dot = eom_3dnr::compute_phi_dot(r, theta, p_phi);
        r_ddot = eom_3dnr::compute_r_ddot(r, theta, theta_dot, phi_dot,
                                          m_du.dU_dr());
        theta_ddot =
            eom_3dnr::compute_theta_ddot(r, r_dot, theta, theta_dot, phi_dot);

        return eom::isLocalMax(prev_r_dot, r_dot);
    }

    SIM_CONST SIM_INLINE const std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot);
    };

  private:
    Pot m_du;
    math::scalar prev_r_dot = 0;
};

template <typename Pot>
struct State3D<true, false, Pot> : State3DBase, Relativistic {
    static constexpr std::array<std::string_view, 10> field_names = {
        "t",         "r",          "r_dot", "r_ddot",  "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot", "gamma"};

    template <typename Factory>
    State3D(const math::Vector3 &r0, const math::Vector3 &v0, Factory &&factory)
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

    SIM_CONST SIM_INLINE bool iterate(math::scalar delta_time) noexcept {
        prev_r_dot = r_dot;
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= math::TWO_PI * static_cast<math::scalar>(phi >= math::TWO_PI);

        namespace eom_3dr = eom::spherical::rel;
        gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);
        phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());
        theta_ddot = eom_3dr::compute_theta_ddot(r, r_dot, theta, theta_dot,
                                                 phi_dot, gamma, m_du.dU_dr());

        return eom::isLocalMax(prev_r_dot, r_dot);
    }

    SIM_CONST SIM_INLINE const std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, gamma);
    }

  private:
    Pot m_du;
    math::scalar prev_r_dot = 0;
};

template <typename Pot>
struct State3D<false, true, Pot> : State3DBase, Quantized {
    static constexpr std::array<std::string_view, 10> field_names = {
        "t",         "r",          "r_dot", "r_ddot",  "theta",
        "theta_dot", "theta_ddot", "phi",   "phi_dot", "delta_psi"};

    template <typename Factory>
    State3D(const math::Vector3 &r0, const math::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dnr = eom::spherical::non_rel;
        m_theta0 = math::sin(theta);
        if (std::abs(m_theta0) <= 1e-6) {
            phi = math::HALF_PI;
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
          sign(other.sign), theta_flag(other.theta_flag),
          prev_max_vec(other.prev_max_vec) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
            m_theta0 = other.m_theta0;
            prev_r_dot = other.prev_r_dot;
            sign = other.sign;
            theta_flag = other.theta_flag;
            prev_max_vec = other.prev_max_vec;
        }
        return *this;
    }

    SIM_CONST SIM_INLINE bool iterate(math::scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= math::TWO_PI * static_cast<math::scalar>(phi >= math::TWO_PI);

        namespace eom_3dnr = eom::spherical::non_rel;
        if (std::abs(m_theta0) <= 1e-6) {
            theta_dot = sign * eom::polar::non_rel::compute_psi_dot(r, p_phi);

            // TODO: make this branchless
            if (theta >= math::PI && !theta_flag) {
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
        const bool isLocalMaxima = eom::isLocalMax(prev_r_dot, r_dot);
        if (isLocalMaxima) [[unlikely]] {
            math::Vector3 curr_max_vec =
                eom::sphericalToCartesian(r, theta, phi);

            if (prev_max_vec) {
                delta_psi +=
                    eom::computeAngularDistance(*prev_max_vec, curr_max_vec);
            }

            prev_max_vec = curr_max_vec;
        }

        return isLocalMaxima;
    }

    SIM_CONST SIM_INLINE std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, delta_psi);
    }

  private:
    Pot m_du;
    math::scalar m_theta0;
    math::scalar prev_r_dot = 0;

    math::scalar sign = 1;
    math::scalar theta_flag = false;

    std::optional<math::Vector3> prev_max_vec = std::nullopt;
};

template <typename Pot>
struct State3D<true, true, Pot> : State3DBase, Relativistic, Quantized {
    static constexpr std::array<std::string_view, 11> field_names = {
        "t",          "r",   "r_dot",   "r_ddot", "theta",    "theta_dot",
        "theta_ddot", "phi", "phi_dot", "gamma",  "delta_psi"};

    template <typename Factory>
    State3D(const math::Vector3 &r0, const math::Vector3 &v0, Factory &&factory)
        : State3DBase(r0, v0), m_du(factory(r)) {
        namespace eom_3dr = eom::spherical::rel;
        m_theta0 = math::sin(theta);

        if (std::abs(m_theta0) <= 1e-6) {
            sign = 1;
            phi = math::HALF_PI;
            phi_dot = 0;
            theta_ddot = 0;

            gamma = eom::polar::rel::compute_gamma(r, r_dot, p_phi);
            theta_dot = eom::polar::rel::compute_psi_dot(r, gamma, p_phi);
        } else {
            gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);
            phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
            theta_ddot = eom_3dr::compute_theta_ddot(
                r, r_dot, theta, theta_dot, phi_dot, gamma, m_du.dU_dr());
        }
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());
    }

    State3D(const State3D &other)
        : State3DBase(other), m_du(other.m_du.rebind(r)),
          prev_r_dot(other.prev_r_dot), m_theta0(other.m_theta0),
          sign(other.sign), theta_flag(other.theta_flag),
          prev_max_vec(other.prev_max_vec) {}

    State3D &operator=(const State3D &other) {
        if (this != &other) {
            State3DBase::operator=(other);
            m_du = other.m_du.rebind(r);
            m_theta0 = other.m_theta0;
            prev_r_dot = other.prev_r_dot;
            sign = other.sign;
            theta_flag = other.theta_flag;
            prev_max_vec = other.prev_max_vec;
        }

        return *this;
    }

    SIM_CONST SIM_CONST SIM_INLINE bool
    iterate(math::scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        theta_dot += theta_ddot * delta_time;
        theta += theta_dot * delta_time;
        phi += phi_dot * delta_time;
        phi -= math::TWO_PI * static_cast<math::scalar>(phi >= math::TWO_PI);

        namespace eom_3dr = eom::spherical::rel;

        if (std::abs(m_theta0) <= 1e-6) {
            gamma = eom::polar::rel::compute_gamma(r, r_dot, p_phi);
            theta_dot =
                sign * eom::polar::rel::compute_psi_dot(r, gamma, p_phi);

            // TODO: make this branchless
            if (theta >= math::PI && !theta_flag) {
                theta_flag = true;
                sign = -1;
                phi = -phi;
            } else if (theta <= 0 && theta_flag) {
                theta_flag = false;
                sign = 1;
                phi = -phi;
            }
        } else {
            gamma = eom_3dr::compute_gamma(r, r_dot, theta, theta_dot, p_phi);
            phi_dot = eom_3dr::compute_phi_dot(r, theta, gamma, p_phi);
            theta_ddot = eom_3dr::compute_theta_ddot(
                r, r_dot, theta, theta_dot, phi_dot, gamma, m_du.dU_dr());
        }
        r_ddot = eom_3dr::compute_r_ddot(r, r_dot, theta, theta_dot, phi_dot,
                                         gamma, m_du.dU_dr());

        const bool isLocalMaxima = eom::isLocalMax(prev_r_dot, r_dot);
        if (isLocalMaxima) [[unlikely]] { // TODO: try to make this branchless
            math::Vector3 curr_max_vec =
                eom::sphericalToCartesian(r, theta, phi);

            if (prev_max_vec) {
                delta_psi +=
                    eom::computeAngularDistance(*prev_max_vec, curr_max_vec);
            }

            prev_max_vec = curr_max_vec;
        }

        return isLocalMaxima;
    }

    SIM_CONST SIM_INLINE std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, theta, theta_dot,
                             theta_ddot, phi, phi_dot, gamma, delta_psi);
    }

  private:
    Pot m_du;
    math::scalar prev_r_dot = 0;
    math::scalar m_theta0;

    math::scalar sign = 1;
    math::scalar theta_flag = false;

    std::optional<math::Vector3> prev_max_vec = std::nullopt;
};
} // namespace physics::state
