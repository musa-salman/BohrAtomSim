import ctypes
from utils import wrap_function, BohrModule
from radial_limit import RadialBound


class PolarCalc:
    def __init__(self):
        self.mod = BohrModule.get_bohr_module()

        self.compute_phi_dot_fn = wrap_function(
            self.mod,
            "compute_phi_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_r_dot_dot_fn = wrap_function(
            self.mod,
            "compute_r_dot_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_radial_limits_fn = wrap_function(
            self.mod,
            "compute_radial_limits",
            ctypes.POINTER(RadialBound),
            [ctypes.c_longdouble, ctypes.c_longdouble],
        )

    def compute_phi_dot(self, l, mass, r):
        return self.compute_phi_dot_fn(l, mass, r)

    def compute_r_dot_dot(self, mass, r, charge, k):
        return self.compute_r_dot_dot_fn(mass, r, charge, k)

    def compute_radial_limits(self, principal, angular):
        return self.compute_radial_limits_fn(principal, angular)
