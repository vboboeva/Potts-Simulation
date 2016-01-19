#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef _FLOAT_PRECISION
typedef float __fpv; /*!< set the __fpv type to be float if the _FLOAT_PRECISION macro has been defined. */
#elif _DOUBLE_PRECISION
typedef double __fpv; /*!< set the __fpv type to be double if the _DOUBLE_PRECISION macro has been defined. */
#endif

#endif
