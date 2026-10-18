#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pc_display_settings.h"
#include <assert.h>
#include <string.h>
int main(void){
    TaxiPcDisplaySettings s;taxi_pc_display_settings_defaults(&s);
    assert(s.textures==0&&s.resolution==3&&s.frame_rate==1&&s.window_mode==0&&s.aspect_ratio==0&&s.menu_text==0);
    assert(!strcmp(taxi_pc_display_settings_value(&s,0),"Original"));
    assert(!taxi_pc_display_settings_adjust(&s,0,1));
    assert(!strcmp(taxi_pc_display_settings_value(&s,0),"Linear"));
    assert(!taxi_pc_display_settings_adjust(&s,0,1));
    assert(s.textures==2&&!strcmp(taxi_pc_display_settings_value(&s,0),"4K Test"));
    assert(!taxi_pc_display_settings_adjust(&s,0,1));
    assert(s.textures==0&&!strcmp(taxi_pc_display_settings_value(&s,0),"Original"));
    assert(!taxi_pc_display_settings_adjust(&s,0,1));
    assert(!taxi_pc_display_settings_adjust(&s,0,-1));
    assert(!taxi_pc_display_settings_adjust(&s,0,-1));
    assert(!strcmp(taxi_pc_display_settings_value(&s,1),"4K"));
    assert(!strcmp(taxi_pc_display_settings_value(&s,2),"60 FPS"));
    assert(!strcmp(taxi_pc_display_settings_value(&s,3),"Windowed"));
    assert(!strcmp(taxi_pc_display_settings_value(&s,4),"16:9"));
    assert(!strcmp(taxi_pc_display_settings_value(&s,TAXI_PC_DISPLAY_MENU_TEXT),"Original"));
    assert(!taxi_pc_display_settings_adjust(&s,TAXI_PC_DISPLAY_MENU_TEXT,1));
    assert(!strcmp(taxi_pc_display_settings_value(&s,TAXI_PC_DISPLAY_MENU_TEXT),"Original"));
    assert(!taxi_pc_display_settings_adjust(&s,TAXI_PC_DISPLAY_MENU_TEXT,-1));
    int width=0,height=0;unsigned fps=0;
    assert(!taxi_pc_display_settings_dimensions(&s,&width,&height)&&width==3840&&height==2160);
    assert(!taxi_pc_display_settings_frame_rate(&s,&fps)&&fps==60);
    static const int expected_heights[]={720,1080,1440,2160};
    static const int aspect_num[]={16,16,21,4},aspect_den[]={9,10,9,3};
    for(unsigned r=0;r<4;r++)for(unsigned a=0;a<4;a++){
        s.resolution=(uint8_t)r;s.aspect_ratio=(uint8_t)a;
        assert(!taxi_pc_display_settings_dimensions(&s,&width,&height));
        assert(height==expected_heights[r]);
        assert(width==(int)(((int64_t)height*aspect_num[a]+aspect_den[a]/2)/aspect_den[a]));
    }
    static const unsigned expected_rates[]={30,60,120,0};
    for(unsigned i=0;i<4;i++){s.frame_rate=(uint8_t)i;assert(!taxi_pc_display_settings_frame_rate(&s,&fps)&&fps==expected_rates[i]);}
    for(unsigned row=0;row<TAXI_PC_DISPLAY_SETTING_COUNT;row++){
        uint8_t *value=(uint8_t *)&s+row;uint8_t before=*value;
        assert(!taxi_pc_display_settings_adjust(&s,row,1));
        assert(!taxi_pc_display_settings_adjust(&s,row,-1));
        assert(*value==before);
        /* Texture filtering has three choices; all other rows currently wrap
           on a divisor of eight. Exercise a whole cycle for each row. */
        unsigned cycle=row==TAXI_PC_DISPLAY_TEXTURES?3u:8u;
        for(unsigned i=0;i<cycle;i++)assert(!taxi_pc_display_settings_adjust(&s,row,1));
        assert(*value==before);
    }
    TaxiPcDisplaySettings before=s;
    assert(taxi_pc_display_settings_adjust(&s,6,1)<0&&!memcmp(&s,&before,sizeof s));
    assert(taxi_pc_display_settings_adjust(&s,0,0)<0&&!memcmp(&s,&before,sizeof s));
    s.resolution=255;assert(!taxi_pc_display_settings_value(&s,1));
    assert(taxi_pc_display_settings_adjust(&s,1,1)<0&&s.resolution==255);
    assert(taxi_pc_display_settings_dimensions(&s,&width,&height)<0);
    taxi_pc_display_settings_defaults(0);
    return 0;
}
