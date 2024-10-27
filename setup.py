from setuptools import setup, Extension
import numpy

setup(
    name='bohr_model',
    version='1.0',
    author='Musa Salman',
    description='A simulation of the Bohr model of the atom, '
                'for experimentation and learning purposes.',
    ext_modules=[Extension('bohr_model',
                           ['src/bohr_model_module.c',
                            'src/polar/polar_sim.c',
                            'src/polar/polar_sim_rel.c',
                            'src/polar/polar_calc.c',
                            'src/polar/polar_calc_rel.c',
                            'src/spherical/spherical_sim.c',
                            'src/spherical/spherical_sim_rel.c',
                            'src/spherical/spherical_calc.c',
                            'src/spherical/spherical_calc_rel.c',
                            'src/utils/iterator.c'],
    include_dirs=['include', 'include/polar', 'include/spherical', 'include/utils',
                    numpy.get_include()],)],
)
