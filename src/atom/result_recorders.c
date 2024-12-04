#include <Python.h>

#include <floatobject.h>
#include <listobject.h>
#include <pytypedefs.h>
#include <stdio.h>

#include "atom/result_recorders.h"
#include "object.h"
#include "utils/iterator.h"
#include "utils/macros.h"

void record2py_list(void *record_in, const struct sim_itr *iter) {
    PyObject *record = PyList_New(0);

    if (!record) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create record list.");
    }

    // Add time value.
    PyObject *item = PyFloat_FromDouble(iter->dt);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append time value.");
    }

    // Add r value.
    item = PyFloat_FromDouble(iter->r);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append r value.");
    }
    Py_DECREF(item);

    // Add phi value.
    item = PyFloat_FromDouble(iter->phi);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append phi value.");
    }
    Py_DECREF(item);

    // Conditionally add theta value if 3D.
    if (iter->theta != -1) {
        item = PyFloat_FromDouble(iter->theta);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append theta value.");
        }
    }

    // Conditionally add delta_phi value.
    if (iter->delta_phi != -1) {
        item = PyFloat_FromDouble(iter->delta_phi);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append delta_phi value.");
        }
    }

    if (PyList_Append(record_in, record) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append record to result.");
    }
}

void record2py_list_verbose(void *record_in, const struct sim_itr *iter) {
    PyObject *record = PyList_New(0);

    if (!record) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create record list.");
    }

    // Add time value.
    PyObject *item = PyFloat_FromDouble(iter->dt);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append time value.");
    }
    Py_DECREF(item);

    // Add r value.
    item = PyFloat_FromDouble(iter->r);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append r value.");
    }
    Py_DECREF(item);

    item = PyFloat_FromDouble(iter->r_dot);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append r_dot value.");
    }
    Py_DECREF(item);

    item = PyFloat_FromDouble(iter->r_dot_dot);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append r_dot_dot value.");
    }
    Py_DECREF(item);

    // Add phi value.
    item = PyFloat_FromDouble(iter->phi);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append phi value.");
    }
    Py_DECREF(item);

    item = PyFloat_FromDouble(iter->phi_dot);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to append phi_dot value.");
    }
    Py_DECREF(item);

    // Conditionally add theta value if 3D.
    if (iter->theta != -1) {
        item = PyFloat_FromDouble(iter->theta);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append theta value.");
        }
        Py_DECREF(item);
    }

    // Conditionally add theta_dot value if 3D.
    if (iter->theta_dot != -1) {
        item = PyFloat_FromDouble(iter->theta_dot);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append theta_dot value.");
        }
        Py_DECREF(item);
    }

    // Conditionally add theta_dot_dot value if 3D.
    if (iter->theta_dot_dot != -1) {
        item = PyFloat_FromDouble(iter->theta_dot_dot);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append theta_dot_dot value.");
        }
        Py_DECREF(item);
    }

    // Conditionally add epsilon value if spin.
    if (iter->epsilon != -1) {
        item = PyFloat_FromDouble(iter->epsilon);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append epsilon value.");
        }
        Py_DECREF(item);
    }

    // Conditionally add gamma value if rel.
    if (iter->gamma != -1) {
        item = PyFloat_FromDouble(iter->gamma);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append gamma value.");
        }
        Py_DECREF(item);
    }

    // Conditionally add delta_phi value if rel.
    if (iter->delta_phi != -1) {
        item = PyFloat_FromDouble(iter->delta_phi);
        if (!item || PyList_Append(record, item) == -1) {
            PyErr_SetString(PyExc_RuntimeError,
                            "Failed to append delta_phi value.");
        }
    }

    if (PyList_Append(record_in, record) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append record to result.");
    }
}

void record2py_list_rel(void *record_in, const struct sim_itr *iter) {
    PyObject *record = PyList_New(0);

    if (!record) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create record list.");
    }

    // Add delta_phi value.
    PyObject *item = PyFloat_FromDouble(iter->delta_phi);
    if (!item || PyList_Append(record, item) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append delta_phi value.");
        Py_XDECREF(item);
        Py_DECREF(record);
        return;
    }
    Py_DECREF(item);

    if (PyList_Append(record_in, record) == -1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Failed to append record to result.");
        Py_DECREF(record);
        return;
    }

    Py_DECREF(record);
}

void record2printf(void *Py_UNUSED(record_in), const struct sim_itr *iter) {

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

void record2csv(void *record_in, const struct sim_itr *iter) {
    FILE *csv = (FILE *)record_in;

    fprintf(csv, "%.20LE,%.20LE,%.20LE,%.20LE,%.20LE,%.20LE,%.20LE", iter->dt,
            R(iter), R_DOT(iter), R_DOT_DOT(iter), PHI(iter), PHI_DOT(iter),
            iter->phi_dot_dot);

    if (THETA(iter) != -1) {
        fprintf(csv, ",%.20LE,%.20LE,%.20LE", THETA(iter), THETA_DOT(iter),
                THETA_DOT_DOT(iter));
    }

    if (EPSILON(iter) != -1) {
        fprintf(csv, ",%.20LE", EPSILON(iter));
    }

    if (GAMMA(iter) != -1) {
        fprintf(csv, ",%.20LE,%.20LE", GAMMA(iter), DELTA_PHI(iter));
    }

    fprintf(csv, "\n");
}