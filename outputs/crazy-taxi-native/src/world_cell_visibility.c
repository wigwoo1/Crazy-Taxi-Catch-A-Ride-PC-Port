#include "world_cell_visibility.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static uint32_t asr8(uint32_t v){return (v>>8)|((v&0x80000000u)?0xff000000u:0u);}
int taxi_world_cell_visibility(const TaxiWorldCellVisibility *s,uint32_t cx,uint32_t cz,uint32_t *packed,uint32_t *shared){
 if(!s)return -1;
 uint32_t x=(uint32_t)s->origin_x-(uint32_t)s->camera_x+(uint32_t)s->spacing_x*cx;
 uint32_t z=(uint32_t)s->origin_z-(uint32_t)s->camera_z+(uint32_t)s->spacing_z*cz,y=0,flags=0;
 const uint32_t offsets_x[4]={0,0,(uint32_t)s->spacing_x,(uint32_t)s->spacing_x};
 const uint32_t offsets_z[4]={0,(uint32_t)s->spacing_z,0,(uint32_t)s->spacing_z};
 for(unsigned i=0;i<4;i++){
  x+=offsets_x[i];z+=offsets_z[i];
  uint32_t rx=x*(uint32_t)s->cos_yaw-z*(uint32_t)s->sin_yaw;
  uint32_t rz=x*(uint32_t)s->sin_yaw+z*(uint32_t)s->cos_yaw;
  x=asr8(rx);z=asr8(rz);
  uint32_t pz=z*(uint32_t)s->cos_pitch-y*(uint32_t)s->sin_pitch;
  uint32_t py=z*(uint32_t)s->sin_pitch+y*(uint32_t)s->cos_pitch;
  z=asr8(pz);y=asr8(py);
  uint32_t extent=z*(uint32_t)s->horizontal;
  if(bits(x)<=bits(0u-extent))flags|=1;
  if(bits(x)>=bits(extent))flags|=2;
  if(bits(z)<=s->near_plane)flags|=16;
  if(bits(z)>=s->far_plane)flags|=32;
  flags<<=6;
 }
 uint32_t common=(flags&63u)&((flags>>6)&63u)&((flags>>12)&63u)&((flags>>18)&63u);
 if(packed)*packed=flags;if(shared)*shared=common;
 return common==0;
}
