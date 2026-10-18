#ifndef TAXI_OBJ_QUEUE_OWNED_H
#define TAXI_OBJ_QUEUE_OWNED_H
#include "title_obj_queue.h"
#include <stddef.h>
typedef struct {const uint8_t *rom;size_t rom_size;uint32_t resource;const uint32_t *group_bases;const int32_t *palette_bases;size_t base_count;TaxiStartupObjQueue *queue;uint16_t width,height,object_mask;} TaxiObjQueueOwner;
/* Original28288 origin0, resource metadata + shared native queue owners.
   Original byte frame/group/flags/palette and signed total gate.
   Returns1 queued,0 disabled/clipped,-1 invalid owner or metadata range.
   Original28288 trusts frame/group; native range errors are host validation. */
int taxi_obj_queue_owned(TaxiObjQueueOwner *,uint32_t frame,uint32_t group,const uint32_t position[3],uint32_t priority,uint32_t bucket,uint32_t flags,uint32_t palette,TaxiStartupObjQueued *);
#endif
