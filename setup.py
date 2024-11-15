from setuptools import setup, Extension
import numpy

setup(
    name="bohr_model",
    version="1.0",
    author="Musa Salman",
    description="A simulation of the Bohr model of the atom, "
    "for experimentation and learning purposes.",
    ext_modules=[
        Extension(
            "bohr_model",
            [
                "src/bohr_model_module.c",
                "src/polar/polar_sim.c",
                "src/polar/polar_sim_rel.c",
                "src/spherical/spherical_sim.c",
                "src/spherical/spherical_sim_rel.c",
                "src/utils/iterator.c",
                "src/atom/result_recorders.c",
                "src/simulation_runner/simulation_runner.c",
                "src/orbital_math.c",
                "src/result/result.c",
            ],
            include_dirs=[
                "include",
                "include/polar",
                "include/spherical",
                "include/utils",
                "include/simulation_runner",
                "include/atom",
                "include/result",
                numpy.get_include(),
            ],
        )
    ],
)
