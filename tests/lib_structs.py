import ctypes
from utils import BohrModule


class RadialBound(ctypes.Structure):
    _fields_ = [
        ("r_min", BohrModule.get_scalar()),
        ("r_max", BohrModule.get_scalar()),
    ]

    def __init__(self, r_min: float, r_max: float):
        super().__init__()
        self.r_min = r_min
        self.r_max = r_max

    def __repr__(self):
        return f"RadialBound({self.r_min}, {self.r_max})"

    def get_r_min(self):
        return self.r_min

    def get_r_max(self):
        return self.r_max


class Vector3(ctypes.Structure):
    _fields_ = [
        ("x", BohrModule.get_scalar()),
        ("y", BohrModule.get_scalar()),
        ("z", BohrModule.get_scalar()),
    ]

    def __init__(self, x, y, z):
        super().__init__()
        self.x = x
        self.y = y
        self.z = z

    def __repr__(self):
        return f"Vector3({self.x}, {self.y}, {self.z})"
