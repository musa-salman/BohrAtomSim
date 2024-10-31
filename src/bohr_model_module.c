#include <Python.h>

// This need to be defined as stated in
// https://github.com/numpy/numpy/issues/21865
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "listobject.h"
#include "numpy/npy_common.h"
#include <numpy/arrayobject.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulation_runner/simulation_runner.h"
#include "utils/iterator.h"

static struct electron_orbit *list2electron_orbits(PyObject *electrons) {
    struct electron_orbit *electron_orbits = NULL;
    Py_ssize_t electrons_count = PyList_Size(electrons);

    electron_orbits = malloc(sizeof(struct electron_orbit[electrons_count]));

    for (Py_ssize_t i = 0; i < electrons_count; i++) {
        PyObject *electron = PyList_GetItem(electrons, i);

        electron_orbits[i] = (struct electron_orbit){
            .principal =
                (unsigned char)PyLong_AsLong(PyTuple_GetItem(electron, 0)),
            .angular =
                (unsigned char)PyLong_AsLong(PyTuple_GetItem(electron, 1)),
            .magnetic =
                (unsigned char)PyLong_AsLong(PyTuple_GetItem(electron, 2)),
            .spin = (float)PyFloat_AsDouble(PyTuple_GetItem(electron, 3)),
        };
    }

    return electron_orbits;
}

static PyObject *simulate(PyObject *Py_UNUSED(self), PyObject *args) {
    import_array();

    PyObject *electrons = NULL;

    double revolutions;
    size_t max_iters;

    double time_interval;
    enum sim_type sim_type;
    bool delta_psi_mode;
    unsigned short record_interval;

    if (!PyArg_ParseTuple(args, "ddkBBHO!", &revolutions, &time_interval,
                          &max_iters, &sim_type, &delta_psi_mode,
                          &record_interval, &PyList_Type, &electrons)) {
        return NULL;
    }

    PyObject *result = NULL;
    const npy_intp dims[2] = {max_iters, 7};

    result = PyList_New(PyList_Size(electrons));

    for (Py_ssize_t i = 0; i < PyList_Size(electrons); i++) {
        PyList_SetItem(result, i, PyArray_SimpleNew(2, dims, NPY_DOUBLE));
    }

    struct record_handler rh = {
        .record_in = result,
        .record = &record2ndarray,
        .curr_records = 0,
    };

    struct atom atom = {
        .electrons = list2electron_orbits(electrons),
        .electrons_count = (unsigned char)PyList_Size(electrons),
    };

    struct sim_itr curr_itr = {0};
    struct sim_itr next_itr = {0};

    struct iter_ctx iter_ctx = {
        .prev_itr = &curr_itr,
        .next_itr = &next_itr,
        .orbit_i = 0,
        .electron_orbit = NULL,
    };

    struct sim_ctx ctx = {
        .atom = &atom,
        .revolutions = revolutions,
        .max_iters = max_iters,
        .time_interval = time_interval,
        .record_handler = &rh,
        .delta_psi_mode = delta_psi_mode,
        .record_interval = record_interval,
        .iter_ctx = &iter_ctx,
    };

    struct simulator sim;

    init_simulation(&sim, &ctx, sim_type);

    if (sim.simulate_orbit == NULL) {
        free(atom.electrons);
        PyErr_SetString(PyExc_ValueError, "Invalid simulation type.");
        return NULL;
    }

    run_simulation(&sim);

    free(atom.electrons);
    return result;
}

static PyMethodDef BohrModelMethods[] = {{.ml_name = "simulate",
                                          .ml_meth = simulate,
                                          .ml_flags = METH_VARARGS,
                                          .ml_doc = "Simulate Bohr's model."},
                                         {NULL, NULL, 0, NULL}};

static struct PyModuleDef bohr_model_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "bohr_model",
    .m_doc = "Bohr's model of the atom.",
    .m_size = -1,
    .m_methods = BohrModelMethods,
};

PyMODINIT_FUNC PyInit_bohr_model(void) {
    return PyModule_Create(&bohr_model_module);
}