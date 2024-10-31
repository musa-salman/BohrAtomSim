import ctypes
from utils import wrap_function, BohrModule
from lib_structs import RadialBound, Vector3


class OrbitalMath:
    def __init__(self):
        self.mod = BohrModule.get_bohr_module()

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
            RadialBound,
            [ctypes.c_longdouble, ctypes.c_longdouble],
        )

        self.spherical_to_cartesian_fn = wrap_function(
            self.mod,
            "spherical_to_cartesian",
            ctypes.POINTER(Vector3),
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_angular_rate_fn = wrap_function(
            self.mod,
            "compute_angular_rate",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_gamma_fn = wrap_function(
            self.mod,
            "compute_gamma",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_rel_angular_rate_fn = wrap_function(
            self.mod,
            "compute_rel_angular_rate",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_rel_r_dot_dot_fn = wrap_function(
            self.mod,
            "compute_rel_r_dot_dot",
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

        self.compute_theta_min_fn = wrap_function(
            self.mod,
            "compute_theta_min",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_spherical_phi_dot_fn = wrap_function(
            self.mod,
            "compute_spherical_phi_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

        self.compute_angular_distance_fn = wrap_function(
            self.mod,
            "compute_angular_distance",
            ctypes.c_longdouble,
            [
                ctypes.POINTER(Vector3),
                ctypes.POINTER(Vector3),
            ],
        )

        self.compute_sphere_theta_dot_dot_fn = wrap_function(
            self.mod,
            "compute_sphere_theta_dot_dot",
            ctypes.c_longdouble,
            [
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
                ctypes.c_longdouble,
            ],
        )

    def compute_r_dot_dot(
        self, mass: float, radius: float, charge: float, angular: float
    ) -> float:
        return self.compute_r_dot_dot_fn(mass, radius, charge, angular)

    def compute_radial_limits(self, principal: float, angular: float) -> RadialBound:
        return self.compute_radial_limits_fn(principal, angular)

    def spherical_to_cartesian(
        self, radius: float, theta: float, phi: float
    ) -> Vector3:
        return self.spherical_to_cartesian_fn(radius, theta, phi).contents

    def compute_angular_rate(self, mass: float, radius: float, charge: float) -> float:
        return self.compute_angular_rate_fn(mass, radius, charge)

    def compute_gamma(
        self, angular: float, mass: float, radius: float, r_dot: float
    ) -> float:
        return self.compute_gamma_fn(angular, mass, radius, r_dot)

    def compute_rel_angular_rate(
        self, angular: float, mass: float, radius: float, gamma: float
    ) -> float:
        return self.compute_rel_angular_rate_fn(angular, mass, radius, gamma)

    def compute_rel_r_dot_dot(
        self,
        angular: float,
        mass: float,
        gamma: float,
        radius: float,
        charge: float,
        r_dot: float,
    ) -> float:
        return self.compute_rel_r_dot_dot_fn(
            angular, mass, gamma, radius, charge, r_dot
        )

    def compute_theta_min(self, n_phi: float, charge: float) -> float:
        return self.compute_theta_min_fn(n_phi, charge)

    def compute_spherical_phi_dot(
        self, n_phi: float, theta: float, mass: float, radius: float
    ) -> float:
        return self.compute_spherical_phi_dot_fn(n_phi, theta, mass, radius)

    def compute_angular_distance(
        self,
        v1: Vector3,
        v2: Vector3,
    ) -> float:
        return self.compute_angular_distance_fn(ctypes.byref(v1), ctypes.byref(v2))

    def compute_sphere_theta_dot_dot(
        self,
        radius: float,
        r_dot: float,
        theta: float,
        theta_dot: float,
        phi_dot: float,
    ) -> float:
        return self.compute_sphere_theta_dot_dot_fn(
            radius, r_dot, theta, theta_dot, phi_dot
        )
