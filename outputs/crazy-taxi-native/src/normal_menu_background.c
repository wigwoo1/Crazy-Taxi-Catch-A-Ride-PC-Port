#include "normal_menu_background.h"
#include "palette_fade.h"
static uint16_t mix(uint16_t a,uint16_t b){
    uint16_t result=0;
    for(unsigned s=0;s<=10;s+=5)result|=(uint16_t)((((a>>s)&31)+((b>>s)&31))/2)<<s;
    return result;
}
static void pixel(uint8_t *out,uint16_t c){
    for(unsigned k=0;k<3;k++)out[k]=(uint8_t)(((c>>(k*5))&31)*255/31);
    out[3]=255;
}
int taxi_normal_menu_background(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned count,unsigned selected,unsigned cap){
    return taxi_normal_menu_background_motion(output,front,back,palette,obj,strips,count,selected,cap,0);
}
int taxi_normal_menu_background_motion(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned count,unsigned selected,unsigned cap,int32_t slide_q12){
    if(slide_q12 < -240*4096 || slide_q12 > 240*4096)return -1;
    if(!output||!front||!back||!palette||!obj||!strips||(count!=4&&count!=5&&count!=6)||selected>=count||cap>31)return -1;
    for(unsigned f=0;f<6;f++){
        if(!strips[f].indices||!strips[f].width||strips[f].width>64||!strips[f].height||strips[f].height>64)return -1;
        for(unsigned i=0;i<strips[f].width*strips[f].height;i++)if(strips[f].indices[i]>15)return -1;
    }
    uint16_t bg[256],colors[16];
    for(unsigned i=0;i<256;i++)bg[i]=taxi_palette_fade_cap_rgb555(palette[i],(int16_t)cap);
    for(unsigned i=0;i<16;i++)colors[i]=taxi_palette_fade_cap_rgb555(obj[i],(int16_t)cap);
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        unsigned a=front[y*512+x],b=back[y*512+x];
        pixel(output+(y*240+x)*4,a&&b?mix(bg[a],bg[b]):bg[a?a:b]);
    }
    for(unsigned row=0;row<count;row++)for(unsigned piece=0;piece<5;piece++){
        unsigned base=row==selected?3:0,f=base+(piece==0?0:piece==4?2:1);
        const TaxiMenuStrip *strip=strips+f;
        int anchor=piece==0?16:24+(int)(piece-1)*32;
        int32_t displacement=row%2?-slide_q12:slide_q12;
        anchor+=displacement/4096-(displacement<0&&displacement%4096!=0);
        int top=(160-((int)count*16+44))/2+28+(int)row*16-4;
        for(unsigned y=0;y<strip->height;y++)for(unsigned x=0;x<strip->width;x++){
            unsigned index=strip->indices[y*strip->width+x];int xx=anchor-strip->ox+(int)x,yy=top-strip->oy+(int)y;
            if(!index||xx<0||xx>=240||yy<0||yy>=160)continue;
            unsigned a=front[yy*512+xx],b=back[yy*512+xx];
            pixel(output+(yy*240+xx)*4,row==selected||!(a||b)?colors[index]:mix(colors[index],bg[a?a:b]));
        }
    }
    return 0;
}

int taxi_menu_background_only(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],unsigned cap){
    if(!output||!front||!back||!palette||cap>31)return -1;
    uint16_t bg[256];
    for(unsigned i=0;i<256;i++)bg[i]=taxi_palette_fade_cap_rgb555(palette[i],(int16_t)cap);
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        unsigned a=front[y*512+x],b=back[y*512+x];
        pixel(output+(y*240+x)*4,a&&b?mix(bg[a],bg[b]):bg[a?a:b]);
    }
    return 0;
}

static uint16_t original_rgba_color(const uint8_t *p){
    uint16_t c=0;
    for(unsigned k=0;k<3;k++)c|=(uint16_t)(((unsigned)p[k]*31+254)/255)<<(5*k);
    return c;
}
int taxi_challenge_grid_buttons(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint8_t grid_sprites[12288],
    unsigned selected,uint16_t badge_mask,unsigned cap){
    if(!output||!front||!back||!palette||!grid_sprites||selected>10||badge_mask>511||cap>31)return -1;
    for(unsigned mode=0;mode<9;mode++){
        int x0=4+(int)(mode/3)*34,y0=71+(int)(mode%3)*16;
        int active=selected==mode+2;
        const uint8_t *sprite=grid_sprites+(active?4096:0);
        for(unsigned y=0;y<16;y++)for(unsigned x=0;x<32;x++){
            const uint8_t *p=sprite+(y*32+x)*4;if(!p[3])continue;
            unsigned xx=(unsigned)x0+x,yy=(unsigned)y0+y;
            uint16_t color=taxi_palette_fade_cap_rgb555(original_rgba_color(p),(int16_t)cap);
            unsigned a=front[yy*512+xx],b=back[yy*512+xx];
            if(!active&&(a||b))color=mix(color,taxi_palette_fade_cap_rgb555(palette[a?a:b],(int16_t)cap));
            pixel(output+(yy*240+xx)*4,color);
        }
        if(badge_mask&(1u<<mode))for(unsigned y=0;y<16;y++)for(unsigned x=0;x<32;x++){
            const uint8_t *p=grid_sprites+8192+(y*32+x)*4;if(!p[3])continue;
            pixel(output+(((unsigned)y0+y)*240+(unsigned)x0+x)*4,
                taxi_palette_fade_cap_rgb555(original_rgba_color(p),(int16_t)cap));
        }
    }
    return 0;
}

/* Original challenge rows0/1:48pixel width,8cap+32middle+8cap. */
int taxi_challenge_profile_panels(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned selected,unsigned cap){
    if(!output||!front||!back||!palette||!obj||!strips||selected>10||cap>31)return -1;
    for(unsigned f=0;f<6;f++){
        if(!strips[f].indices||!strips[f].width||strips[f].width>64||!strips[f].height||strips[f].height>64)return -1;
        for(unsigned i=0;i<strips[f].width*strips[f].height;i++)if(strips[f].indices[i]>15)return -1;
    }
    for(unsigned row=0;row<2;row++)for(unsigned piece=0;piece<3;piece++){
        unsigned f=(selected==row?3:0)+piece;
        const TaxiMenuStrip *s=strips+f;int anchor=piece==0?4:piece==1?12:44;
        int top=39+(int)row*16;
        for(unsigned y=0;y<s->height;y++)for(unsigned x=0;x<s->width;x++){
            unsigned index=s->indices[y*s->width+x];int xx=anchor-s->ox+(int)x,yy=top-s->oy+(int)y;
            if(!index||xx<0||xx>=240||yy<0||yy>=160)continue;
            uint16_t color=taxi_palette_fade_cap_rgb555(obj[index],(int16_t)cap);
            unsigned a=front[yy*512+xx],b=back[yy*512+xx];
            if(selected!=row&&(a||b))color=mix(color,taxi_palette_fade_cap_rgb555(palette[a?a:b],(int16_t)cap));
            pixel(output+(yy*240+xx)*4,color);
        }
    }
    return 0;
}
