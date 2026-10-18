#ifndef TAXI_INTEGER_ROOT_H
#define TAXI_INTEGER_ROOT_H
#include <stdint.h>
/* Unsigned floor square root. Native functional replacement for the
   mathematical operation requested by ROM32038/SWI8; no BIOS execution,
   register-clobber or timing qualification. */
uint32_t taxi_integer_root(uint32_t value);
#endif
