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
