#ifndef STATE2D_HPP
#define STATE2D_HPP

#include <array>
#include <cmath>
#include <string_view>

#include "State.hpp"
#include "eom/polar.hpp"
#include "eom/polar_rel.hpp"
#include "eom/utils.hpp"

struct State2DBase : State {
    State2DBase(const eom::Vector2 &r0, const eom::Vector2 &v0)
        : r(eom::polar::compute_r_0(r0)),
          r_dot(eom::polar::compute_r_dot_0(r0, v0)), r_ddot(0),
          psi(eom::polar::compute_psi_0(r0)), psi_dot(0), gamma(0),
          p_psi(eom::polar::compute_p_psi(r0, v0)) {}

    template <typename EOM, typename Pot>
    SIM_INLINE inline void integrate_common(scalar delta_time,
                                            const Pot &m_du) {
        time += delta_time;
        r_dot += r_ddot * delta_time;
        r += r_dot * delta_time;
        psi += psi_dot * delta_time;
        psi -= TWO_PI * static_cast<scalar>(psi >= TWO_PI);

        EOM::update(*this, m_du);
    }

    scalar r, r_dot, r_ddot;
    scalar psi, psi_dot;

    scalar gamma;

    scalar p_psi;
};

template <bool IsRel, bool IsQuant, typename Pot> struct State2D {};

template <typename Pot> struct State2D<false, false, Pot> : State2DBase {
    static constexpr std::array<std::string_view, 6> field_names = {
        "t", "r", "r_dot", "r_ddot", "psi", "psi_dot"};

    template <typename Factory>
    State2D(eom::Vector2 r0, eom::Vector2 v0, Factory &&factory)
        : State2DBase(r0, v0), m_du(factory(r)), prev_r_dot(r_dot) {
        namespace eom_2dnr = eom::polar::non_rel;
        eom_2dnr::EOM::update(*this, m_du);
    }

    State2D(const State2D &other)
        : State2DBase(other), m_du(other.m_du.rebind(r)) {}

    State2D &operator=(const State2D &other) {
        if (this != &other) {
            State2DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        prev_r_dot = r_dot;
        integrate_common<eom::polar::non_rel::EOM>(delta_time, m_du);
        return eom::isLocalMax(prev_r_dot, r_dot);
    }

    SIM_CONST SIM_INLINE inline const std::array<double, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, psi, psi_dot);
    }

  private:
    Pot m_du;
    scalar prev_r_dot;
};

template <typename Pot>
struct State2D<false, true, Pot> : State2DBase, Quantized {
    static constexpr std::array<std::string_view, 7> field_names = {
        "t", "r", "r_dot", "r_ddot", "psi", "psi_dot", "delta_psi"};

    template <typename Factory>
    State2D(eom::Vector2 r0, eom::Vector2 v0, Factory &&factory)
        : State2DBase(r0, v0), m_du(factory(r)), prev_r_dot((r_dot)),
          prev_psi_at_r_max(0) {
        namespace eom_2dnr = eom::polar::non_rel;
        eom_2dnr::EOM::update(*this, m_du);
    }

    State2D(const State2D &other)
        : State2DBase(other), m_du(other.m_du.rebind(r)) {}

    State2D &operator=(const State2D &other) {
        if (this != &other) {
            State2DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        integrate_common<eom::polar::non_rel::EOM>(delta_time, m_du);

        const bool isLocalMax = eom::isLocalMax(prev_r_dot, r_dot);
        if (isLocalMax) {
            if (prev_psi_at_r_max > 1e-10) {
                delta_psi += psi - prev_psi_at_r_max;
            }
            prev_psi_at_r_max = psi;
        }

        return isLocalMax;
    }

    SIM_CONST SIM_INLINE inline const std::array<scalar, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, psi, psi_dot, delta_psi);
    }

  private:
    Pot m_du;
    scalar prev_r_dot;
    scalar prev_psi_at_r_max;
};

template <typename Pot>
struct State2D<true, false, Pot> : State2DBase, Relativistic {
    static constexpr std::array<std::string_view, 7> field_names = {
        "t", "r", "r_dot", "r_ddot", "psi", "psi_dot", "gamma"};

    template <typename Factory>
    State2D(eom::Vector2 r0, eom::Vector2 v0, Factory &&factory)
        : State2DBase(r0, v0), m_du(factory(r)) {
        eom::polar::rel::EOM::update(*this, m_du);
    }

    State2D(const State2D &other)
        : State2DBase(other), m_du(other.m_du.rebind(r)) {}

    State2D &operator=(const State2D &other) {
        if (this != &other) {
            State2DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    bool iterate(scalar delta_time) noexcept {
        prev_r_dot = r_dot;
        integrate_common<eom::polar::rel::EOM>(delta_time, m_du);
        return eom::isLocalMax(prev_r_dot, r_dot);
    }

    const std::array<scalar, field_names.size()> to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, psi, psi_dot, gamma);
    }

  private:
    Pot m_du;
    scalar prev_r_dot;
};

template <typename Pot>
struct State2D<true, true, Pot> : State2DBase, Relativistic, Quantized {
    static constexpr std::array<std::string_view, 8> field_names = {
        "t", "r", "r_dot", "r_ddot", "psi", "psi_dot", "gamma", "delta_psi"};

    template <typename Factory>
    State2D(eom::Vector2 r0, eom::Vector2 v0, Factory &&factory)
        : State2DBase(r0, v0), m_du(factory(r)), prev_r_dot(r_dot),
          prev_psi_at_r_max(0) {
        namespace eom_2dr = eom::polar::rel;
        eom_2dr::EOM::update(*this, m_du);
    }

    State2D(const State2D &other)
        : State2DBase(other), m_du(other.m_du.rebind(r)) {}

    State2D &operator=(const State2D &other) {
        if (this != &other) {
            State2DBase::operator=(other);
            m_du = other.m_du.rebind(r);
        }
        return *this;
    }

    SIM_CONST SIM_INLINE inline bool iterate(scalar delta_time) noexcept {
        prev_r_dot = r_dot;

        integrate_common<eom::polar::rel::EOM>(delta_time, m_du);

        const bool isLocalMaxima = eom::isLocalMax(prev_r_dot, r_dot);
        if (isLocalMaxima) { // TODO: try to make this branchless
            if (prev_psi_at_r_max > 1e-10) {
                delta_psi += psi - prev_psi_at_r_max;
            }
            prev_psi_at_r_max = psi;
        }

        return isLocalMaxima;
    }

    SIM_CONST SIM_INLINE inline const std::array<scalar, field_names.size()>
    to_array() const noexcept {
        return export_fields(time, r, r_dot, r_ddot, psi, psi_dot, gamma,
                             delta_psi);
    }

  private:
    Pot m_du;
    scalar prev_r_dot;
    scalar prev_psi_at_r_max;
};

#endif // STATE2D_HPP