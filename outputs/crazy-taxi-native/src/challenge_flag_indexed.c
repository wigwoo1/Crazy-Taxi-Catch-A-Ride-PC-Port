#include "challenge_flag_indexed.h"
#include <string.h>
int taxi_challenge_flag_vertices(const TaxiChallengeFlagProjection *p,TaxiIndexedVertex out[4]){
 if(!p||!out||(p->packet_flags!=32&&p->packet_flags!=36))return -1;
 TaxiIndexedVertex v[4];
 for(unsigned i=0;i<4;i++)v[i]=(TaxiIndexedVertex){p->screen_corners[i].x,p->screen_corners[i].y,i<2?0u:32u<<14,(i==0||i==3)?64u<<14:0u,(uint32_t)p->screen_corners[i].z};
 memcpy(out,v,sizeof v);return 0;
}
int taxi_challenge_flag_indexed(uint8_t framebuffer[38400],const TaxiChallengeFlagProjection *p,const uint8_t indices[2048]){
 TaxiIndexedVertex vertices[4];TaxiIndexedSampling sampling;
 if(!framebuffer||!indices||taxi_challenge_flag_vertices(p,vertices)||!taxi_indexed_sampling(&sampling,32,64,5,0))return -1;
 if(p->packet_flags==32)return taxi_indexed_unshaded_polygon(framebuffer,vertices,4,indices,2048,sampling)?1:-1;
 uint8_t identity[2560];for(unsigned i=0;i<2560;i++)identity[i]=(uint8_t)i;
 return taxi_indexed_polygon(framebuffer,vertices,4,indices,2048,sampling,identity,0,0,240,160,0,0)?1:-1;
}

int taxi_challenge_flag_rows(TaxiIndexedSpan spans[160],const TaxiChallengeFlagProjection *p,int32_t left,int32_t right){
 TaxiIndexedVertex vertices[4];
 if(!spans||taxi_challenge_flag_vertices(p,vertices))return -1;
 return taxi_indexed_affine_rows_bounds(spans,vertices,4,left,right,(p->packet_flags&4)!=0);
}
