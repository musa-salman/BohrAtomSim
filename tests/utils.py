import ctypes


def wrap_function(lib, fn_name, restype, argtypes):
    func = lib.__getattr__(fn_name)
    func.restype = restype
    func.argtypes = argtypes
    return func


class BohrModule:
    __mod = None
    __module_path = "lib/libbohr_simulation.so"

    def __init__(self, module_path=None):
        if BohrModule.__mod is not None:
            raise RuntimeError("BohrMod is a singleton class")
        if module_path is not None:
            BohrModule.__module_path = module_path
        BohrModule.__mod = ctypes.CDLL(BohrModule.__module_path)

    @staticmethod
    def get_bohr_module():
        if BohrModule.__mod is None:
            BohrModule.__mod = ctypes.CDLL(BohrModule.__module_path)
        return BohrModule.__mod

    @staticmethod
    def get_scalar():
        return ctypes.c_double
