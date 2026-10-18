#include "text_layout.h"
#include <string.h>
#include "challenge_ball_hud.h"
int taxi_challenge_ball_hud_plan(TaxiBallHudPlan *out,int16_t remaining,int16_t popped,uint16_t width,uint16_t height,uint8_t flags,int paused,const uint8_t positions[60],size_t size){
 if(!out||!positions||size!=60)return -1;
 TaxiBallHudPlan p={0};
 if(taxi_challenge_flag_hud_plan(&p.clock,remaining,height,flags)||taxi_challenge_flag_hud_positions(&p.clock,positions,size))return -1;
 int32_t x=(int32_t)width-80,y=p.clock.glyphs[4].y;
 int32_t tens=popped/10,units=popped%10;
 if(tens)p.counter[p.counter_count++]=(TaxiBallCountGlyph){(uint8_t)(tens+16),x,y,0};
 x+=16;p.counter[p.counter_count++]=(TaxiBallCountGlyph){(uint8_t)(units+16),x,y,0};
 x+=16;p.counter[p.counter_count++]=(TaxiBallCountGlyph){15,x,y,0};
 x+=16;p.counter[p.counter_count++]=(TaxiBallCountGlyph){18,x,y,0};
 x+=16;p.counter[p.counter_count++]=(TaxiBallCountGlyph){16,x,y,0};
 p.label_id=615;p.label_x=(int32_t)width-2;uint32_t raw_y=(uint32_t)y+18u;memcpy(&p.label_y,&raw_y,4);p.label_alignment=2;p.label_style=paused?1:2;p.label_scale=256;
 *out=p;return 0;
}

int taxi_challenge_ball_hud_pixels(uint8_t out[153600],const TaxiBallHudPlan *p,const TaxiFontAsset *font){
 if(!out||!p||!font||p->counter_count>5||font->count!=26||font->cell!=16||!font->metrics||!font->rgba)return -1;
 for(unsigned i=0;i<p->counter_count;i++)if(p->counter[i].glyph>=26)return -1;
 if(taxi_challenge_flag_hud_pixels(out,&p->clock,font))return -1;
 for(unsigned i=0;i<p->counter_count;i++){
  const TaxiBallCountGlyph *g=p->counter+i;const uint16_t *m=font->metrics+g->glyph*9;
  const uint8_t *pixels=taxi_font_asset_glyph(font,0,g->glyph);int left=g->x-(int16_t)m[2],top=g->y-(int16_t)m[3];
  for(unsigned y=0;y<(unsigned)m[1]*8;y++)for(unsigned x=0;x<(unsigned)m[0]*8;x++){
   int dx=left+(int)x,dy=top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;
   const uint8_t *source=pixels+((size_t)y*16+x)*4;uint8_t *dest=out+((size_t)dy*240+(unsigned)dx)*4;
   if(source[3]&&!dest[3])memcpy(dest,source,4);
  }
 }
 return 0;
}

int taxi_challenge_ball_label_pixels(uint8_t out[153600],const TaxiBallHudPlan *p,const TaxiFontAsset *font,const char *label){
 if(!out||!p||!font||!label||font->cell!=32||!font->metrics||!font->rgba||!font->pairs||p->label_alignment!=2||p->label_scale!=256||p->label_style>=font->palettes)return -1;
 size_t length=0;int width=0;
 for(;label[length];length++){unsigned glyph=(uint8_t)label[length];if(glyph<32||glyph-32>=font->count||length>=4096)return -1;}
 for(size_t i=0;i<length;i++){unsigned glyph=(uint8_t)label[i]-32,next=i+1<length?(uint8_t)label[i+1]-32:0;width+=(int)font->metrics[glyph*9]*8*256-taxi_text_correction_q8(font->metrics,font->count,(uint16_t)glyph,(uint16_t)next,font->pairs,font->spacing,256);}
 int position=p->label_x*256-width;
 for(size_t i=0;i<length;i++){
  unsigned glyph=(uint8_t)label[i]-32,next=i+1<length?(uint8_t)label[i+1]-32:0;const uint16_t *m=font->metrics+glyph*9;
  const uint8_t *pixels=taxi_font_asset_glyph(font,p->label_style,glyph);int left=position/256-(int16_t)m[2],top=p->label_y-(int16_t)m[3];
  for(unsigned y=0;y<32;y++)for(unsigned x=0;x<32;x++){int dx=left+(int)x,dy=top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;const uint8_t *source=pixels+((size_t)y*32+x)*4;uint8_t *dest=out+((size_t)dy*240+(unsigned)dx)*4;if(source[3]&&!dest[3])memcpy(dest,source,4);}
  position+=(int)m[0]*8*256-taxi_text_correction_q8(font->metrics,font->count,(uint16_t)glyph,(uint16_t)next,font->pairs,font->spacing,256);
 }
 return 0;
}
