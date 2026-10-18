#ifndef TAXI_DESTINATION_H
#define TAXI_DESTINATION_H
#include <stdint.h>
/* Scalar predicate 0800e0f4. The selected record is 48 original bytes.
   A null record or selected index -1 means no destination. Invalid slots
   contain 0xffff. Input is a full 32-bit material value, not narrowed. */
int taxi_destination_accepts(int32_t selected_index, const uint8_t *record,
                             uint32_t material);
#endif

