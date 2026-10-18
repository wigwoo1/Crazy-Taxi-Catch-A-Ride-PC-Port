#ifndef TAXI_TIP_HUD_H
#define TAXI_TIP_HUD_H
#include <stdint.h>
typedef struct {int32_t image_x,image_y,text_x,text_y;uint32_t image_address;uint8_t color;char text[32];} TaxiTipHudRequest;
/* Original2944C request projection; no GPU/pixel implementation implied. */
int taxi_tip_hud_requests(const uint8_t tips[10][56],uint16_t frame_5332,uint8_t paused,TaxiTipHudRequest out[10]);
#endif
