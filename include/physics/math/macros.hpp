#pragma once

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define SIM_CONST __attribute__((const))
#define SIM_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define SIM_CONST
#define SIM_INLINE __forceinline
#else
#define SIM_CONST
#define SIM_INLINE inline
#endif

#define SQUARE(x) ((x) * (x))