#ifdef __cplusplus
extern "C" {
#endif

#ifndef TYPES_H
#define TYPES_H

#include "_types.h"

/**
 * @brief This header file defines the scalar type for mathematical operations.
 *
 * The scalar type is used for all floating-point calculations in the orbital
 * math functions.
 *
 * It meant to make it easier to change the precision of the calculations.
 */
typedef __scalar scalar;

typedef unsigned char quantum_principle;
typedef unsigned char quantum_angular;
typedef unsigned char quantum_magnetic;
typedef signed char quantum_spin;

#endif // TYPES_H

#ifdef __cplusplus
}
#endif