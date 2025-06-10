#pragma once

#include "_types.h"

namespace physics::math {

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

} // namespace physics::math