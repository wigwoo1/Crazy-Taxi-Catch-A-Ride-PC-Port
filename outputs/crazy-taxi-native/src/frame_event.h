#ifndef TAXI_FRAME_EVENT_H
#define TAXI_FRAME_EVENT_H
#include <stdint.h>
typedef struct {uint32_t counter_3520,scanline_4308,callback_352c;uint8_t preserve_1ea7,settings_1eb0;uint16_t acknowledge;} TaxiFrameEvent;
/* Original16174 logical event caller. Scanline is supplied source data,
   callbacks are native service boundaries, not ARM execution or IRQ emulation. */
typedef int (*TaxiFrameEventCall)(void *,TaxiFrameEvent *,uint32_t target,uint32_t argument);
/* Original161D8/161DC are bx-lr native no-op identities. */
int taxi_frame_event_callback_is_noop(uint32_t identity);
/* Original16158 setter: zero selects original161DD no-op identity. */
int taxi_frame_event_register(TaxiFrameEvent *,uint32_t identity);
int taxi_frame_event(TaxiFrameEvent *,uint16_t scanline,TaxiFrameEventCall,void *);
/* Native host already advanced3520 exactly once. Reject skipped/doubled ticks
   before mutation. Run the same software services without incrementing twice.
   No GBA VCOUNT exists on the PC: retain scanline_4308 as diagnostic observation
   rather than fabricating a hardware sample. Event calls own software state. */
int taxi_frame_event_after_counter(TaxiFrameEvent *,uint32_t counter,TaxiFrameEventCall,void *);
#endif
