#include "taxi_render_owned.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static const uint8_t *range(const TaxiOrdinaryGraphicsOwners *g,size_t offset,size_t n){if(offset>g->rom_size||n>g->rom_size-offset)return 0;return g->rom+offset;}
int taxi_render_owned(TaxiOrdinaryGraphicsOwners *g,TaxiSpriteCopies *copies,TaxiSpriteImmediateCopy *dma,TaxiPaletteTransfer *palette_dma,uint32_t *previous,uint32_t frame,uint8_t driver,uint8_t immediate,uint8_t override,const uint32_t left[3],const uint32_t right[3],uint32_t scale,uint16_t roll,int32_t sine,int32_t cosine,uint16_t width,uint16_t height,uint16_t mask){
 if(!g||!g->assets||!g->rom||!g->palette||!g->banks||!g->queue||!g->transport||!left||!right)return -1;
 const size_t resource=0x70e50;const uint8_t *p=range(g,resource+8,4);if(!p)return -1;size_t group=resource+word(p);p=range(g,group,20);if(!p||word(p)<2||word(p+4)!=16)return -1;
 uint16_t metrics[2][9];for(unsigned f=0;f<2;f++){p=range(g,group+20+f*4,4);if(!p)return -1;p=range(g,resource+word(p),18);if(!p)return -1;for(unsigned i=0;i<9;i++)metrics[f][i]=(uint16_t)(p[2*i]|(uint16_t)p[2*i+1]<<8);if(metrics[f][0]!=8||metrics[f][1]!=8)return -1;}
 TaxiSpriteReplacementOwner sprite={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,copies,dma,g->assets->objects,sizeof g->assets->objects,g->destination};
 if(taxi_frame_cache_owned(&sprite,previous,frame,driver,immediate)<0)return -1;
 TaxiPlacementRequest requests[2];taxi_placement_requests(left,right,scale,roll,requests);
 uint32_t tile=(g->assets->bases[1]+0xf9ff0000u)>>5;
 for(unsigned f=0;f<2;f++){TaxiPlacementRequest *r=requests+f;TaxiStartupObjQueued out;if(taxi_affine_queue(g->queue,g->transport,metrics[f],(int32_t)r->position[0],(int32_t)r->position[1],(int32_t)r->position[2],width,height,2,0,0,mask,tile,g->assets->palette_bases[1],0,(int16_t)r->scale,(int16_t)r->angle,sine,cosine,&out)<0)return -1;}
 if(taxi_frame_palette_owned(g->rom,g->rom_size,g->palette,g->banks,palette_dma,immediate,driver,override)<0)return -1;
 return 0;
}
