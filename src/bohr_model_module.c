#include <Python.h>

#include <listobject.h>
#include <stdlib.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulation_runner/simulation_runner.h"
#include "utils/iterator.h"
#include "utils/types.h"

static struct electron_orbit *list2electron_orbits(PyObject *electrons);
static PyObject *create_result_list(size_t num_orbits);

static PyObject *simulate(PyObject *Py_UNUSED(self), PyObject *args) {
    PyObject *electrons = NULL;

    double revolutions;

    double time_interval;
    enum sim_type sim_type;
    bool delta_psi_mode;
    unsigned short record_interval;

    if (!PyArg_ParseTuple(args, "ddBBHO!", &revolutions, &time_interval,
                          &sim_type, &delta_psi_mode, &record_interval,
                          &PyList_Type, &electrons)) {
        return NULL;
    }

    PyObject *result = create_result_list(PyList_Size(electrons));

    struct record_handler rh = {
        .record_in = result,
        .record = &record2py_list,
        .curr_records = 0,
    };

    struct atom atom = {
        .electrons = list2electron_orbits(electrons),
        .electrons_count = (unsigned char)PyList_Size(electrons),
    };

    struct sim_itr curr_itr = {};
    struct sim_itr next_itr = {};

    struct iter_ctx iter_ctx = {
        .prev_itr = &curr_itr,
        .next_itr = &next_itr,
        .orbit_i = 0,
        .electron_orbit = NULL,
    };

    struct sim_ctx ctx = {
        .atom = &atom,
        .revolutions = (float)revolutions,
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

static PyObject *create_result_list(size_t num_orbits) {
    PyObject *result = PyList_New(num_orbits);
    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create result list.");
        return NULL;
    }

    for (size_t orbit_i = 0; orbit_i < num_orbits; orbit_i++) {
        PyObject *iteration_list = PyList_New(0);
        if (!iteration_list) {
            Py_DECREF(result);
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to create iteration list.");
            return NULL;
        }

        PyList_SET_ITEM(result, orbit_i, iteration_list); // Owned reference.
    }

    return result;
}

static struct electron_orbit *list2electron_orbits(PyObject *electrons) {
    struct electron_orbit *electron_orbits = NULL;
    Py_ssize_t electrons_count = PyList_Size(electrons);

    electron_orbits = malloc(sizeof(struct electron_orbit[electrons_count]));

    for (Py_ssize_t i = 0; i < electrons_count; i++) {
        PyObject *electron = PyList_GetItem(electrons, i);

        electron_orbits[i] = (struct electron_orbit){
            (quantum_principle)PyLong_AsLong(PyTuple_GetItem(electron, 0)),
            (quantum_angular)PyLong_AsLong(PyTuple_GetItem(electron, 1)),
            (quantum_magnetic)PyLong_AsLong(PyTuple_GetItem(electron, 2)),
            (quantum_spin)PyFloat_AsDouble(PyTuple_GetItem(electron, 3)),
        };
    }

    return electron_orbits;
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