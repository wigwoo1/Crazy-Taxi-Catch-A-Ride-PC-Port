#include "../src/indexed_pixel.h"
#include "../src/taxi_shadow.h"
#include "../src/taxi_oam_layer.h"
#include "../src/indexed_asset.h"
#include "../src/render_queue.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
typedef struct {unsigned family,texture,pattern,n;int rect[4];TaxiIndexedVertex vertices[8];} Packet;
typedef struct {Packet packets[64];TaxiIndexedAsset asset;TaxiShadow shadow;uint8_t fb[38400];} Context;
static int draw(void *arg,unsigned kind,size_t payload){Context *c=arg;Packet *p=c->packets+payload;if(kind!=p->family)return -1;TaxiPairColumnWork w={0};TaxiFlatRowWork f={0};int ok=0;if(p->family==0)ok=taxi_indexed_clipped_flat_polygon_work(c->fb,p->vertices,p->n,(uint16_t)p->pattern,&f);else if(p->family==7){if(p->texture>=64)return -1;ok=taxi_indexed_billboard_work(c->fb,p->rect[0],p->rect[1],p->rect[2],p->rect[3],c->shadow.pixels+p->texture*2048,2048,64,32,6,0,&w);}else{TaxiIndexedTexture t;TaxiIndexedSampling s;if(taxi_indexed_asset_texture(&c->asset,p->texture,&t)||!taxi_indexed_sampling(&s,t.width,t.height,t.log_width,0))return -1;switch(p->family){case 1:ok=taxi_indexed_unshaded_polygon_work(c->fb,p->vertices,p->n,t.pixels,t.size,s,&w);break;case 2:ok=taxi_indexed_unshaded_clipped_polygon_work(c->fb,p->vertices,p->n,t.pixels,t.size,s,&w);break;case 3:ok=taxi_indexed_perspective_rows_polygon_work(c->fb,p->vertices,p->n,t.pixels,t.size,s,&w);break;case 4:ok=taxi_indexed_perspective_columns_polygon_work(c->fb,p->vertices,p->n,t.pixels,t.size,s,&w);break;case 5:case 6:ok=taxi_indexed_billboard_work(c->fb,p->rect[0],p->rect[1],p->rect[2],p->rect[3],t.pixels,t.size,t.width,t.height,t.log_width,p->family==5,&w);break;}}if(!ok)return -1;uint32_t counts[5]={(uint32_t)payload,w.texture_byte_reads,w.framebuffer_halfword_writes,f.framebuffer_halfword_writes,f.framebuffer_word_writes};if(fwrite(c->fb,1,sizeof c->fb,stdout)!=sizeof c->fb||fwrite(counts,1,sizeof counts,stdout)!=sizeof counts)return -1;return 0;}
int main(int argc,char **argv){if(argc!=4&&argc!=6)return 1;Context c={0};if(taxi_indexed_asset_load(&c.asset,argv[1])||taxi_shadow_load(&c.shadow,argv[3]))return 2;if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 3;FILE *background=fopen(argv[2],"rb");if(!background||fread(c.fb,1,sizeof c.fb,background)!=sizeof c.fb)return 11;fclose(background);unsigned count;if(scanf("%u",&count)!=1||count>64)return 4;TaxiRenderPacket storage[64];TaxiRenderQueue q;taxi_render_queue_init(&q,storage,64);for(unsigned i=0;i<count;i++){Packet *p=c.packets+i;unsigned bucket;if(scanf("%u%u%u%u%u",&p->family,&bucket,&p->texture,&p->pattern,&p->n)!=5||p->n>8)return 5;if(p->family>=5){for(unsigned j=0;j<4;j++)if(scanf("%d",p->rect+j)!=1)return 6;}else for(unsigned j=0;j<p->n;j++){int x,y;unsigned u,v,z;if(scanf("%d%d%u%u%u",&x,&y,&u,&v,&z)!=5)return 7;p->vertices[j]=(TaxiIndexedVertex){x,y,u,v,z};}if(taxi_render_queue_push(&q,bucket,p->family,i))return 8;}if(taxi_render_queue_consume(&q,draw,&c))return 9;if(q.count)return 10;
if(argc==6){TaxiTitleAssets assets={0};TaxiPaletteBanks palette={0};uint16_t oam[128][4];uint8_t layer[153600],rgba[153600];
 FILE *input=fopen(argv[4],"rb");if(!input)return 12;
 if(fread(oam,1,sizeof oam,input)!=sizeof oam||fread(assets.objects,1,sizeof assets.objects,input)!=sizeof assets.objects||fread(palette.objects,1,sizeof palette.objects,input)!=sizeof palette.objects||fgetc(input)!=EOF||fclose(input))return 13;
 input=fopen(argv[5],"rb");if(!input)return 14;if(fread(palette.background,1,sizeof palette.background,input)!=sizeof palette.background||fgetc(input)!=EOF||fclose(input))return 15;
 if(taxi_oam_layer(layer,240,160,oam,&assets,&palette,0x06014000))return 16;
 for(unsigned i=0;i<38400;i++){uint16_t color=palette.background[c.fb[i]];for(unsigned j=0;j<3;j++)rgba[i*4+j]=(uint8_t)(((color>>(j*5))&31)*255/31);rgba[i*4+3]=255;if(layer[i*4+3])memcpy(rgba+i*4,layer+i*4,4);}
 if(fwrite(rgba,1,sizeof rgba,stdout)!=sizeof rgba)return 17;
}
taxi_indexed_asset_free(&c.asset);return 0;}
