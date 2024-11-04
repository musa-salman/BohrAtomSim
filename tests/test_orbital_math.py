import pytest
from orbital_math import OrbitalMath
from lib_structs import RadialBound, Vector3


@pytest.fixture(scope="module")
def orbital_math():
    return OrbitalMath()


@pytest.mark.parametrize(
    "radius, angular, expected",
    [
        (
            1,
            1,
            0,
        ),
    ],
)
def test_compute_r_dot_dot(orbital_math, radius, angular, expected):
    result = orbital_math.compute_r_dot_dot(radius, angular)

    assert result == pytest.approx(expected)


def test_compute_radial_limits(orbital_math):
    result = orbital_math.compute_radial_limits(1, 1)

    assert isinstance(result, RadialBound)


@pytest.mark.parametrize(
    "principal, angular, expected",
    [
        (1, 1, RadialBound(1, 1)),
        (2, 2, RadialBound(4, 4)),
        (3, 1, RadialBound(0.51471862576142970718986, 17.485281374238570292810)),
        (3, 2, RadialBound(2.2917960675006309107, 15.708203932499369089227)),
        (3, 3, RadialBound(9, 9)),
    ],
)
def test_compute_radial_limits(orbital_math, principal, angular, expected):
    result = orbital_math.compute_radial_limits(principal, angular)

    assert result.get_r_min() == pytest.approx(expected.get_r_min())
    assert result.get_r_max() == pytest.approx(expected.get_r_max())


@pytest.mark.parametrize(
    "angular, radius, expected",
    [
        (
            1,
            1,
            1,
        ),
        (
            2,
            2,
            0.5,
        ),
        (
            1,
            4,
            0.0625,
        ),
    ],
)
def test_compute_angular_rate(orbital_math, angular, radius, expected):
    result = orbital_math.compute_angular_rate(angular, radius)
    assert result == pytest.approx(expected)


def test_compute_gamma(orbital_math):
    result = orbital_math.compute_gamma(
        1,
        1,
        0.0,
    )

    assert result == pytest.approx(1.000026625326016244300322376)


def test_compute_rel_r_dot_dot(orbital_math):
    result = orbital_math.compute_rel_r_dot_dot(
        1,
        1.00002662532601624434,
        1,
        0.0,
    )

    assert result == pytest.approx(-0.000026623908256812629993233678)


def test_compute_spherical_phi_dot(orbital_math):
    assert orbital_math.compute_spherical_phi_dot(
        1, 1.57079632679489661926, 1
    ) == pytest.approx(1)


def test_compute_theta_min(orbital_math):
    assert orbital_math.compute_theta_min(1, 1) == pytest.approx(1.57079632679489661926)


def test_spherical_to_cartesian(orbital_math):
    result = orbital_math.spherical_to_cartesian(
        1.00000000016729678482, 1.57079632679489661926, 0.000168986839881015949499
    )

    assert result.x == pytest.approx(0.999999985889020789908)
    assert result.y == pytest.approx(0.000168986839105006657273)
    assert result.z == pytest.approx(-2.5082788067537929915e-20)


def test_compute_angular_distance(orbital_math):
    assert orbital_math.compute_angular_distance(
        Vector3(
            0.999999985889020789908,
            0.000168986839105006657273,
            -2.5082788067537929915e-20,
        ),
        Vector3(
            0.999999943895420572511,
            0.000335973726719336467734,
            -2.50827880717342272898e-20,
        ),
    ) == pytest.approx(0.000166986893046271759506)


def test_sphere_theta_dot_dot(orbital_math):
    assert orbital_math.compute_sphere_theta_dot_dot(
        1, 0, 1.5707963267948966, 0, 0.999973416
    ) == pytest.approx(6.122908439959025e-17)


def test_compute_sphere_rel_theta_dot_dot(orbital_math):
    assert orbital_math.compute_sphere_rel_theta_dot_dot(
        1,
        0,
        1.5707963267948966,
        0,
        0.999973416,
        1.0000265656068432,
    ) == pytest.approx(6.1229084399590256672350814644e-17)
