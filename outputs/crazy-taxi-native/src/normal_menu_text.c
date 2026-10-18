#include "normal_menu_text.h"
#include "text_layout.h"
#include "palette_fade.h"
#include "menu_footer_layout.h"
#include <stdio.h>
#include <string.h>
static int valid(const TaxiFontAsset *font,const char *text){
    if(!text||!font->metrics||!font->rgba||!font->pairs||font->cell!=32||!font->palettes)return 0;
    for(unsigned i=0;text[i];i++)if((uint8_t)text[i]<32||(uint8_t)text[i]-32>=font->count)return 0;
    return 1;
}
static int advance(const TaxiFontAsset *font,unsigned glyph,unsigned next){
    return (int)font->metrics[glyph*9]*8*256-taxi_text_correction_q8(font->metrics,font->count,(uint16_t)glyph,(uint16_t)next,font->pairs,font->spacing,256);
}
static void text_bank(uint8_t *output,const TaxiFontAsset *font,const char *value,int x,int y,int centered,unsigned cap,unsigned bank){
    int width=0;
    for(unsigned i=0;value[i];i++)width+=advance(font,(uint8_t)value[i]-32,value[i+1]?(uint8_t)value[i+1]-32:0);
    int position=x*256-(centered?width/2:0);
    for(unsigned i=0;value[i];i++){
        unsigned glyph=(uint8_t)value[i]-32,next=value[i+1]?(uint8_t)value[i+1]-32:0;
        const uint16_t *m=font->metrics+glyph*9;const uint8_t *pixels=taxi_font_asset_glyph(font,bank,glyph);
        /* Original f94a/f95c signed division truncates toward zero. */
        int left=position/256-(int16_t)m[2],top=y-(int16_t)m[3];
        for(unsigned yy=0;yy<32;yy++)for(unsigned xx=0;xx<32;xx++){
            int dx=left+(int)xx,dy=top+(int)yy;const uint8_t *p=pixels+(yy*32+xx)*4;
            if(p[3]&&dx>=0&&dx<240&&dy>=0&&dy<160)taxi_palette_fade_cap_rgba(p,output+(dy*240+dx)*4,1,(int16_t)cap);
        }
        position+=advance(font,glyph,next);
    }
}
static void text(uint8_t *output,const TaxiFontAsset *font,const char *value,int x,int y,int centered,unsigned cap){
    text_bank(output,font,value,x,y,centered,cap,0);
}
int taxi_menu_number_text(uint8_t output[153600],const TaxiFontAsset *font,
    const TaxiAudioTestRender *request,unsigned cap){
    if(!output||!font||!request||cap>31||request->font!=1||request->text_flags!=4||
       request->x < -240||request->x>480||request->y < -160||request->y>320)return -1;
    unsigned length=0;
    while(length<sizeof request->number&&request->number[length]){
        char c=request->number[length++];if(c!=' '&&(c<'0'||c>'9'))return -1;
    }
    if(!length||length==sizeof request->number||!valid(font,request->number))return -1;
    text(output,font,request->number,request->x,request->y,1,cap);return 0;
}
int taxi_control_menu_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *const labels[16],unsigned page,unsigned layout,unsigned cap){
    if(!output||!fonts||!labels||page>=7||layout>=3||cap>31||fonts[1].palettes<2||
       !valid(fonts,labels[15])||!valid(fonts+1,labels[14])||
       !valid(fonts+1,labels[page*2])||!valid(fonts+1,labels[page*2+1]))return -1;
    char config[64];int n=snprintf(config,sizeof config,"%s %u",labels[14],layout+1);
    if(n<0||(unsigned)n>=sizeof config||!valid(fonts+1,config))return -1;
    text(output,fonts,labels[15],4,6,0,cap);
    text_bank(output,fonts+1,config,120,56,1,cap,1);
    text_bank(output,fonts+1,labels[page*2],120,68,1,cap,1);
    text_bank(output,fonts+1,labels[page*2+1],120,76,1,cap,1);
    return 0;
}
int taxi_audio_menu_test_arrows(uint8_t output[153600],const TaxiFontAsset *font,
    const TaxiAudioTestRender *request){
    if(!output||!font||!font->metrics||!font->rgba||font->cell!=32||!font->palettes||!request||request->arrows>2)return -1;
    for(unsigned i=0;i<request->arrows;i++){
        const TaxiAudioTestArrow *a=request->arrow+i;
        if(a->descriptor<99||a->descriptor>100||a->descriptor>=font->count||a->kind!=1||a->flags!=1)return -1;
    }
    for(unsigned i=0;i<request->arrows;i++){
        const TaxiAudioTestArrow *a=request->arrow+i;
        const uint16_t *m=font->metrics+a->descriptor*9;
        const uint8_t *pixels=taxi_font_asset_glyph(font,0,a->descriptor);
        for(unsigned y=0;y<32;y++)for(unsigned x=0;x<32;x++){
            int64_t dx=(int64_t)a->x-(int16_t)m[2]+x,dy=(int64_t)a->y-(int16_t)m[3]+y;
            const uint8_t *p=pixels+(y*32+x)*4;
            if(!p[3]||dx<0||dx>=240||dy<0||dy>=160)continue;
            for(unsigned c=0;c<4;c++)output[(dy*240+dx)*4+c]=p[c];
        }
    }
    return 0;
}
int taxi_audio_menu_test_text(uint8_t output[153600],const TaxiFontAsset *font,
    int16_t sound_index,int16_t music_index,unsigned cap){
    if(!output||!font||sound_index<0||sound_index>202||music_index<0||music_index>4||cap>31)return -1;
    char sound_value[4],music_value[3];
    snprintf(sound_value,sizeof sound_value,"%3d",sound_index+1);
    snprintf(music_value,sizeof music_value,"%2d",music_index+1);
    if(!valid(font,sound_value)||!valid(font,music_value))return -1;
    text(output,font,sound_value,180,86,1,cap);
    text(output,font,music_value,180,102,1,cap);
    return 0;
}
int taxi_normal_menu_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
                         const char *heading,const char *const *choices,unsigned count,unsigned cap){
    return taxi_normal_menu_text_motion(output,fonts,heading,choices,count,cap,0);
}
static int slide_pixels(int32_t value){
    int pixels=value/4096;
    return pixels-(value<0&&value%4096!=0);
}
int taxi_normal_menu_text_motion(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *heading,const char *const *choices,unsigned count,unsigned cap,int32_t slide_q12){
    if(slide_q12 < -240*4096 || slide_q12 > 240*4096)return -1;
    if(!output||!fonts||!choices||(count!=4&&count!=5&&count!=6)||cap>31||!valid(fonts,heading))return -1;
    for(unsigned i=0;i<count;i++)if(!valid(fonts+1,choices[i]))return -1;
    text(output,fonts,heading,4+slide_pixels(slide_q12),6,0,cap);
    int y=(160-((int)count*16+44))/2+28;
    for(unsigned i=0;i<count;i++)text(output,fonts+1,choices[i],72+slide_pixels(i%2?-slide_q12:slide_q12),y+(int)i*16,1,cap);
    return 0;
}
int taxi_normal_menu_footer(uint8_t output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap){
    return taxi_normal_menu_footer_motion(output,font,front,back,bg,obj,strips,icons,confirm,back_text,cap,0);
}
int taxi_normal_menu_footer_motion(uint8_t output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap,int32_t slide_q12){
    return taxi_normal_menu_footer_layers_motion(output,output,font,front,back,bg,obj,strips,icons,confirm,back_text,cap,slide_q12);
}
int taxi_normal_menu_footer_layers_motion(uint8_t output[153600],uint8_t text_output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap,int32_t slide_q12){
    if(slide_q12 < -240*4096 || slide_q12 > 240*4096)return -1;
    int shift=slide_pixels(slide_q12);
    if(!output||!text_output||!font||!front||!back||!bg||!obj||!strips||!icons||cap>31||(confirm&&!valid(font,confirm))||(back_text&&!valid(font,back_text)))return -1;
    for(unsigned f=1;f<=2;f++){
        const TaxiMenuStrip *s=strips+f;
        if(!s->indices||!s->width||s->width>64||!s->height||s->height>64)return -1;
        for(unsigned i=0;i<s->width*s->height;i++)if(s->indices[i]>15)return -1;
    }
    int width=0;
    if(confirm)for(unsigned i=0;confirm[i];i++)width+=advance(font,(uint8_t)confirm[i]-32,confirm[i+1]?(uint8_t)confirm[i+1]-32:0);
    TaxiMenuFooterLayout layout;
    /* Original kind2 frames4/5 metrics: ox=0, oy=5, ink width15,
       padded cell16x16; sprites.json records offsets0/1024, bank0. */
    if(taxi_menu_footer_layout(160,0x080174dd,confirm!=NULL,back_text!=NULL,
                              (uint16_t)(width/256),15,15,&layout))return -1;
    /* Normal callbacks174dd/17741 and audio17a25: five frame1 middles then frame2,
       queue1/flag4. Backdrop is not a BLDCNT second target. */
    for(unsigned piece=0;piece<=layout.middle_count;piece++){
        const TaxiMenuStrip *s=strips+(piece==layout.middle_count?2:1);
        for(unsigned y=0;y<s->height;y++)for(unsigned x=0;x<s->width;x++){
            int xx=layout.strip_x+(int)piece*32-s->ox+(int)x+shift,yy=layout.strip_y-s->oy+(int)y;
            unsigned index=s->indices[y*s->width+x];if(!index||xx<0||xx>=240||yy<0||yy>=160)continue;
            unsigned a=front[yy*512+xx],b=back[yy*512+xx];
            uint16_t color=taxi_palette_fade_cap_rgb555(obj[index],(int16_t)cap);
            if(a||b){
                uint16_t target=taxi_palette_fade_cap_rgb555(bg[a?a:b],(int16_t)cap),mixed=0;
                for(unsigned k=0;k<3;k++)mixed|=(uint16_t)((((color>>(k*5))&31)+((target>>(k*5))&31))/2)<<(k*5);
                color=mixed;
            }
            uint8_t *p=output+(yy*240+xx)*4;
            for(unsigned k=0;k<3;k++)p[k]=(uint8_t)(((color>>(k*5))&31)*255/31);p[3]=255;
        }
    }
    for(unsigned i=0;i<layout.prompts;i++){
        const TaxiMenuFooterPrompt *prompt=layout.prompt+i;
        unsigned icon=prompt->descriptor-4;
        const char *label=icon?back_text:confirm;
        for(unsigned y=0;y<16;y++)for(unsigned x=0;x<16;x++){
            const uint8_t *p=icons+icon*1024+(y*16+x)*4;
            int xx=prompt->icon_x+shift+(int)x,yy=prompt->icon_y-5+(int)y;
            if(p[3]&&xx>=0&&xx<240&&yy>=0&&yy<160)taxi_palette_fade_cap_rgba(p,output+(yy*240+xx)*4,1,(int16_t)cap);
        }
        text(text_output,font,label,prompt->text_x+shift,prompt->text_y,0,cap);
    }
    return 0;
}

/* Original1AB74..1AD22 briefing text only; settled240x160, BG/footer external. */
int taxi_challenge_briefing_text(uint8_t output[153600],const TaxiFontAsset fonts[2],const char *title,unsigned submode,const char *page_header,const char *const lines[4],unsigned cap){
 if(!output||!fonts||!lines||submode>8||cap>31||!valid(fonts,title)||!valid(fonts+1,page_header))return -1;
 char heading[128];int n=snprintf(heading,sizeof heading,"%u-%u %s",submode/3+1,submode%3+1,title);
 if(n<0||(unsigned)n>=sizeof heading||!valid(fonts,heading))return -1;
 for(unsigned i=0;i<4;i++)if(!valid(fonts+1,lines[i]))return -1;
 text(output,fonts,heading,4,6,0,cap);text(output,fonts+1,page_header,120,44,1,cap);
 for(unsigned i=0;i<4;i++)text(output,fonts+1,lines[i],120,56+(int)i*8,1,cap);
 return 0;
}

int taxi_challenge_grid_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *heading,const char *name_label,const char *driver_label,
    uint16_t badge_mask,unsigned cap){
    if(!output||!fonts||cap>31||badge_mask>511||!valid(fonts,heading)||
       !valid(fonts+1,name_label)||!valid(fonts+1,driver_label)||!valid(fonts+1,"1-123"))return -1;
    text(output,fonts,heading,4,6,0,cap);
    text(output,fonts+1,name_label,28,43,1,cap);
    text(output,fonts+1,driver_label,28,59,1,cap);
    for(unsigned mode=0;mode<9;mode++)if(!(badge_mask&(1u<<mode))){
        char number[8];snprintf(number,sizeof number,"%u-%u",mode/3+1,mode%3+1);
        text(output,fonts+1,number,20+(int)(mode/3)*34,75+(int)(mode%3)*16,1,cap);
    }
    return 0;
}

int taxi_challenge_briefing_screen(uint8_t output[153600],
    const TaxiChallengeScreenAssets *assets,const char *title,unsigned submode,
    const char *page_header,const char *const lines[4],const char *toggle_label,
    const char *back_label,unsigned cap){
    if(!output||!assets||!assets->fonts)return -1;
    uint8_t composed[153600];
    if(taxi_menu_background_only(composed,assets->front,assets->back,assets->bg,cap)||
       taxi_challenge_briefing_text(composed,assets->fonts,title,submode,page_header,lines,cap)||
       taxi_normal_menu_footer(composed,assets->fonts+1,assets->front,assets->back,
           assets->bg,assets->obj,assets->strips,assets->icons,toggle_label,back_label,cap))return -1;
    memcpy(output,composed,sizeof composed);return 0;
}

int taxi_challenge_failure_text(uint8_t output[153600],const TaxiFontAsset *font,
    const char *const labels[4],unsigned selected,unsigned cap){
    if(!output||!font||!labels||selected>1||cap>31||font->palettes<3)return -1;
    for(unsigned i=0;i<4;i++)if(!valid(font,labels[i]))return -1;
    text_bank(output,font,labels[0],120,25,1,cap,2);
    text_bank(output,font,labels[1],120,45,1,cap,0);
    text_bank(output,font,labels[2],120,65,1,cap,selected==0);
    text_bank(output,font,labels[3],120,75,1,cap,selected==1);
    return 0;
}

int taxi_challenge_success_text(uint8_t output[153600],const TaxiFontAsset *font,
    const char *const labels[6],const char *name,const char *result,
    const char *const records[3],const char *const names[3],unsigned cap){
    if(!output||!font||!labels||!records||!names||cap>31||font->palettes<4)return -1;
    for(unsigned i=0;i<6;i++)if(!valid(font,labels[i]))return -1;
    if(!valid(font,name)||!valid(font,result))return -1;
    for(unsigned i=0;i<3;i++)if(!valid(font,records[i])||!valid(font,names[i]))return -1;
    text_bank(output,font,labels[0],120,25,1,cap,1);
    text_bank(output,font,labels[1],120,40,1,cap,0);
    text_bank(output,font,name,120,50,1,cap,0);
    text_bank(output,font,labels[2],120,65,1,cap,3);
    text_bank(output,font,result,120,75,1,cap,1);
    for(unsigned i=0;i<3;i++)text_bank(output,font,labels[i+3],60+(int)i*60,90,1,cap,0);
    for(unsigned i=0;i<3;i++){
        unsigned bank=i==0?3:0;char rank[2]={(char)('1'+i),0};
        text_bank(output,font,rank,60,105+(int)i*10,1,cap,bank);
        text_bank(output,font,records[i],120,105+(int)i*10,1,cap,bank);
        text_bank(output,font,names[i],180,105+(int)i*10,1,cap,bank);
    }
    return 0;
}
