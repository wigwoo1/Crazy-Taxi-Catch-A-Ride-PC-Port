#include "challenge_flag_hud.h"
#include "obj_order.h"
#include <string.h>
int taxi_challenge_flag_hud_pixels(uint8_t out[153600],const TaxiFlagHudPlan *plan,const TaxiFontAsset *font){
 if(!out||!plan||!font||font->count!=26||font->cell!=16||!font->metrics||!font->rgba)return -1;
 for(unsigned i=0;i<5;i++)if(plan->glyphs[i].glyph>=26)return -1;
 memset(out,0,153600);
 for(unsigned i=0;i<5;i++){
  const TaxiFlagHudGlyph *g=plan->glyphs+i;const uint16_t *m=font->metrics+g->glyph*9;
  unsigned w=m[0]*8,h=m[1]*8;const uint8_t *pixels=taxi_font_asset_glyph(font,0,g->glyph);
  TaxiObjField field;int16_t matrix[4]={0};
  if(g->scaled){if(taxi_obj_field(g->x,g->y,w,h,128,0,&field)||taxi_obj_matrix_values(0,256,128,matrix))return -1;}
  else{field=(TaxiObjField){.left=g->x-(int16_t)m[2],.top=g->y-(int16_t)m[3],.width=w,.height=h};}
  for(unsigned y=0;y<field.height;y++)for(unsigned x=0;x<field.width;x++){
   int sx=(int)x,sy=(int)y;if(g->scaled){int rc=taxi_obj_affine_point(matrix,w,h,0,(int)x,(int)y,&sx,&sy);if(rc<0)return -1;if(!rc)continue;}
   int dx=field.left+(int)x,dy=field.top+(int)y;if(dx<0||dx>=240||dy<0||dy>=160)continue;
   const uint8_t *source=pixels+((size_t)sy*16+(unsigned)sx)*4;uint8_t *dest=out+((size_t)dy*240+(unsigned)dx)*4;
   /* Earlier OBJ slots retain priority over later same-priority glyphs. */
   if(source[3]&&!dest[3])memcpy(dest,source,4);
  }
 }
 return 0;
}

int taxi_challenge_flag_indicator_pixels(uint8_t out[153600],const TaxiFlagHudPlan *plan,const uint8_t *indices,size_t size,const uint16_t palette[256]){
 if(!out||!plan||!indices||!palette||size!=1440||plan->indicator_variant<1||plan->indicator_variant>2)return -1;
 for(unsigned y=0;y<24;y++){
  int64_t dy=(int64_t)plan->indicator_y+y;if(dy<0||dy>=160)continue;
  for(unsigned x=0;x<20;x++){
   unsigned index=indices[plan->indicator_variant*480+y*20+x];uint16_t color=palette[index];
   uint8_t *dest=out+((size_t)dy*240+4+x)*4;
   for(unsigned c=0;c<3;c++)dest[c]=(uint8_t)(((color>>(c*5))&31)*255/31);
   dest[3]=255;
  }
 }
 return 0;
}
