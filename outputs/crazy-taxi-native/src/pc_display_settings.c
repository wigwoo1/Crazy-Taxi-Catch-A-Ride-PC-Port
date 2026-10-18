#include "pc_display_settings.h"

static const char *const texture_values[]={"Original","Linear","4K Test"};
static const char *const resolution_values[]={"720p","1080p","1440p","4K"};
static const char *const rate_values[]={"30 FPS","60 FPS","120 FPS","Unlimited"};
static const char *const window_values[]={"Windowed","Fullscreen"};
static const char *const aspect_values[]={"16:9","16:10","21:9","4:3"};
static const char *const menu_text_values[]={"Original"};
static const int resolution_heights[]={720,1080,1440,2160};
static const int aspect_numerators[]={16,16,21,4},aspect_denominators[]={9,10,9,3};
static const unsigned frame_rates[]={30,60,120,0};

void taxi_pc_display_settings_defaults(TaxiPcDisplaySettings *s){
    if(!s)return;
    *s=(TaxiPcDisplaySettings){.textures=0,.resolution=3,.frame_rate=1,
        .window_mode=0,.aspect_ratio=0,.menu_text=0};
}
static unsigned count(unsigned row){
    static const unsigned counts[TAXI_PC_DISPLAY_SETTING_COUNT]={3,4,4,2,4,1};
    return row<TAXI_PC_DISPLAY_SETTING_COUNT?counts[row]:0;
}
int taxi_pc_display_settings_adjust(TaxiPcDisplaySettings *s,unsigned row,int direction){
    if(!s||row>=TAXI_PC_DISPLAY_SETTING_COUNT||!direction)return -1;
    unsigned n=count(row);uint8_t *value=(uint8_t *)s+row;
    if(*value>=n)return -1;
    *value=(uint8_t)((*value+n+(direction>0?1u:n-1u))%n);
    return 0;
}
const char *taxi_pc_display_settings_value(const TaxiPcDisplaySettings *s,unsigned row){
    if(!s)return 0;
    const char *const *values=0;
    switch(row){
    case TAXI_PC_DISPLAY_TEXTURES:values=texture_values;break;
    case TAXI_PC_DISPLAY_RESOLUTION:values=resolution_values;break;
    case TAXI_PC_DISPLAY_FRAME_RATE:values=rate_values;break;
    case TAXI_PC_DISPLAY_WINDOW_MODE:values=window_values;break;
    case TAXI_PC_DISPLAY_ASPECT_RATIO:values=aspect_values;break;
    case TAXI_PC_DISPLAY_MENU_TEXT:values=menu_text_values;break;
    default:return 0;
    }
    return *(((const uint8_t *)s)+row)<count(row)?values[*(((const uint8_t *)s)+row)]:0;
}
int taxi_pc_display_settings_dimensions(const TaxiPcDisplaySettings *s,int *width,int *height){
    if(!s||!width||!height||s->resolution>=4||s->aspect_ratio>=4)return -1;
    int h=resolution_heights[s->resolution];
    int w=(int)(((int64_t)h*aspect_numerators[s->aspect_ratio]+aspect_denominators[s->aspect_ratio]/2)/aspect_denominators[s->aspect_ratio]);
    *width=w;*height=h;return 0;
}
int taxi_pc_display_settings_frame_rate(const TaxiPcDisplaySettings *s,unsigned *fps){
    if(!s||!fps||s->frame_rate>=4)return -1;
    *fps=frame_rates[s->frame_rate];return 0;
}
