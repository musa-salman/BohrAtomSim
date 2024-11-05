#include <Python.h>

#include <floatobject.h>
#include <listobject.h>
#include <pytypedefs.h>

#include "atom/result_recorders.h"
#include "utils/macros.h"

void record2py_list(void *record_in, const unsigned char orbit_i,
                    const unsigned char iter_i, const struct sim_itr *iter) {
    PyObject *result = PyList_GetItem(record_in, orbit_i);

    PyObject *record = NULL;
    if (THETA(iter) != -1)
        record = PyList_New(3); // Include 3D (theta) information.

    else
        record = PyList_New(2);

    if (!record) {
        char msg[100];
        snprintf(msg, sizeof(msg),
                 "Failed to create record list. orbit_i: %d, iter_i: %d",
                 orbit_i, iter_i);
        PyErr_SetString(PyExc_RuntimeError, msg);
        return;
    }

    // Add r value.
    PyObject *item = PyFloat_FromDouble(iter->r);
    if (!item) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to build r value.");
        Py_DECREF(record);
        return;
    }
    PyList_SET_ITEM(record, 0, item);

    // Add phi value.
    item = PyFloat_FromDouble(iter->phi);
    if (!item) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to build phi value.");
        Py_DECREF(record);
        return;
    }
    PyList_SET_ITEM(record, 1, item);

    // Conditionally add theta value if 3D.
    if (THETA(iter) != -1) {
        item = PyFloat_FromDouble(THETA(iter));
        if (!item) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to build theta value.");
            Py_DECREF(record);
            return;
        }
        PyList_SET_ITEM(record, 2, item);
    }

    // Set the record in the correct slot of the iteration list.
    PyList_SET_ITEM(result, iter_i, record);
}

void record2array(void *record_in, const unsigned char orbit_i,
                  const unsigned char iter_i, const struct sim_itr *iter) {
    double(*result)[5000][2] = record_in;

    unsigned char i = 0;

    result[orbit_i][iter_i][i++] = iter->r;
    result[orbit_i][iter_i][i++] = iter->phi;

    if (THETA(iter) != -1)
        result[orbit_i][iter_i][i++] = THETA(iter);
}

void record2printf(void *Py_UNUSED(record_in),
                   const unsigned char Py_UNUSED(orbit_i),
                   const unsigned char Py_UNUSED(iter_i),
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