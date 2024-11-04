#ifndef _TYPES_H
#define _TYPES_H

#ifdef LONG_DOUBLE_SCALAR
typedef long double __scalar;
#elif defined(DOUBLE_SCALAR)
typedef double __scalar;
#else
typedef float __scalar;
#endif

#ifdef LONG_DOUBLE_SCALAR
#define FLOAT_LITERAL_SUFFIX(x) x##l
#elif defined(DOUBLE_SCALAR)
#define FLOAT_LITERAL_SUFFIX(x) x
#else
#define FLOAT_LITERAL_SUFFIX(x) x##f
#endif


#endif // _TYPES_H