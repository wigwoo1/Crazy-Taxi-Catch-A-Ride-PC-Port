#ifndef TAXI_AI_COMMAND_H
#define TAXI_AI_COMMAND_H
#include <stdint.h>
/* Source554c..5612 only: command reset and target-heading/speed decisions.
   Caller must recover target path and subsequent AI side effects separately. */
uint8_t taxi_ai_command_prefix(uint8_t previous,int32_t speed,int32_t heading_error);
/* Original ARM52b4 via Thumb2dd5c; half-turn ties choose wrapped route. */
int32_t taxi_ai_angle_difference(uint32_t from,uint32_t to);
int32_t taxi_ai_vector_angle(int32_t x,int32_t z);
/* Source5614..5668: phase words378, caller-owned03000000 counter.
   Returns1 and wrapped target query only when original separation>384. */
int taxi_ai_reposition_query(uint32_t player_phase,uint32_t vehicle_phase,
    uint32_t *counter,uint16_t *query);
#endif
