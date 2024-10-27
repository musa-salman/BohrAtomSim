import pytest
from polar_calc import PolarCalc
from radial_limit import RadialBound


@pytest.fixture
def polar_calc(scope="module"):
    return PolarCalc()


def test_compute_radial_limits(polar_calc):
    result = polar_calc.compute_radial_limits(1, 1)

    assert isinstance(result.contents, RadialBound)


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
def test_compute_radial_limits(polar_calc, principal, angular, expected):
    result = polar_calc.compute_radial_limits(principal, angular)

    assert result.contents.get_r_min() == pytest.approx(expected.get_r_min(), abs=1e-9)
    assert result.contents.get_r_max() == pytest.approx(expected.get_r_max(), abs=1e-9)


@pytest.mark.parametrize(
    "l, mass, r, expected",
    [
        (
            1.05457266e-27,
            9.109383e-28,
            5.286575e-09,
            4.142273e16,
        ),
        (
            1.05457266000000002969e-27,
            9.10938299999999984642e-28,
            5.28657494726808941049e-09,
            4.142273e16,
        ),
        (
            2.10914532000000005939e-27,
            9.10938299999999984642e-28,
            2.11808533420276745622e-08,
            5.160962e15,
        ),
        (
            1.05457266000000002969e-27,
            9.10938299999999984642e-28,
            4.08954807832393067593e-08,
            6.922085e14,
        ),
    ],
)
def test_compute_phi_dot(polar_calc, l, mass, r, expected):
    result = polar_calc.compute_phi_dot(l, mass, r)

    assert result == pytest.approx(expected)


@pytest.mark.parametrize(
    "mass, r, charge, k, expected",
    [
        (
            9.10938299999999984642e-28,
            5.29177210903000035596e-09,
            4.80299999999999985432e-10,
            1,
            771311342549318697118.98,
        ),
    ],
)
def test_compute_r_dot_dot(polar_calc, mass, r, charge, k, expected):
    result = polar_calc.compute_r_dot_dot(mass, r, charge, k)

    assert result == pytest.approx(expected)
