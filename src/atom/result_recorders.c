#include <Python.h>

#include <floatobject.h>
#include <listobject.h>
#include <pytypedefs.h>

#include "atom/result_recorders.h"
#include "utils/macros.h"

void record2py_list(void *record_in, const unsigned char orbit_i,
                    const struct sim_itr *iter) {
    PyObject *result = PyList_GetItem(record_in, orbit_i);
    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to retrieve result list.");
        return;
    }

    PyObject *record = PyList_New(0);
    if (!record) {
        char msg[100];
        snprintf(msg, sizeof(msg), "Failed to create record list. orbit_i: %d",
                 orbit_i);
        PyErr_SetString(PyExc_RuntimeError, msg);
        return;
    }

    // Add r value.
    PyObject *item = PyFloat_FromDouble(iter->r);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append r value.");
        Py_XDECREF(item);
        Py_DECREF(record);
        return;
    }
    Py_DECREF(item); // Release reference now that it is in the list

    // Add phi value.
    item = PyFloat_FromDouble(iter->phi);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append phi value.");
        Py_XDECREF(item);
        Py_DECREF(record);
        return;
    }
    Py_DECREF(item);

    // Conditionally add theta value if 3D.
    if (THETA(iter) != -1) {
        item = PyFloat_FromDouble(THETA(iter));
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append theta value.");
            Py_XDECREF(item);
            Py_DECREF(record);
            return;
        }
        Py_DECREF(item);
    }

    // Append `record` to `result` list at orbit_i.
    if (PyList_Append(result, record) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append record to result.");
        Py_DECREF(record);
        return;
    }

    Py_DECREF(record);
}

void record2printf(void *Py_UNUSED(record_in),
                   const unsigned char Py_UNUSED(orbit_i),
                   const struct sim_itr *iter) {

    print(iter->dt);

    print(R(iter));
    print(R_DOT(iter));
    print(R_DOT_DOT(iter));
    print(PHI(iter));

    if (PHI_DOT_0(iter) != -1) {
        print(PHI_DOT_0(iter));
    }

    if (THETA(iter) != -1) {
        print(THETA(iter));
    }

    if (THETA_DOT(iter) != -1) {
        print(THETA_DOT(iter));
    }

    if (THETA_DOT_DOT(iter) != -1) {
        print(THETA_DOT_DOT(iter));
    }

    if (EPSILON(iter) != -1) {
        print(EPSILON(iter));
    }

    if (GAMMA(iter) != -1) {
        print(GAMMA(iter));
    }

    if (DELTA_PHI(iter) != -1) {
        print(DELTA_PHI(iter));
    }

    printf("\n\n\n");
}