#include "../src/world_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void load(const char *path,void *out,size_t n){FILE *f=fopen(path,"rb");if(!f||fread(out,1,n,f)!=n)exit(2);fclose(f);}
int main(int argc,char **argv){if(argc!=6)return 1;TaxiWorldAsset world={0};TaxiIndexedAsset textures={0};if(taxi_world_asset_load(&world,argv[1])||taxi_world_asset_load_special(&world,argv[2])||taxi_indexed_asset_load(&textures,argv[3]))return 3;
 uint8_t *ew=malloc(262144),*iw=malloc(65536);if(!ew||!iw)return 4;load(argv[4],ew,262144);load(argv[5],iw,65536);
 TaxiTrig trig;memcpy(trig.sine,ew+0x2c4f0,16384);memcpy(trig.cosine,ew+0x304f0,16384);uint32_t view=word(iw+0x6814);if(view<0x02000000||view+72>0x02040000)return 5;int32_t v[18],corners[8][4];memcpy(v,ew+view-0x02000000,72);memcpy(corners,iw+0x6860,128);
 uint8_t *seen=calloc(world.np,1),*special=calloc(world.special_count,1);TaxiRenderPacket *storage=calloc(16384,sizeof *storage);TaxiWorldFramePacket *packets=calloc(16384,sizeof *packets);if(!seen||!special||!storage||!packets)return 4;
 TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,16384);
 TaxiWorldFrame frame={.world=&world,.camera={v[0],v[1],v[2]},.yaw=v[4],.pitch=v[3],.trig=&trig,.projection={120,80,192,256,38400},.right=240,.bottom=160,.bounds_corners=corners,.texture_count=textures.count,.textures=&textures,.material_cache=ew+0x2b6ec,.material_cache_size=512,.shade_depth=0,.queue=&queue,.packets=packets,.capacity=16384,.normal_seen=seen,.normal_seen_size=world.np,.special_seen=special,.special_seen_size=world.special_count,.normal_stamps=(uint32_t *)(ew+0x1b6ec),.normal_stamp_count=world.np,.special_stamps=(uint32_t *)(ew+0x276ec),.special_stamp_count=world.special_count,.generation=word(iw+0x6844)+1};
 int rc=taxi_world_frame_build(&frame);if(rc)return 6;
 printf("{\"generation\":%u,\"packets\":[",frame.generation);
 for(size_t i=0;i<frame.count;i++){TaxiWorldFramePacket *p=packets+i;printf("%s{\"kind\":%u,\"source_index\":%u,\"material\":%u,\"texture\":%u,\"bucket\":%u,\"flags\":%u,\"vertices\":%u}",i?",":"",p->kind,p->source_index,p->material,p->texture,p->bucket,p->raster_flags,p->polygon.count);}
 printf("],\"normal_stamps\":[");for(unsigned i=0;i<world.np;i++)printf("%s%u",i?",":"",frame.normal_stamps[i]);printf("],\"special_stamps\":[");for(unsigned i=0;i<world.special_count;i++)printf("%s%u",i?",":"",frame.special_stamps[i]);puts("]}");return 0;}
