#include <Python.h>

// as stated in https://github.com/numpy/numpy/issues/21865
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "listobject.h"
#include "numpy/npy_common.h"
#include <numpy/arrayobject.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "polar/polar_sim.h"
#include "polar/polar_sim_rel.h"
#include "spherical/spherical_sim.h"
#include "spherical/spherical_sim_rel.h"

#include "utils/iterator.h"
#include "utils/macros.h"
#include "utils/types.h"

static void record2ndarray(void *record_in, const unsigned char orbit_i,
                           const unsigned char iter_i,
                           const struct sim_itr *iter) {
    PyArrayObject *result =
        (PyArrayObject *)((PyListObject *)record_in)->ob_item[orbit_i];
    unsigned char i = 0;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->dt;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->r;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->r_dot;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->r_dot_dot;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->phi;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)iter->phi_dot;

    if (PHI_DOT_0(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) =
            (double)PHI_DOT_0(iter);
    }

    if (THETA(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)THETA(iter);
    }

    if (THETA_DOT(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) =
            (double)THETA_DOT(iter);
    }

    if (THETA_DOT_DOT(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) =
            (double)THETA_DOT_DOT(iter);
    }

    if (EPSILON(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)EPSILON(iter);
    }

    if (GAMMA(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = (double)GAMMA(iter);
    }

    if (DELTA_PHI(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) =
            (double)DELTA_PHI(iter);
    }
}

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
    double electron_charge;
    double electron_mass;

    double revolutions;
    size_t max_iters;

    double time_interval;
    enum sim_type sim_type;
    bool delta_psi_mode;
    unsigned char record_interval;

    // "dddkBBBBO!" means:
    if (!PyArg_ParseTuple(args, "ddddkBBBO!", &electron_charge, &electron_mass,
                          &revolutions, &time_interval, &max_iters, &sim_type,
                          &delta_psi_mode, &record_interval, &PyList_Type,
                          &electrons)) {
        return NULL;
    }

    PyObject *result = NULL;
    const npy_intp dims[2] = {max_iters, 6};

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
        .electron_charge = electron_charge,
        .electron_mass = electron_mass,
        .electrons = list2electron_orbits(electrons),
        .electrons_count = (unsigned char)PyList_Size(electrons),
    };

    struct sim_itr curr_itr = {0};
    struct sim_itr next_itr = {0};

    struct iter_ctx iter_ctx = {
        .curr_itr = &curr_itr,
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

    switch (sim_type) {
    case POLAR:
        polar_sim_ele(&ctx);
        break;
    case REL_POLAR:
        polar_sim_rel_ele(&ctx);
        break;
    case SPHERICAL:
        spherical_sim_ele(&ctx);
        break;
    case REL_SPHERICAL:
        spherical_sim_rel_ele(&ctx);
        break;
    case SPIN:
        break;
    case REL_SPIN:
        break;
    }

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