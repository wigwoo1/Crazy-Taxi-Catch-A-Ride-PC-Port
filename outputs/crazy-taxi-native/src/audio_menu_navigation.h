#ifndef TAXI_AUDIO_MENU_NAVIGATION_H
#define TAXI_AUDIO_MENU_NAVIGATION_H
#include <stdint.h>
/* Source1c0d8..110/224..240, scoped to audio pointer02003887 which has
   none of the profile/locked-city row skips in this shared routine.
   row_count is original caller byte, not an invented fixed menu size. */
typedef struct {uint8_t row;int sound_index;} TaxiAudioMenuNavigation;
TaxiAudioMenuNavigation taxi_audio_menu_navigation(uint8_t row,uint8_t row_count,
    uint8_t up,uint8_t down,uint8_t exiting,uint8_t fade_active,
    uint8_t physical_gate,uint8_t sound_enabled);
typedef struct {uint8_t exiting;uint16_t fade_flags;int sound_index;} TaxiAudioMenuExit;
/* Source1c6e0..87c scoped audio menu. Confirm rows0..3 remains in menu;
   signed row>3 confirms. B cancels; successful confirm starts fade before B. */
TaxiAudioMenuExit taxi_audio_menu_exit(uint8_t row,uint8_t exiting,uint8_t fade,
    uint8_t gate,uint8_t confirm,uint8_t back,uint8_t enabled);
#endif
