#include "../src/world_frame.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){if(argc!=8)return 2;TaxiWorldAsset worlds[2]={{0}};TaxiIndexedAsset indexed[2]={{0}};for(unsigned i=0;i<2;i++)if(taxi_world_asset_load(worlds+i,argv[i+1])||taxi_indexed_asset_load(indexed+i,argv[i+4]))return 3;for(unsigned i=0;i<2;i++)if(taxi_world_asset_load_special(worlds+i,argv[i+6]))return 3;FILE *file=fopen(argv[3],"rb");unsigned char raw[16388];if(!file||fread(raw,1,sizeof raw,file)!=sizeof raw)return 4;fclose(file);TaxiTrig trig;taxi_trig_decode(&trig,raw+4,raw+8196);unsigned city,ref;int cx,cy,cz,yaw,pitch;while(scanf("%u %u %d %d %d %d %d",&city,&ref,&cx,&cy,&cz,&yaw,&pitch)==7){if(city>1||ref>=worlds[city].np)return 5;TaxiWorldAsset w=worlds[city];
uint8_t *seen=calloc(w.np,1),*special=calloc(w.special_count,1),cache[512]={0};
TaxiRenderPacket *storage=calloc(16384,sizeof(*storage));TaxiWorldFramePacket *packet=calloc(16384,sizeof(*packet));
if(!seen||!special||!storage||!packet)return 6;TaxiRenderQueue q;taxi_render_queue_init(&q,storage,16384);
TaxiWorldFrame f={.world=&w,.camera={cx,cy,cz},.yaw=yaw,.pitch=pitch,.trig=&trig,.projection={120,80,192,256,38400},.right=240,.bottom=160,.texture_count=indexed[city].count,.textures=indexed+city,.material_cache=cache,.material_cache_size=512,.queue=&q,.packets=packet,.capacity=16384,.normal_seen=seen,.normal_seen_size=w.np,.special_seen=special,.special_seen_size=w.special_count};
int rc=taxi_world_frame_build(&f);if(rc)return 7;printf("{\"city\":%u,\"trace_index\":%u,\"packets\":[",city,ref);
for(size_t k=0;k<f.count;k++){
 TaxiWorldFramePacket *p=packet+k;
 printf("%s{\"kind\":%u,\"source_index\":%u,\"material\":%u,\"texture\":%u,\"flags\":%u,\"bucket\":%u,\"draw_culled\":%u,\"vertices\":[",k?",":"",p->kind,p->source_index,p->material,p->texture,p->raster_flags,p->bucket,p->draw_culled);
 for(unsigned i=0;i<p->polygon.count;i++){
  TaxiWorldClipVertex v=p->polygon.vertices[i];
  printf("%s[%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld]",i?",":"",(long)v.screen_x,(long)v.screen_y,(long)v.x,(long)v.y,(long)v.z,(long)v.u,(long)v.v,(long)v.attribute);
 }
 printf("],\"rect\":[%ld,%ld,%ld,%ld]}",(long)p->special.screen.left,(long)p->special.screen.top,(long)p->special.screen.right,(long)p->special.screen.bottom);
}puts("]}");free(seen);free(special);free(storage);free(packet);}for(unsigned i=0;i<2;i++){taxi_world_asset_free(worlds+i);taxi_indexed_asset_free(indexed+i);}return 0;}
