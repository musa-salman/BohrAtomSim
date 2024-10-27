import ctypes
from utils import wrap_function, BohrModule
from radial_limit import RadialBound


class PolarCalcRel:
    def __init__(self):
        self.mod = BohrModule.get_bohr_module()

        self.calc_rel_phi_dot_fn = wrap_function(
            self.mod,
            "calc_rel_phi_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.calc_rel_gamma_fn = wrap_function(
            self.mod,
            "calc_rel_gamma",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.calc_rel_r_dot_dot_fn = wrap_function(
            self.mod,
            "calc_rel_r_dot_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

    def calc_rel_phi_dot(self, l: float, gamma: float, r: float, mass: float) -> float:
        return self.calc_rel_phi_dot_fn(l, gamma, r, mass)

    def calc_rel_gamma(self, l: float, m: float, r: float, r_dot: float) -> float:
        return self.calc_rel_gamma_fn(l, m, r, r_dot)

    def calc_rel_r_dot_dot(self, l_sqr: float, m: float, gamma, r, e, r_dot) -> float:
        return self.calc_rel_r_dot_dot_fn(l_sqr, m, gamma, r, e, r_dot)
