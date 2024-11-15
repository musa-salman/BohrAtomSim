#ifndef _TYPES_H
#define _TYPES_H

#define LONG_DOUBLE_SCALAR

#ifdef FLOAT_SCALAR
typedef float __scalar; // not recommended
#elif defined(LONG_DOUBLE_SCALAR)
typedef long double __scalar;
#else
typedef double __scalar;
#endif

#ifdef FLOAT_SCALAR
#define FLOAT_LITERAL_SUFFIX(x) x##f
#elif defined(LONG_DOUBLE_SCALAR)
#define FLOAT_LITERAL_SUFFIX(x) x##l
#else
#define FLOAT_LITERAL_SUFFIX(x) x
#endif


#endif // _TYPES_H