#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <numpy/ndarrayobject.h>
#include <numpy/ndarraytypes.h>

#include "atom/result_recorders.h"
#include "utils/macros.h"

void record2ndarray(void *record_in, const unsigned char orbit_i,
                    const unsigned char iter_i, const struct sim_itr *iter) {
    PyArrayObject *result =
        (PyArrayObject *)((PyListObject *)record_in)->ob_item[orbit_i];
    unsigned char i = 0;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->dt;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->r;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->r_dot;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->r_dot_dot;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->phi;

    *(double *)PyArray_GETPTR2(result, iter_i, i++) = iter->phi_dot;

    if (PHI_DOT_0(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = PHI_DOT_0(iter);
    }

    if (THETA(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = THETA(iter);
    }

    if (THETA_DOT(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = THETA_DOT(iter);
    }

    if (THETA_DOT_DOT(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = THETA_DOT_DOT(iter);
    }

    if (EPSILON(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = EPSILON(iter);
    }

    if (GAMMA(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = GAMMA(iter);
    }

    if (DELTA_PHI(iter) != -1) {
        *(double *)PyArray_GETPTR2(result, iter_i, i++) = DELTA_PHI(iter);
    }
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