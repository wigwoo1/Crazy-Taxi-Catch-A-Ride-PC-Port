#include "restart_visual_owned.h"
#include "world_background.h"
#include <string.h>
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static const uint8_t *range(const TaxiOrdinaryGraphicsOwners *g,uint32_t id,size_t n){
 if(id<0x08000000u)return NULL;size_t off=id-0x08000000u;return off>g->rom_size||n>g->rom_size-off?NULL:g->rom+off;
}
static int valid(TaxiRestartVisualOwners *o){
 if(!o||!o->loading||!o->buffers||!o->display||!o->bases||!o->bindings||!o->bootstrap||!o->render||!o->registration||!o->assets||!o->guidance||!o->hud||!o->hud_input||!o->session||!o->pause||!o->assets->descriptor||!o->assets->sine||!o->assets->cosine||!o->assets->textures||!o->assets->palette||!o->alpha_00e0||!o->alpha_00e2||!o->driver_graphics_187c||!o->intro_graphics_1804||!o->fill_06900||!o->frame_34d4||!o->frame_34d8||!o->scroll_0010||!o->scroll_0012)return -1;
 TaxiOrdinaryLoadingPending *p=o->loading;if(p->phase!=TAXI_LOADING_IDLE||!p->state||!p->loading||!p->loading->frame||!p->loading->fill||!p->host||!p->service||taxi_ordinary_presentation_owner_validate(p->presentation))return -1;
 TaxiOrdinaryGraphicsOwners *g=p->presentation->graphics;
 if(p->loading->fill!=o->fill_06900||p->loading->graphics!=g||p->loading->copies!=p->presentation->copies||p->loading->immediate!=p->presentation->immediate||p->loading->frame_size<38400)return -1;
 if(o->hud->graphics!=g||o->hud->copies!=p->presentation->copies||o->hud->immediate!=p->presentation->immediate||o->hud_input->demo||o->hud_input->driver>4||o->hud_input->challenge_driver>3||o->hud_input->session_driver>3)return -1;
 return 0;
}
static int page(TaxiRestartVisualOwners *o){
 TaxiOrdinaryLoadingPending *p=o->loading;if(taxi_ordinary_loading_pending_buffers(p,o->buffers))return -1;
 for(unsigned i=0;i<2;i++)if(o->buffers->pixels[i]==*p->loading->frame){
  o->render->frame=o->buffers->pixels[i];o->render->frame_size=o->buffers->size[i];o->render->alternate_frame=o->buffers->pixels[i^1];o->render->alternate_size=o->buffers->size[i^1];return 0;
 }
 return -1;
}
static void fonts(TaxiRestartVisualOwners *o){
 TaxiOrdinaryLoadingPending *p=o->loading;TaxiOrdinaryGraphicsOwners *g=p->presentation->graphics;
 p->service->cursor_1e10=g->fonts->cursor;p->host->frame.animation_1e10=g->fonts->cursor;memcpy((uint8_t *)g->palette->records+0x680,g->fonts,sizeof *g->fonts);
}
static int reset(TaxiRestartVisualOwners *o){
 TaxiOrdinaryLoadingPending *p=o->loading;p->service->current_3520=p->host->counter;
 if(taxi_presentation_reset(p->service,*o->alpha_00e0,*o->alpha_00e2))return -1;
 p->host->frame.previous_3524=p->service->previous_3524;p->host->frame.delta_3b00=(int32_t)p->service->delta_3b00;return 0;
}
static int graphics(TaxiRestartVisualOwners *o,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t a[7]){
 static const uint32_t targets[15]={0x08027994,0x0800fec8,0x0800fec8,0x0800e154,0x0800e5a8,0x0802ac6c,0x0802c834,0x0802ca94,0x0802c688,0x080203c4,0x08027fb8,0x08027fb8,0x08027fb8,0x08027fb8,0x08026440};
 unsigned i=o->graphics_index;if(i>=15||target!=targets[i]||s->graphics.city>1)return -1;
 TaxiOrdinaryLoadingPending *p=o->loading;TaxiOrdinaryGraphicsOwners *g=p->presentation->graphics;uint32_t expected[7]={0};
 switch(i){
 case 0:expected[0]=0x08070e50;expected[1]=1;break;
 case 1:expected[1]=32;expected[3]=0x08415bbc;expected[4]=s->graphics.retained_sl;break;
 case 2:expected[0]=1;expected[1]=32;expected[2]=3;expected[4]=3;break;
 case 3:expected[0]=0x084a5a34;expected[1]=32;expected[2]=1;break;
 case 4:expected[0]=1;expected[1]=32;expected[2]=expected[3]=1;break;
 case 5:expected[0]=0x087556cc+104u*s->graphics.city;expected[3]=240;expected[4]=160;expected[5]=256;expected[6]=38400;break;
 case 6:expected[0]=o->render->source_transform_address;break;
 case 7:{unsigned b=(g->palette->count-1u)&255u;expected[0]=(uint32_t)(b<128?(int)b:(int)b-256);break;}
 case 8:expected[0]=31;break;
 case 10:case 11:case 12:expected[0]=0x080fcff8;expected[1]=expected[2]=i-10;expected[3]=7;expected[4]=s->graphics.retained_sl;break;
 case 13:expected[0]=0x080fd5fc;expected[3]=2;expected[4]=s->graphics.retained_sl;break;
 default:break;
 }
 if(memcmp(a,expected,sizeof expected))return -1;
 int rc=0;
 if(i==0){
  if(o->display->globals[1]!=4||half(o->display->globals+2)!=240||half(o->display->globals+4)!=160||o->bases->base_152c!=g->destination||o->bases->base_1530!=g->destination)return -1;
  p->state->gate.width_7542=240;p->state->gate.height_7544=160;p->service->mode_7541=4;p->service->bank_7548=o->display->globals[8];p->service->first_754c=word(o->display->globals+12);p->service->second_7550=word(o->display->globals+16);p->service->display_control=o->display->control;*p->presentation->mosaic=o->display->mosaic;
  o->render->width=240;o->render->height=160;o->render->bitmap_mode=4;s->entry.width_7542=240;s->entry.height_7544=160;
  if(page(o))return -1;
  rc=taxi_obj_assets_load_eleven(g->assets,g->palette,g->banks,g->rom,g->rom_size,a[0],g->destination);
  if(!rc)rc=taxi_startup_obj_setup(g->queue,g->transport,g->base_template,g->matrix_template,a[1]);
 }else if(i==1||i==2){rc=taxi_font_setup(g->fonts,a[0],a[1],a[2],a[3],a[4],a[5]);if(!rc)fonts(o);}
 else if(i==3||i==4)rc=taxi_city_palette_call(p->presentation->palette,target,a)==1?0:-1;
 else if(i==5){
  const uint8_t *descriptor=range(g,a[0],104);if(!descriptor||o->assets->descriptor_size!=104||memcmp(descriptor,o->assets->descriptor,104)||page(o))return -1;
  o->render->registered_materials=(const uint8_t *)g->palette->records;o->render->registered_material_count=g->palette->count;
  const int32_t bounds[4]={0,0,240,160};rc=taxi_graphics_bootstrap_owned(o->bindings,o->bootstrap,o->render,o->registration,o->assets,bounds,256,38400);
  if(!rc)*o->fill_06900=o->render->fill_06900;
 }else if(i==6||i==7||i==8)rc=taxi_restart_render_call(o->render,target,a)==1?0:-1;
 else if(i==9)rc=taxi_guidance_scene(o->guidance,o->registration->descriptor,104,g->rom,g->rom_size);
 else if(i>=10&&i<=13){
  if(i==10)*o->driver_graphics_187c=s->graphics.driver_graphics_187c;
  if(i==13)*o->intro_graphics_1804=s->graphics.intro_graphics_1804;
  TaxiSpriteReplacementOwner sprite={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,p->presentation->copies,p->presentation->immediate,g->assets->objects,sizeof g->assets->objects,g->destination};
  rc=taxi_sprite_replace_owned(&sprite,a[0],a[1],a[2],a[3],a[4])<0?-1:0;
 }else rc=reset(o);
 if(rc)return -1;s->graphics.viewport_1780=(uint8_t)g->palette->count;s->graphics.fade_177b=p->presentation->palette->fade->fade.active_177b;o->graphics_index++;return 0;
}
static int entry(TaxiRestartVisualOwners *o,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t a[7]){
 static const uint32_t targets[8]={0x08026440,0x0800e5a8,0x0802c9f0,0x0802c9f0,0x0800e5a8,0x0801ee28,0x0802277c,0x08026440};
 unsigned i=o->entry_index;if(o->graphics_index!=15||i>=8||target!=targets[i])return -1;uint32_t expected[7]={0};
 if(i==1||i==4){expected[0]=i==4;expected[1]=32;expected[2]=expected[3]=1;}
 if(i==2||i==3){expected[2]=s->entry.width_7542;expected[3]=s->entry.height_7544;}
 if(memcmp(a,expected,sizeof expected))return -1;
 if(!i){s->entry.scroll_x=*o->scroll_0010;s->entry.scroll_y=*o->scroll_0012;}
 *o->frame_34d4=s->entry.frame_34d4;*o->frame_34d8=s->entry.frame_34d8;*o->scroll_0010=s->entry.scroll_x;*o->scroll_0012=s->entry.scroll_y;
 TaxiOrdinaryLoadingPending *p=o->loading;int rc=0;
 if(i==0||i==7)rc=reset(o);
 else if(i==1||i==4)rc=taxi_city_palette_call(p->presentation->palette,target,a)==1?0:-1;
 else if(i==2||i==3){if(page(o))return -1;rc=taxi_restart_render_call(o->render,target,a)==1?0:-1;}
 else if(i==5){
  TaxiHudStartInput input=*o->hud_input;input.settings=o->session->settings[0];
  memcpy(o->hud->scalar_16c0,o->session->session_165e+98,32);rc=taxi_hud_start_owned(o->hud,&input);
  memcpy(o->session->session_165e+98,o->hud->scalar_16c0,32);
 }else {taxi_pause_menu_reset(o->pause);p->service->frozen_5f20=o->pause->frozen;}
 if(rc)return -1;s->entry.fade_177b=p->presentation->palette->fade->fade.active_177b;o->entry_index++;return 0;
}
int taxi_restart_visual_owned(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 TaxiRestartVisualOwners *o=context;if(!o||o->executing||o->aborted||!s||!args||!value||valid(o)||(s->phase!=TAXI_RESTART_GRAPHICS&&s->phase!=TAXI_RESTART_ENTRY)||(o->flow&&o->flow!=s))return -1;
 if(!o->flow&&s->phase!=TAXI_RESTART_GRAPHICS)return -1;
 o->executing=1;int rc=s->phase==TAXI_RESTART_GRAPHICS?graphics(o,s,target,args):entry(o,s,target,args);
 if(!rc){o->flow=s;*value=0;}o->executing=0;return rc;
}
