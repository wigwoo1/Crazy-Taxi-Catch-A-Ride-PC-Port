#ifndef TAXI_NONCUSTOMER_INIT_H
#define TAXI_NONCUSTOMER_INIT_H
#include <stdint.h>
/* Original1E9D0 resolved-ground path. Retains all unwritten actor bytes.
   Caller supplies source bitmap identity from087EC068 table and groundY;
   this routine owns no random state. Invalid source kinds are rejected. */
int taxi_noncustomer_init(uint8_t actor[64],const uint8_t source[32],uint32_t bitmap,int32_t ground_y);
#endif
