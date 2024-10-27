#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <numpy/ndarrayobject.h>
#include <numpy/ndarraytypes.h>

#include "atom/result_recorders.h"
#include "utils/macros.h"
#include "utils/types.h"

void record2ndarray(void *record_in, const unsigned char orbit_i,
                    const unsigned char iter_i, const struct sim_itr *iter) {
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

void record2printf(void *Py_UNUSED(record_in),
                   const unsigned char Py_UNUSED(orbit_i),
                   const unsigned char Py_UNUSED(iter_i),
                   const struct sim_itr *iter) {

    printf("dt: %Lf\t", iter->dt);
    printf("r: %Lf\t", iter->r);
    printf("r_dot: %Lf\t", iter->r_dot);
    printf("r_dot_dot: %Lf\t", iter->r_dot_dot);
    printf("phi: %Lf\t", iter->phi);

    if (PHI_DOT_0(iter) != -1) {
        printf("phi_dot_0: %Lf\t", PHI_DOT_0(iter));
    }

    if (THETA(iter) != -1) {
        printf("theta: %Lf\t", THETA(iter));
    }

    if (THETA_DOT(iter) != -1) {
        printf("theta_dot: %Lf\t", THETA_DOT(iter));
    }

    if (THETA_DOT_DOT(iter) != -1) {
        printf("theta_dot_dot: %Lf\t", THETA_DOT_DOT(iter));
    }

    if (EPSILON(iter) != -1) {
        printf("epsilon: %Lf\t", EPSILON(iter));
    }

    if (GAMMA(iter) != -1) {
        printf("gamma: %Lf\t", GAMMA(iter));
    }

    if (DELTA_PHI(iter) != -1) {
        printf("delta_phi: %Lf\t", DELTA_PHI(iter));
    }

    printf("\n");
}