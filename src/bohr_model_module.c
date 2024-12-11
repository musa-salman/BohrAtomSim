#include <Python.h>

#include <dictobject.h>
#include <listobject.h>
#include <pytypedefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulation_runner/simulation_runner.h"
#include "utils/iterator.h"
#include "utils/types.h"

static struct electron_orbit *list2electron_orbits(PyObject *electrons);
static PyObject *create_result_dict(struct atom atom);
void struct_to_dict(PyObject *dict, const struct result *result);
void *lookup_record(void *record_in, long key);

static PyObject *simulate(PyObject *Py_UNUSED(self), PyObject *args) {
    PyObject *electrons = NULL;

    double revolutions;

    const char *time_interval_str;
    scalar time_interval;
    enum sim_type sim_type;
    bool delta_psi_mode;
    unsigned short record_interval;

    if (!PyArg_ParseTuple(args, "dsBBHO!", &revolutions, &time_interval_str,
                          &sim_type, &delta_psi_mode, &record_interval,
                          &PyList_Type, &electrons)) {
        return NULL;
    }

    struct atom atom = {
        .electrons = list2electron_orbits(electrons),
        .electrons_count = (unsigned char)PyList_Size(electrons),
    };

    PyObject *result = create_result_dict(atom);

    record_fn record;
    record = NULL;

    struct record_handler rh = {
        .record_in = result,
        .curr_records = 0,
        .record = record,
        .records_lookup = &lookup_record,
    };

    time_interval = strtold(time_interval_str, NULL);

    struct sim_ctx ctx = {
        .revolutions = (float)revolutions,
        .time_interval = time_interval,
        .record_handler = &rh,
        .delta_psi_mode = delta_psi_mode,
        .record_interval = record_interval,
        .active_orbits = atom.electrons_count,
    };

    struct simulator sim;

    init_simulation(&sim, atom, &ctx, sim_type);

    if (sim.simulate_orbit == NULL) {
        free(atom.electrons);
        PyErr_SetString(PyExc_ValueError, "Invalid simulation type.");
        return NULL;
    }

    run_simulation(&sim);

    free(atom.electrons);

    return result;
}

void struct_to_dict(PyObject *dict, const struct result *result) {
    for (size_t i = 0; i < result->count; i++) {
        PyObject *record_list = PyDict_GetItem(dict, PyLong_FromLong(i));
        const struct records *records = &result->records[i];

        for (size_t j = 0; j < records->count; j++) {
            PyObject *record = PyList_New(0);

            PyObject *item = PyFloat_FromDouble(records->records[j].r);
            PyList_Append(record, item);
            Py_DECREF(item);

            item = PyFloat_FromDouble(records->records[j].phi);
            PyList_Append(record, item);
            Py_DECREF(item);

            if (records->records[j].theta != -1) {
                item = PyFloat_FromDouble(records->records[j].theta);
                PyList_Append(record, item);
                Py_DECREF(item);
            }

            PyList_Append(record_list, record);
        }
    }
}

long orbit_hash(struct electron_orbit orbit) {
    return orbit.principal | orbit.angular << 8 | orbit.magnetic << 16;
}

void *lookup_record(void *record_in, long key) {
    PyObject *orbit_i = PyLong_FromLong(key);
    void *r = PyDict_GetItem(record_in, orbit_i);
    Py_DECREF(orbit_i);
    return r;
}

static PyObject *create_result_dict(struct atom atom) {
    PyObject *result = PyDict_New();
    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create result dict.");
        return NULL;
    }

    for (unsigned char orbit_i = 0; orbit_i < atom.electrons_count; orbit_i++) {
        PyObject *record_list = PyList_New(0);
        if (!record_list) {
            Py_DECREF(result);
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to create record list.");
            return NULL;
        }

        PyObject *orbit_key =
            PyLong_FromLong(orbit_hash(atom.electrons[orbit_i]));
        PyDict_SetItem(result, orbit_key, record_list);
        Py_DECREF(orbit_key);
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