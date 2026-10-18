#ifndef TAXI_PC_DISPLAY_SETTINGS_H
#define TAXI_PC_DISPLAY_SETTINGS_H
#include <stdint.h>

enum { TAXI_PC_DISPLAY_TEXTURES=0, TAXI_PC_DISPLAY_RESOLUTION=1,
       TAXI_PC_DISPLAY_FRAME_RATE=2, TAXI_PC_DISPLAY_WINDOW_MODE=3,
       TAXI_PC_DISPLAY_ASPECT_RATIO=4, TAXI_PC_DISPLAY_MENU_TEXT=5,
       TAXI_PC_DISPLAY_SETTING_COUNT=6 };
typedef struct {
    uint8_t textures;
    uint8_t resolution;
    uint8_t frame_rate;
    uint8_t window_mode;
    uint8_t aspect_ratio;
    uint8_t menu_text; /* Locked to Original; text is excluded from texture upscaling. */
} TaxiPcDisplaySettings;

/* PC-only choices.  These do not alter the ROM's Game Boy Advance settings. */
void taxi_pc_display_settings_defaults(TaxiPcDisplaySettings *settings);
int taxi_pc_display_settings_adjust(TaxiPcDisplaySettings *settings,
                                    unsigned row,int direction);
const char *taxi_pc_display_settings_value(const TaxiPcDisplaySettings *settings,
                                           unsigned row);
int taxi_pc_display_settings_dimensions(const TaxiPcDisplaySettings *settings,
                                        int *width,int *height);
int taxi_pc_display_settings_frame_rate(const TaxiPcDisplaySettings *settings,
                                        unsigned *frames_per_second);
#endif
