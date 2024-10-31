import ctypes


class RadialBound(ctypes.Structure):
    _fields_ = [
        ("r_min", ctypes.c_longdouble),
        ("r_max", ctypes.c_longdouble),
    ]

    def __init__(self, r_min, r_max):
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
        ("x", ctypes.c_longdouble),
        ("y", ctypes.c_longdouble),
        ("z", ctypes.c_longdouble),
    ]

    def __init__(self, x, y, z):
        super().__init__()
        self.x = x
        self.y = y
        self.z = z

    def __repr__(self):
        return f"Vector3({self.x}, {self.y}, {self.z})"
