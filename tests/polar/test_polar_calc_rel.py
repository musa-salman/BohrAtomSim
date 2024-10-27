import pytest
from polar_calc_rel import PolarCalcRel


@pytest.fixture
def polar_calc(scope="module"):
    return PolarCalcRel()


def test_calc_rel_phi_dot(polar_calc):
    result = polar_calc.calc_rel_phi_dot(
        1.05457266e-27,
        1.00002662532601624434,
        5.29177211e-09,
        9.109383e-28,
    )

    assert result == pytest.approx(41340275128904270.922276060)


def test_calc_rel_gamma(polar_calc):
    result = polar_calc.calc_rel_gamma(
        1.0545718e-27,
        9.109383e-28,
        5.29177210903e-09,
        0.0,
    )

    assert result == pytest.approx(1.000026625326016244300322376)


def test_calc_rel_r_dot_dot(polar_calc):
    result = polar_calc.calc_rel_r_dot_dot(
        1.0545718e-27 * 1.0545718e-27,
        1.00002662532601624434,
        9.109383e-28,
        5.29177210903e-09,
        4.8032068e-10,
        0.0,
    )

    assert result == pytest.approx(-248254499349412178719.8396427854363533222)
