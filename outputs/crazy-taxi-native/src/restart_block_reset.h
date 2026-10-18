#ifndef TAXI_RESTART_BLOCK_RESET_H
#define TAXI_RESTART_BLOCK_RESET_H
#include <stdint.h>
/* DF8C->DF9C(-1): exact owners, later block-consumer semantics unresolved. */
typedef struct {int32_t word_1768;uint32_t word_176c;uint8_t block_2b8ec[512];} TaxiRestartBlockReset;
int taxi_restart_block_reset(TaxiRestartBlockReset *);
#endif
