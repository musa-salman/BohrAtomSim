import pytest
from orbital_math import OrbitalMath
from lib_structs import RadialBound, Vector3


@pytest.fixture(scope="module")
def orbital_math():
    return OrbitalMath()


@pytest.mark.parametrize(
    "mass, radius, charge, angular, expected",
    [
        (
            9.10938299999999984642e-28,
            5.29177210903000035596e-09,
            4.80299999999999985432e-10,
            1,
            771311342549318697118.9800056,
        ),  # 771311342549168713088.0 My result
    ],
)
def test_compute_r_dot_dot(orbital_math, mass, radius, charge, angular, expected):
    result = orbital_math.compute_r_dot_dot(mass, radius, charge, angular)

    assert result == pytest.approx(expected)


def test_compute_radial_limits(orbital_math):
    result = orbital_math.compute_radial_limits(1, 1)

    assert isinstance(result, RadialBound)


@pytest.mark.parametrize(
    "principal, angular, expected",
    [
        (1, 1, RadialBound(5.297721e-09, 5.297721e-09)),
        (2, 2, RadialBound(2.119088e-08, 2.119088e-08)),
        (3, 1, RadialBound(2.726836e-09, 9.263214e-08)),
        (3, 2, RadialBound(1.214130e-08, 8.321768e-08)),
        (3, 3, RadialBound(4.767949e-08, 4.767949e-08)),
    ],
)
def test_compute_radial_limits(orbital_math, principal, angular, expected):
    result = orbital_math.compute_radial_limits(principal, angular)

    assert result.get_r_max() == pytest.approx(expected.get_r_max(), rel=1e-2)
    assert result.get_r_min() == pytest.approx(expected.get_r_min(), rel=1e-2)


@pytest.mark.parametrize(
    "angular, mass, radius, expected",
    [
        (
            1,
            9.109383e-28,
            5.286575e-09,
            4.142273e16,
        ),
        (
            1,
            9.10938299999999984642e-28,
            5.28657494726808941049e-09,
            4.142273e16,
        ),
        (
            2,
            9.10938299999999984642e-28,
            2.11808533420276745622e-08,
            5.160962e15,
        ),
        (
            1,
            9.10938299999999984642e-28,
            4.08954807832393067593e-08,
            6.922085e14,
        ),
    ],
)
def test_compute_angular_rate(orbital_math, angular, mass, radius, expected):
    result = orbital_math.compute_angular_rate(angular, mass, radius)
    assert result == pytest.approx(expected)


def test_compute_gamma(orbital_math):
    result = orbital_math.compute_gamma(
        1,
        9.1093829999999998e-28,
        5.2977210902999997e-09,
        0.0,
    )

    assert result == pytest.approx(1.000026625326016244300322376)


def test_compute_rel_angular_rate(orbital_math):
    result = orbital_math.compute_rel_angular_rate(
        1,
        9.109383e-28,
        5.29177211e-09,
        1.00002662532601624434,
    )

    assert result == pytest.approx(41340275128904270.922276060)


def test_compute_rel_r_dot_dot(orbital_math):
    result = orbital_math.compute_rel_r_dot_dot(
        1,
        9.109383e-28,
        1.00002662532601624434,
        5.29177210903e-09,
        4.8032068e-10,
        0.0,
    )

    assert result == pytest.approx(-248254499349412178719.8396427854363533222)


def test_compute_spherical_phi_dot(orbital_math):
    assert orbital_math.compute_spherical_phi_dot(
        1,
        1.57079632679489661926,
        9.10938299999999984642e-28,
        5.29177210903000035596e-09,
    ) == pytest.approx(
        41341375827207185.15807488
    )  # 41341375827207179.2383 My result


def test_compute_theta_min(orbital_math):
    assert orbital_math.compute_theta_min(1, 1) == pytest.approx(1.57079632679489661926)


def test_spherical_to_cartesian(orbital_math):
    result = orbital_math.spherical_to_cartesian(
        5.2977210902999997e-09, 1.5707963267948966, 0.00016741284749282698
    )

    assert result.x == pytest.approx(5.2977210318867481e-09)
    assert result.y == pytest.approx(8.8690657145659307e-13)
    assert result.z == pytest.approx(3.243918597696612e-25)


def test_compute_angular_distance(orbital_math):
    assert orbital_math.compute_angular_distance(
        Vector3(5.2977210318867481e-09, 8.8690657145659307e-13, 3.243918597696612e-25),
        Vector3(5.2977208249929289e-09, 1.7738127851409232e-12, 3.2439186073868745e-25),
    ) == pytest.approx(0.00016741278286186293)


def test_sphere_theta_dot_dot(orbital_math):
    assert orbital_math.compute_sphere_theta_dot_dot(
        5.2977210902999997e-09, 0, 1.5707963267948966, 0, 41248614476718784
    ) == pytest.approx(1.041836543725672e17)
