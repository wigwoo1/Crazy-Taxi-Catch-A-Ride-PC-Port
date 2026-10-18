#ifndef CRAZY_TAXI_XBOX_H
#define CRAZY_TAXI_XBOX_H
#include <stdint.h>

typedef struct {
    int connected;
    uint16_t active_low_keys;
    int16_t steering;
    uint8_t accelerator;
    uint8_t brake;
    uint8_t look_back;
} TaxiXboxSample;

/* Native Windows XInput; no GBA hardware layer.
   Logical driving actions remain provisional until game consumers are recovered. */
TaxiXboxSample taxi_xbox_poll(unsigned controller_index);
/* Retain a connected controller; otherwise select the lowest connected slot. */
unsigned taxi_xbox_select_slot(unsigned connected_mask,unsigned current_slot);
TaxiXboxSample taxi_xbox_poll_any(void);
/* Pure mapping for a connected native XInput sample; no polling or effects. */
TaxiXboxSample taxi_xbox_decode(uint16_t buttons,int16_t lx,int16_t ly,uint8_t right_trigger,uint8_t left_trigger);
/* Native PC adaptation: XInput stick -> original command+12 halfword.
   Positive original steering matches digital left; full magnitude192*8=1536. */
int16_t taxi_xbox_steering_command(int16_t lx);
/* During the original pending gear/steering transition, use decoded digital
   direction bits so the native human steering branch can enter phase2. */
int16_t taxi_xbox_vehicle_steering_command(int16_t lx,uint8_t phase,int32_t phase_timer);
/* PC adaptation: hold right-stick click for recovered command5 rear bit. */
uint8_t taxi_xbox_camera_command(uint8_t original_command,uint8_t look_back);
/* Driving-only trigger adaptation; preserves menu button mapping. */
uint16_t taxi_xbox_driving_keys(TaxiXboxSample sample,uint8_t profile);
#endif



