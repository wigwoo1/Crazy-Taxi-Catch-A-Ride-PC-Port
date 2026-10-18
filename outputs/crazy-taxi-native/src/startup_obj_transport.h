#ifndef TAXI_STARTUP_OBJ_TRANSPORT_H
#define TAXI_STARTUP_OBJ_TRANSPORT_H
#include "startup_obj_queue.h"
typedef struct {uint8_t previous_total,matrix_count;TaxiObjMatrixSlot matrices[32];uint16_t output[128][4];} TaxiStartupObjTransport;
/* Whole27CD0 cold branch: caller has no loaded sprite resource. Clears
   base bucket/matrices and publishes disabled objects; retains other queues.
   Active-resource branch is a merge service and must not call this reset. */
int taxi_startup_obj_cold_clear(TaxiStartupObjQueue *,TaxiStartupObjTransport *);
/* Active27E68 publication with caller-resolved original reset templates.
   Mutates sorted bucket2/depth, publishes, then restores base/matrices. */
int taxi_startup_obj_publish(TaxiStartupObjQueue *,TaxiStartupObjTransport *,
 const uint16_t base_template[128][4],const TaxiObjMatrixSlot matrix_template[32]);
/* Intro-only affine producer: positive scale, zero angle, origin0, 8bpp.
   Pending whole28658 ROM qualification; scale256 delegates28288. */
int taxi_startup_obj_scaled_queue(TaxiStartupObjQueue *,TaxiStartupObjTransport *,
 const uint16_t metrics[9],int32_t x,int32_t y,int32_t z,uint16_t width,uint16_t height,
 uint8_t bucket,uint8_t priority,uint8_t flags,uint16_t mask,uint32_t tile_base,
 int16_t scale,TaxiStartupObjQueued *);
#endif
