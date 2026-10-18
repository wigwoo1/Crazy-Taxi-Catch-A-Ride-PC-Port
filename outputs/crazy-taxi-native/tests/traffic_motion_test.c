#include "../src/indexed_pixel.h"
#include "traffic_motion.h"
#include "world_asset.h"
#include "traffic_follow.h"
#include "traffic_actor.h"
#include "traffic_presentation.h"
#include "traffic_scene.h"
#include "traffic_drive.h"
#include "traffic_assets.h"
#include "traffic_init.h"
#include "world_render.h"
#include "world_clip.h"
#include "world_material.h"
#include "world_frame.h"
#include "raster_dispatch.h"
#include "world_indexed.h"
#include "world_background.h"
#include "destination_panel.h"
#include "material_blink.h"
#include "passenger_render.h"
#include "taxi_shadow_render.h"
#include "render_interpolation.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct {unsigned refs[16],count;int filter;} VisitFixture;
typedef struct {
    TaxiWorldFrame *frame;TaxiIndexedAsset *textures;uint8_t *image,*lookup;
    int32_t draw_shade_depth;unsigned drawn,last_bucket;size_t last_payload;
} IndexedLog;
static int indexed_dispatch(void *context,unsigned kind,size_t payload) {
    IndexedLog *log=context;assert(payload<log->frame->count);
    TaxiWorldFramePacket *p=log->frame->packets+payload;assert(kind==p->kind);
    assert(p->bucket<=log->last_bucket);
    if(p->bucket==log->last_bucket)assert(payload<log->last_payload);
    log->last_bucket=p->bucket;log->last_payload=payload;
    int rc=taxi_world_indexed_packet(log->image,p,log->frame->world,log->textures,
        log->lookup,log->frame->projection.far_plane,log->draw_shade_depth);
    assert(rc>=0);log->drawn+=(unsigned)rc;return 0;
}
static int fixture_visible(void *context,unsigned x,unsigned z){
    (void)z;VisitFixture *f=context;return !f->filter||!x;
}
static int fixture_visit(void *context,unsigned ref){
    VisitFixture *f=context;assert(f->count<16);f->refs[f->count++]=ref;return 0;
}
static int count_visit(void *context,unsigned ref){
    unsigned *counts=context;counts[(ref&0x8000u)?1:0]++;return 0;
}
typedef struct {TaxiWorldFrame *frame;unsigned last_bucket,world_count,traffic_count;size_t last_ordinal;} FrameLog;
static int frame_dispatch(void *context,unsigned kind,size_t payload){
    FrameLog *log=context;unsigned bucket;size_t ordinal;
    if(kind==TAXI_RENDER_TRAFFIC){assert(payload==999);bucket=60;ordinal=0;log->traffic_count++;}
    else{
        assert(payload<log->frame->count);const TaxiWorldFramePacket *packet=log->frame->packets+payload;
        assert(kind==packet->kind);bucket=packet->bucket;ordinal=payload+1;log->world_count++;
        if(kind==TAXI_RENDER_WORLD)assert(packet->polygon.count>=3&&packet->polygon.count<=8);
        else assert(kind==TAXI_RENDER_SPECIAL);
    }
    assert(bucket<=log->last_bucket);
    if(bucket==log->last_bucket)assert(ordinal<log->last_ordinal);
    log->last_bucket=bucket;log->last_ordinal=ordinal;return 0;
}
static void traversal_fixture(void){
    uint8_t material[28]={0};material[16]=5;material[17]=3;material[19]=16;
    unsigned texture=99;
    for(int phase=-100000;phase<=100000;phase++){
        uint32_t product=(uint32_t)phase*4096u;
        int32_t wrapped;memcpy(&wrapped,&product,4);
        int32_t shifted=wrapped/4096-(wrapped<0&&wrapped%4096!=0);
        assert(!taxi_world_material_texture(material,phase,256,&texture));
        assert(texture==(unsigned)(5+shifted%3));
    }
    texture=99;assert(taxi_world_material_texture(material,0,5,&texture)==-1&&texture==99);
    material[16]=255;assert(!taxi_world_material_texture(material,0,0,&texture)&&texture==255);
    uint8_t polygon[24]={0};
    TaxiWorldAsset material_world={.polygons=polygon,.materials=material,.np=1,.nm=1};
    TaxiWorldRenderPolygon geometry={.count=3};
    TaxiWorldMaterialPolygon packet,old_packet;memset(&packet,0x5a,sizeof packet);old_packet=packet;
    material[16]=0;material[17]=1;material[24]=1;
    assert(taxi_world_material_polygon(&material_world,0,&geometry,1,1,&packet)==-1);
    assert(!memcmp(&packet,&old_packet,sizeof packet));
    material[16]=255;polygon[16]=7;polygon[18]=9;polygon[20]=11;
    assert(!taxi_world_material_polygon(&material_world,0,&geometry,1,0,&packet));
    assert(packet.vertices[0].attribute==7*4096&&packet.vertices[1].attribute==9*4096&&packet.vertices[2].attribute==11*4096);
    TaxiTrig identity={0};identity.cosine[0]=256;identity.sine[1024]=256;
    TaxiWorldAsset grid={.nx=100,.nz=100,.sx=1000,.sz=1000,.ox=-50000,.oz=-50000};
    TaxiTrafficProjection projection=taxi_traffic_projection_normal();TaxiWorldRenderBounds bounds;
    assert(!taxi_world_render_bounds(&grid,(TaxiPoint){0},0,0,&identity,&projection,0,0,240,160,&bounds));
    assert(bounds.min_x==26&&bounds.max_x==74&&bounds.min_z==50&&bounds.max_z==88);
    assert(!taxi_world_render_bounds(&grid,(TaxiPoint){0},1024,0,&identity,&projection,0,0,240,160,&bounds));
    assert(bounds.min_x==11&&bounds.max_x==49&&bounds.min_z==26&&bounds.max_z==74);
    assert(!taxi_world_render_bounds(&grid,(TaxiPoint){0},0,1024,&identity,&projection,0,0,240,160,&bounds));
    assert(bounds.min_x==26&&bounds.max_x==74&&bounds.min_z==34&&bounds.max_z==66);
    TaxiWorldRenderBounds old=bounds;grid.sx=0;
    assert(taxi_world_render_bounds(&grid,(TaxiPoint){0},0,0,&identity,&projection,0,0,240,160,&bounds)==-1);
    assert(!memcmp(&bounds,&old,sizeof bounds));
    int32_t special_words[8]={0,0,0,500,-100,0,100,100};
    TaxiWorldAsset special_world={.special_storage=(uint8_t *)special_words,.special_count=1,.nm=1};
    TaxiWorldRenderSpecial special;
    TaxiWorldRenderFrustum sf=taxi_world_render_frustum(0,0,240,160);
    assert(taxi_world_render_special(&special_world,0,(TaxiPoint){0},0,0,&identity,&projection,sf,&special)==1);
    assert(special.depth==500&&special.screen.left==82&&special.screen.top==42&&
        special.screen.right==158&&special.screen.bottom==80);
    TaxiWorldRenderSpecial retained_special=special;special_words[3]=256;
    uint8_t passenger[44]={1};int32_t passenger_xyz[3]={0,0,4096};memcpy(passenger+16,passenger_xyz,12);
    TaxiPassengerRender pr;
    TaxiShadowRender shadow;
    TaxiTrafficActor old_boundary={.z=4096},new_boundary={.z=1024};
    TaxiTrafficProjection boundary_projection=taxi_traffic_projection_normal();TaxiTrafficScreenBounds boundary_screen;
    TaxiTrafficBillboard boundary_billboard=taxi_traffic_billboard((TaxiPoint){0,0,new_boundary.z},0x8e800);
    assert(!taxi_traffic_project(&boundary_billboard,&boundary_projection,&boundary_screen));
    TaxiTrafficActor middle=taxi_render_traffic_actor(&old_boundary,&new_boundary,.5);
    boundary_billboard=taxi_traffic_billboard((TaxiPoint){middle.x,middle.y,middle.z},0x8e800);
    assert(taxi_traffic_project(&boundary_billboard,&boundary_projection,&boundary_screen)==1);
    old_boundary.z=38000;new_boundary.z=38400;
    boundary_billboard=taxi_traffic_billboard((TaxiPoint){0,0,new_boundary.z},0x8e800);
    assert(!taxi_traffic_project(&boundary_billboard,&boundary_projection,&boundary_screen));
    middle=taxi_render_traffic_actor(&old_boundary,&new_boundary,.5);
    boundary_billboard=taxi_traffic_billboard((TaxiPoint){middle.x,middle.y,middle.z},0x8e800);
    assert(taxi_traffic_project(&boundary_billboard,&boundary_projection,&boundary_screen)==1);
    assert(taxi_shadow_render((TaxiPoint){0,999,4096},0,(TaxiPoint){0},0,0,&identity,160,106,&shadow)==1);
    assert(shadow.billboard.top_left.x==-512&&shadow.billboard.top_left.y==320&&shadow.billboard.bottom_right.y==-192);
    assert(shadow.screen.left==96&&shadow.screen.right==144&&shadow.screen.top==65&&shadow.screen.bottom==89);
    TaxiShadowRender retained_shadow=shadow;
    assert(!taxi_shadow_render((TaxiPoint){0,999,1024},0,(TaxiPoint){0},0,0,&identity,160,106,&shadow));
    assert(!memcmp(&shadow,&retained_shadow,sizeof shadow));
    assert(taxi_passenger_render(passenger,(TaxiPoint){0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1);
    assert(pr.scale==0xb1400&&pr.billboard.top_left.x==-177&&pr.billboard.top_left.y==709);
    assert(pr.screen.left==112&&pr.screen.top==47&&pr.screen.right==128&&pr.screen.bottom==80);
    static const unsigned stage_start[4]={0,1,5,9},stage_count[4]={1,4,4,9};
    for(unsigned kind=0;kind<8;kind++)for(unsigned stage=0;stage<4;stage++)for(unsigned frame=0;frame<stage_count[stage];frame++){
        passenger[1]=(uint8_t)kind;passenger[10]=(uint8_t)stage;passenger[8]=(uint8_t)frame;
        assert(taxi_passenger_render(passenger,(TaxiPoint){0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1);
        assert(pr.texture==kind*18+stage_start[stage]+frame&&pr.flip==(stage==3));
        assert(pr.u[0]==(stage==3?(32<<14):0)&&pr.u[1]==pr.u[0]);
        assert(pr.u[2]==(stage==3?0:(32<<14))&&pr.u[3]==pr.u[2]);
        assert(pr.v[0]==(64<<14)&&pr.v[3]==pr.v[0]&&!pr.v[1]&&!pr.v[2]);
    }
    passenger[1]=0;passenger[10]=3;passenger[8]=0;
    assert(taxi_passenger_render(passenger,(TaxiPoint){1,0,0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1&&!pr.flip);
    TaxiPassengerRender retained_pr=pr;
    passenger_xyz[0]=2737;memcpy(passenger+16,passenger_xyz,12);
    assert(!taxi_passenger_render(passenger,(TaxiPoint){2737,0,0},0,(TaxiPoint){0},0,0,0,&identity,&pr));
    assert(!memcmp(&pr,&retained_pr,sizeof pr));
    passenger_xyz[0]=2736;memcpy(passenger+16,passenger_xyz,12);
    assert(taxi_passenger_render(passenger,(TaxiPoint){2736,0,0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1);
    assert(taxi_passenger_render_frustum(passenger,(TaxiPoint){2736,0,0},0,(TaxiPoint){0},0,0,0,&identity,240,106,&pr)==1);
    passenger_xyz[0]=0;memcpy(passenger+16,passenger_xyz,12);pr=retained_pr;
    assert(!taxi_passenger_render(passenger,(TaxiPoint){12800,0,0},0,(TaxiPoint){0},0,0,0,&identity,&pr));
    assert(!memcmp(&pr,&retained_pr,sizeof pr));
    assert(taxi_passenger_render(passenger,(TaxiPoint){12799,0,0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1);
    retained_pr=pr;passenger[10]=0;passenger_xyz[2]=1024;memcpy(passenger+16,passenger_xyz,12);
    assert(!taxi_passenger_render(passenger,(TaxiPoint){0},0,(TaxiPoint){0},0,0,0,&identity,&pr));
    assert(!memcmp(&pr,&retained_pr,sizeof pr));
    passenger_xyz[2]=1025;memcpy(passenger+16,passenger_xyz,12);
    assert(taxi_passenger_render(passenger,(TaxiPoint){0},0,(TaxiPoint){0},0,0,0,&identity,&pr)==1);
    retained_pr=pr;passenger_xyz[2]=38400;memcpy(passenger+16,passenger_xyz,12);
    assert(!taxi_passenger_render(passenger,(TaxiPoint){0,0,38400},0,(TaxiPoint){0},0,0,0,&identity,&pr));
    assert(!memcmp(&pr,&retained_pr,sizeof pr));
    puts("Original normal passenger scale/frame/facing/proximity/depth projection fixtures PASS");
    assert(!taxi_world_render_special(&special_world,0,(TaxiPoint){0},0,0,&identity,&projection,sf,&special));
    assert(!memcmp(&special,&retained_special,sizeof special));special_words[3]=38400;
    assert(!taxi_world_render_special(&special_world,0,(TaxiPoint){0},0,0,&identity,&projection,sf,&special));
    assert(!memcmp(&special,&retained_special,sizeof special));
    uint8_t a[34]={3},b[34]={3},d[32]={2};
    a[28]=2;a[30]=1;a[32]=2;
    b[28]=1;b[30]=0;b[32]=0;b[33]=128;
    d[28]=3;d[30]=0;d[31]=128;
    const uint8_t *cells[4]={a,b,0,d};
    TaxiWorldAsset w={.cells=cells,.nx=2,.nz=2,.np=4};uint8_t seen[4];VisitFixture f={0};
    assert(!taxi_world_render_visit(&w,-1,-1,2,2,seen,4,fixture_visible,fixture_visit,&f));
    unsigned expected[]={2,1,0,0x8000,3,0x8000};
    assert(f.count==6&&!memcmp(f.refs,expected,sizeof expected));
    f=(VisitFixture){.filter=1};
    assert(!taxi_world_render_visit(&w,0,0,1,1,seen,4,fixture_visible,fixture_visit,&f));
    assert(f.count==2&&f.refs[0]==2&&f.refs[1]==1);
    f=(VisitFixture){0};
    assert(!taxi_world_render_visit(&w,5,5,6,6,seen,4,0,fixture_visit,&f)&&!f.count);
    assert(taxi_world_render_visit(&w,0,0,1,1,seen,3,0,fixture_visit,&f)==-1);
}
static int query(void *context,TaxiPoint p,TaxiObstacleHit *hit){
    (void)p;(void)hit;return *(int *)context;
}
static int failed_height(void *context,TaxiPoint p,int32_t *height){
    (void)context;(void)p;(void)height;return -1;
}
#include "view_transform.h"
static void capture_special_fixture(const TaxiWorldFrame *frame,unsigned city,unsigned camera,
    uint32_t far_depth,uint32_t shade_depth,unsigned captured[3],int stress) {
    const char *names[3]={"clipped","unclipped","shaded"};
    for(size_t j=0;j<frame->count;j++) {
        const TaxiWorldFramePacket *packet=frame->packets+j;
        if(packet->kind!=TAXI_RENDER_SPECIAL)continue;
        uint32_t callback=taxi_world_indexed_callback(packet,frame->world);
        int family=callback==0x030069a4?0:callback==0x03006a28?1:callback==0x03006a38?2:-1;
        if(family<0||captured[family]||(stress?(family!=2):(family==2)))continue;
        TaxiTrafficScreenBounds b=packet->special.screen;
        int left=b.left<0?0:b.left,right=b.right>240?240:b.right;
        int top=b.top<0?0:b.top,bottom=b.bottom>160?160:b.bottom;
        if(right-left<4||bottom-top<4)continue;
        unsigned bank;if(stress&&!taxi_indexed_billboard_bank(&bank,(uint32_t)packet->special.depth,far_depth,shade_depth))continue;
        char path[256];snprintf(path,sizeof path,"outputs/crazy-taxi-native/reports/city%u-special-%s-packet.bin",city,names[family]);
        FILE *file=fopen(path,"wb");assert(file);
        uint32_t info[16]={0x31505342,city,camera,packet->material,packet->texture,packet->raster_flags,
            packet->source_index,callback,far_depth,shade_depth,(uint32_t)b.left,(uint32_t)b.top,
            (uint32_t)b.right,(uint32_t)b.bottom,(uint32_t)packet->special.depth,packet->kind};
        assert(fwrite(info,sizeof info,1,file)==1);
        assert(fwrite(frame->world->materials+packet->material*28,28,1,file)==1);fclose(file);captured[family]=1;
        printf("City%u special%s source fixture: camera%u source%u texture%u callback%08x; %s.\n",city,names[family],camera,packet->source_index,packet->texture,callback,stress?"artificial producer-far shading":"source producer-zero phase");
    }
}
int main(int argc,char **argv){
    assert(argc>=4&&argc<=6);
    traversal_fixture();
    TaxiAiPathPoint points[2]={{0,0,0,0},{100,100,0,0}};
    TaxiAiPathAsset road={points,2};TaxiAngleTable angles={0};int kind=0;
    TaxiTrafficActor actor={0};actor.x=10;actor.y=777;actor.impulse_x=600;actor.flags=6;
    assert(taxi_traffic_postimpact_motion(&actor,&road,1,1,0,0,query,&kind,&angles)==0);
    assert(actor.impulse_x==569&&actor.x==19&&actor.y==777&&actor.heading==34&&actor.offset_x==19);
    TaxiTrafficActor saved=actor;kind=-1;
    assert(taxi_traffic_postimpact_motion(&actor,&road,1,1,0,0,query,&kind,&angles)==-1);
    assert(memcmp(&actor,&saved,sizeof actor)==0);
    kind=0;actor.impulse_x=0;actor.offset_x=17;actor.offset_z=0;
    assert(taxi_traffic_postimpact_motion(&actor,&road,1,1,0,0,query,&kind,&angles)==0);
    assert(actor.x==18&&actor.offset_x==1&&actor.flags==6);
    assert(taxi_traffic_postimpact_motion(&actor,&road,1,1,0,0,query,&kind,&angles)==0);
    assert(actor.x==3&&actor.offset_x==0&&actor.flags==7);
    uint8_t runtime[34504];FILE *f=fopen(argv[3],"rb");assert(f);
    assert(fread(runtime,1,sizeof runtime,f)==sizeof runtime&&fgetc(f)==EOF);
    fclose(f);assert(!memcmp(runtime,"CTR1",4));
    taxi_angle_table_decode(&angles,runtime+4+8192+8192+16386);
    TaxiTrig trig;taxi_trig_decode(&trig,runtime+4,runtime+4+8192);
    for(int depth=0;depth<=38400;depth++){
        unsigned bucket=999;assert(!taxi_traffic_depth_bucket(depth,0,38400,&trig,&bucket));
        unsigned phase=(unsigned)((int64_t)depth*1024/38400);
        assert(bucket==(unsigned)(trig.sine[phase]*127/256));
    }
    unsigned bucket=999;
    assert(!taxi_traffic_depth_bucket(0,100,38400,&trig,&bucket)&&bucket==0);
    assert(!taxi_traffic_depth_bucket(40000,0,38400,&trig,&bucket));
    assert(bucket==(unsigned)(trig.sine[1024]*127/256));
    unsigned retained_bucket=bucket;
    assert(taxi_traffic_depth_bucket(100,0,0,&trig,&bucket)==-1&&bucket==retained_bucket);
    TaxiWorldAsset world={0};TaxiAiPathAsset original={0};
    assert(taxi_world_asset_load(&world,argv[1])==0);
    char special_path[1024];size_t world_path_length=strlen(argv[1]);
    assert(world_path_length>=4&&world_path_length+16<sizeof special_path);
    memcpy(special_path,argv[1],world_path_length-4);
    strcpy(special_path+world_path_length-4,"-special.cts");
    assert(!taxi_world_asset_load_special(&world,special_path));
    assert(world.special_count==(strstr(argv[1],"city1")?214u:70u));
    uint8_t *retained_special=world.special_storage;unsigned retained_count=world.special_count;
    assert(taxi_world_asset_load_special(&world,argv[1])==-1);
    assert(world.special_storage==retained_special&&world.special_count==retained_count);
    unsigned special_refs=0;
    for(size_t i=0;i<(size_t)world.nx*world.nz;i++)if(world.cells[i]){
        const uint8_t *cell=world.cells[i];uint32_t refs;memcpy(&refs,cell,4);
        for(unsigned j=0;j<refs;j++){
            unsigned ref=cell[28+2*j]|((unsigned)cell[29+2*j]<<8);
            if((ref&0xc000u)==0x8000u){assert((ref&0x3fffu)<world.special_count);special_refs++;}
        }
    }
    assert(special_refs);
    printf("Original special world records: %u objects, %u validated grid refs and failed replacement retention PASS\n",world.special_count,special_refs);
    assert(taxi_traffic_path_load(&original,argv[2])==0);
    memset(&actor,0,sizeof actor);
    assert(taxi_traffic_path_initialize(original.points,original.count,0,&actor.path)==0);
    actor.x=actor.path.x;actor.z=actor.path.z;actor.flags=6;
    int32_t height;assert(taxi_world_asset_height(&world,(TaxiPoint){actor.x,0,actor.z},&height)>=0);
    actor.y=height;actor.impulse_x=600;
    for(unsigned i=0;i<100;i++){
        assert(taxi_traffic_postimpact_motion(&actor,&original,1,1,0,0,
            taxi_world_asset_obstacle,&world,&angles)==0);
        assert(actor.y==height);
    }
    assert(actor.impulse_x==0&&actor.impulse_z==0);
    memset(&actor,0,sizeof actor);
    assert(taxi_traffic_path_initialize(original.points,original.count,0,&actor.path)==0);
    for(unsigned i=0;i<100;i++){
        assert(taxi_traffic_follow_prepare(&actor,&original,1,1,10000000,10000000,0)==0);
        assert(actor.path.distance==(int32_t)i+1&&actor.x==actor.path.x&&actor.z==actor.path.z);
    }
    actor.collision_timer=2;int32_t retained=actor.path.distance;
    assert(taxi_traffic_follow_prepare(&actor,&original,1,1,10000000,10000000,0)==0&&actor.path.distance==retained&&actor.collision_timer==1);
    assert(taxi_traffic_follow_prepare(&actor,&original,1,1,actor.path.x,actor.path.z,0)==1&&actor.collision_timer==0);
    /* Clean USA-ROM route31 rows8217->8218: 0x08006CE8 subtracts globalDelta5 from raw+0x58 (120->115). */
    TaxiTrafficActor source_timer={0};source_timer.collision_timer=120;
    assert(taxi_traffic_postimpact_prepare(&source_timer,5,1000000,1000000,0)==0);
    assert(source_timer.collision_timer==115);
    if(strstr(argv[1],"city1")){
        /* Clean USA-ROM route31 slot3: source row8217 -> 8218, before 0x08006CE8. */
        char source_road_path[1024];size_t source_path_len=strlen(argv[2]);
        assert(source_path_len<sizeof source_road_path);memcpy(source_road_path,argv[2],source_path_len+1);
        char *source_sep=strrchr(source_road_path,'\\');if(!source_sep)source_sep=strrchr(source_road_path,'/');assert(source_sep);
        snprintf(source_road_path,sizeof source_road_path,"%.*scity1-road-146.ctrp",(int)(source_sep+1-source_road_path),source_road_path);
        TaxiAiPathAsset source_road={0};assert(!taxi_traffic_path_load(&source_road,source_road_path));
        TaxiTrafficActor source_actor={.flags=0,.type=0,.countdown=-7,.x=132434,.y=-30720,.z=-235333,
            .heading=-1,.route=146,.collision_timer=120,.impulse_x=-2608,.impulse_z=-3378,
            .path={.distance=5369,.segment=12,.x=132580,.z=-234724,.heading=1}};
        TaxiAcceleration source_player={0};source_player.gears.speed_544=34251;
        TaxiTrafficGlobals source_globals={.cooldown_e4=60,.near_miss_25d3=0,.voice_25d6=8,
            .combo_flag_9aa6=0,.combo_9aa8=0,.combo_9aac=0};
        uint32_t source_rng=1779866770;TaxiTrafficActorEffects source_effects={0};
        unsigned source_heading=(8070856u>>12)&4095u;
        assert(!taxi_traffic_actor_postimpact(&source_actor,&source_road,&source_player,
            133044,-234543,trig.sine[source_heading],trig.cosine[source_heading],5,5,1,1,
            &source_globals,&source_rng,0,taxi_world_asset_obstacle,&world,&angles,&source_effects));
        printf("ROUTE31_POSTIMPACT_SLOT3_NATIVE x=%d y=%d z=%d heading=%d timer=%d impulse=%d,%d offset=%d,%d\n",
            source_actor.x,source_actor.y,source_actor.z,source_actor.heading,source_actor.collision_timer,
            source_actor.impulse_x,source_actor.impulse_z,source_actor.offset_x,source_actor.offset_z);
        assert(!source_effects.collision&&source_actor.collision_timer==115);
        assert(source_actor.x==132268&&source_actor.y==-30720&&source_actor.z==-235549&&source_actor.heading==169);
        assert(source_actor.impulse_x==-2007&&source_actor.impulse_z==-2601);
        assert(source_actor.offset_x==-312&&source_actor.offset_z==-825&&source_actor.path.distance==5369&&source_actor.path.segment==12);
        assert(source_actor.path.x==132580&&source_actor.path.z==-234724&&source_actor.path.heading==1);
    }
    TaxiTrafficActor miss={0};miss.path.z=-767;miss.z=-10;
    assert(taxi_traffic_near_miss(&miss,0,0,0x855d,0,0,256)==1);
    assert(taxi_traffic_near_miss(&miss,0,0,0x855c,0,0,256)==0);
    assert(taxi_traffic_near_miss(&miss,0,0,0x855d,1,0,256)==0);
    miss.path.z=-768;assert(taxi_traffic_near_miss(&miss,0,0,0x855d,0,0,256)==0);
    TaxiAcceleration power={0};TaxiGearEffects effects={0};
    power.gears.speed_544=100;power.gears.gear=3;power.gears.boost_75c=-1;
    power.gears.ratios[0]=4096;power.gears.final_ratio=4096;power.gears.scale_5cc=60;
    power.gears.timer_e6c=12;power.gears.timer_e80=99;
    assert(taxi_traffic_player_response(&power,1,&effects)==0);
    assert(power.gears.speed_544==25&&power.gears.gear==1&&power.gears.rpm_538==25);
    assert(power.scale_e84==4096&&!power.gears.timer_e6c&&!power.gears.timer_e80);
    /* Real city road/terrain, explicit player collision fixture. */
    memset(&actor,0,sizeof actor);
    assert(!taxi_traffic_path_initialize(original.points,original.count,0,&actor.path));
    actor.x=actor.path.x+100;actor.z=actor.path.z;actor.y=height;
    power.gears.speed_544=100;power.gears.gear=2;power.gears.ratios[1]=4096;
    power.gears.timer_e6c=12;power.scale_e84=123;
    TaxiTrafficGlobals globals={0};globals.combo_9aa8=99;
    uint32_t rng=123;TaxiTrafficActorEffects actor_effects;
    assert(!taxi_traffic_actor_postimpact(&actor,&original,&power,actor.path.x,actor.path.z,
        0,0,1,1,1,1,&globals,&rng,0,taxi_world_asset_obstacle,&world,&angles,&actor_effects));
    assert(actor_effects.collision&&actor.collision_timer==120&&actor.path.distance==0);
    assert(power.gears.speed_544==37&&power.gears.gear==2&&power.gears.timer_e6c==12&&power.scale_e84==123);
    assert(globals.cooldown_e4==60&&!globals.combo_9aa8&&actor_effects.effects.random_calls==3);
    assert(actor.impulse_x==4050&&actor.y==height);
    /* Failure after collision effects must roll back the whole native actor step. */
    actor.x=actor.path.x+100;actor.z=actor.path.z;power.gears.speed_544=100;
    TaxiTrafficActor old_actor=actor;TaxiAcceleration old_power=power;
    TaxiTrafficGlobals old_globals=globals;uint32_t old_rng=rng;kind=-1;
    assert(taxi_traffic_actor_postimpact(&actor,&original,&power,actor.path.x,actor.path.z,
        0,0,1,1,1,1,&globals,&rng,0,query,&kind,&angles,&actor_effects)==-1);
    assert(!memcmp(&actor,&old_actor,sizeof actor)&&!memcmp(&power,&old_power,sizeof power));
    assert(!memcmp(&globals,&old_globals,sizeof globals)&&rng==old_rng);
    actor.x=actor.path.x;actor.z=actor.path.z;actor.countdown=0;
    actor.collision_timer=0;actor.heading=0;actor.path.heading=100;
    assert(taxi_traffic_presentation_prepare(&actor,actor.x,actor.z,1,0,
        taxi_world_asset_height,&world)==1);
    assert(actor.heading==-16);
    assert(taxi_world_asset_height(&world,(TaxiPoint){actor.x,actor.y,actor.z},&height)>=0&&height==actor.y);
    old_actor=actor;
    assert(taxi_traffic_presentation_prepare(&actor,actor.x,actor.z,1,0,failed_height,0)==-1);
    assert(!memcmp(&actor,&old_actor,sizeof actor));
    int32_t retained_heading=actor.heading;
    assert(taxi_traffic_presentation_prepare(&actor,actor.x-23040,actor.z,1,1,
        taxi_world_asset_height,&world)==1&&actor.heading==retained_heading);
    assert(taxi_traffic_presentation_prepare(&actor,actor.x-23041,actor.z,1,0,
        taxi_world_asset_height,&world)==0&&actor.countdown==180);
    /* Camera direction uses original tables and sequential fixed-point transforms. */
    TaxiTrafficView view;
    for(int yaw=0;yaw<4096;yaw++){
        assert(!taxi_traffic_view(&actor,(TaxiPoint){actor.x-1024,actor.y,actor.z-2048},yaw,0,&trig,&angles,&view));
        TaxiPoint reference=taxi_rotate_y((TaxiPoint){1024,0,2048},-yaw,&trig);
        assert(view.camera_space.x==reference.x&&view.camera_space.z==reference.z&&view.camera_space.y==0);
        assert(view.frame<16);
    }
    assert(!taxi_traffic_view(&actor,(TaxiPoint){actor.x,actor.y-1024,actor.z-2048},0,1024,&trig,&angles,&view));
    assert(view.camera_space.x==0&&view.camera_space.y==-2048&&view.camera_space.z==1024);
    TaxiTrafficBillboard bounds=taxi_traffic_billboard((TaxiPoint){100,200,300},0x8e800);
    assert(bounds.top_left.x==-470&&bounds.top_left.y==770&&bounds.top_left.z==300);
    assert(bounds.bottom_right.x==670&&bounds.bottom_right.y==200&&bounds.bottom_right.z==300);
    bounds=taxi_traffic_billboard((TaxiPoint){0,0,300},0x01000000);
    assert(bounds.top_left.x==16384&&bounds.bottom_right.x==-16384&&bounds.top_left.y==16384);
    TaxiTrafficProjection projection={120,80,120,0,10000};TaxiTrafficScreenBounds screen={0};
    TaxiTrafficProjection viewport={0,0,0,12,34567};
    taxi_traffic_projection_viewport(&viewport,1,3,240,160);
    assert(viewport.center_x==120&&viewport.center_y==81&&viewport.focal==192);
    assert(viewport.near_plane==12&&viewport.far_plane==34567);
    taxi_traffic_projection_viewport(&viewport,-3,-5,0,0);
    assert(viewport.center_x==-1&&viewport.center_y==-2);
    viewport=taxi_traffic_projection_normal();
    assert(viewport.center_x==120&&viewport.center_y==80&&viewport.focal==192);
    assert(viewport.near_plane==256&&viewport.far_plane==38400);
    bounds=taxi_traffic_billboard((TaxiPoint){0,0,1024},0x8e800);
    assert(taxi_traffic_project(&bounds,&viewport,&screen)==0);
    bounds.top_left.z=bounds.bottom_right.z=1025;
    assert(taxi_traffic_project(&bounds,&viewport,&screen)==1);
    bounds=taxi_traffic_billboard((TaxiPoint){100,200,1200},0x8e800);
    assert(taxi_traffic_project(&bounds,&projection,&screen)==1);
    assert(screen.left==73&&screen.top==3&&screen.right==187&&screen.bottom==60);
    TaxiTrafficScreenBounds saved_screen=screen;
    bounds.top_left.z=bounds.bottom_right.z=768;
    assert(!taxi_traffic_project(&bounds,&projection,&screen)&&!memcmp(&screen,&saved_screen,sizeof screen));
    bounds.top_left.z=bounds.bottom_right.z=10000;
    assert(!taxi_traffic_project(&bounds,&projection,&screen));
    TaxiAiPathAsset roads[256]={{0}};roads[0]=original;
    TaxiTrafficGrid grid={0};uint32_t scales[3]={0x8e800,0xa0600,0x6b8cc};
    uint8_t timer=2;uint32_t counter=0;TaxiTrafficSubmission submission;
    actor.countdown=0;actor.flags=1;actor.route=0;actor.type=2;actor.collision_timer=0;
    TaxiTrafficScene scene={.player=&power,.player_position={10000000,0,10000000},
        .camera={actor.x-1024,actor.y,actor.z-2048},.frame_delta=1,.global_delta=1,
        .grid=&grid,.roads=roads,.trig=&trig,.angles=&angles,.globals=&globals,
        .timer_25d5=&timer,.rng=&rng,.selection_counter=&counter,.spacing=20,.scales=scales,
        .obstacle=taxi_world_asset_obstacle,.height=taxi_world_asset_height,.terrain=&world,
        .submissions=&submission,.submission_count=1};
    assert(taxi_traffic_scene_update(&actor,1,&scene)==0);
    assert(actor.path.distance==1&&submission.visible&&submission.type==2&&submission.scale==0x6b8cc);
    assert(submission.position.x==actor.x&&submission.position.y==actor.y&&timer==1);
    scene.paused=1;int32_t old_distance=actor.path.distance;retained_heading=actor.heading;
    assert(!taxi_traffic_scene_update(&actor,1,&scene));
    assert(actor.path.distance==old_distance&&actor.heading==retained_heading&&submission.visible&&timer==0);
    scene.paused=0;TaxiDriveWorld drive={0};drive.driver.power=power;
    drive.world.height.position=(TaxiPoint){actor.path.x-100,actor.y,actor.path.z};
    drive.driver.power.gears.speed_544=100;actor.flags=1;actor.collision_timer=0;
    assert(!taxi_traffic_drive_update(&actor,1,&scene,&drive));
    assert(submission.effects.collision&&drive.driver.power.gears.speed_544==25);
    assert(drive.world.speed_544==25&&drive.driver.motion.motion.orientation.speed_544==25);
    assert(drive.world.height.scale_e84==4096&&drive.driver.motion.motion.scale_e84==4096);
    assert(scene.player_sine==trig.sine[0]&&scene.player_cosine==trig.cosine[0]);
    scene.paused=1;
    for(unsigned direction=0;direction<4096;direction++){
        drive.world.heading_1c=(int32_t)(direction*4096u+4095u);
        assert(!taxi_traffic_drive_update(&actor,1,&scene,&drive));
        assert(scene.player_sine==trig.sine[direction]&&scene.player_cosine==trig.cosine[direction]);
    }
    drive.world.heading_1c=-1;
    assert(!taxi_traffic_drive_update(&actor,1,&scene,&drive));
    assert(scene.player_sine==trig.sine[4095]&&scene.player_cosine==trig.cosine[4095]);
    TaxiPassengerScene passengers={0};passengers.random_state=123;
    passengers.session.trip.active_1694=1;passengers.session.end.flag_9aa6=1;
    passengers.session.trip.voice_25d6=12;passengers.pickup.voice_25d6=14;
    scene.paused=0;scene.sound_enabled=1;actor.flags=1;actor.collision_timer=0;
    drive.world.height.position=(TaxiPoint){actor.path.x-100,actor.y,actor.path.z};
    drive.driver.power.gears.speed_544=100;
    assert(!taxi_traffic_game_update(&actor,1,&scene,&drive,&passengers));
    assert(submission.effects.collision&&submission.effects.effects.random_calls==3);
    assert(scene.rng==&passengers.random_state&&passengers.random_state!=123);
    assert(!passengers.session.end.flag_9aa6);
    assert(passengers.session.trip.voice_25d6>=6&&passengers.session.trip.voice_25d6<=8);
    assert(passengers.pickup.voice_25d6==passengers.session.trip.voice_25d6);
    /* Entire source grid/bank, delayed spawn followed by native actor updates. */
    char directory[1024];assert(strlen(argv[2])<sizeof directory);
    strcpy(directory,argv[2]);char *separator=strrchr(directory,'/');
    char *backslash=strrchr(directory,'\\');if(backslash&&(!separator||backslash>separator))separator=backslash;
    assert(separator);*separator=0;
    TaxiTrafficAssets bank={0};unsigned city=strstr(argv[1],"city1")?1u:2u;
    assert(!taxi_traffic_assets_load(&bank,directory,city));
    size_t cell_index=0;
    while(cell_index<(size_t)bank.grid.width*bank.grid.height&&
        (!bank.grid.cells[cell_index].source_pointer||!bank.grid.cells[cell_index].phases[0]))++cell_index;
    assert(cell_index<(size_t)bank.grid.width*bank.grid.height);
    scene.camera.x=bank.grid.origin_x+(int32_t)(cell_index%bank.grid.width)*bank.grid.cell_x;
    scene.camera.z=bank.grid.origin_z+(int32_t)(cell_index/bank.grid.width)*bank.grid.cell_z;
    scene.camera.y=0;scene.grid=&bank.grid;scene.roads=bank.roads;scene.paused=0;
    scene.player=&power;scene.player_position=(TaxiPoint){10000000,0,10000000};scene.rng=&rng;
    memset(&actor,0,sizeof actor);counter=0;
    assert(!taxi_traffic_actors_reinitialize(&actor,1,1,scales));
    unsigned spawns=0,visible=0;
    for(unsigned tick=0;tick<600;tick++){
        int rc=taxi_traffic_scene_update(&actor,1,&scene);assert(rc>=0);spawns+=(unsigned)rc;
        if(tick<179)assert(!counter&&!submission.visible);
        if(submission.visible){++visible;assert(submission.type<3&&submission.view.frame<16);}
    }
    assert(spawns&&visible&&counter==spawns);
    printf("City%u source-grid delayed traffic: %u spawns, %u visible submissions over600 ticks PASS\n",city,spawns,visible);
    unsigned world_visible=0,world_partial=0,world_depth_clipped=0;
    TaxiPoint world_camera={original.points[0].x,height+1000,original.points[0].z};
    TaxiTrafficProjection wp=taxi_traffic_projection_normal();
    TaxiWorldRenderFrustum frustum=taxi_world_render_frustum(0,0,240,160);
    assert(frustum.horizontal==160&&frustum.vertical==106);
    frustum=taxi_world_render_frustum(1,3,240,160);
    assert(frustum.horizontal==159&&frustum.vertical==104);
    frustum=taxi_world_render_frustum(240,160,0,0);
    assert(frustum.horizontal==-160&&frustum.vertical==-106);
    frustum=taxi_world_render_frustum(INT32_MIN,0,INT32_MAX,160);
    assert(frustum.horizontal==0&&frustum.vertical==106);
    for(int span=-100000;span<=100000;span++){
        frustum=taxi_world_render_frustum(0,0,span,span);
        assert(frustum.horizontal==(int64_t)span*128/192&&frustum.vertical==frustum.horizontal);
    }
    frustum=taxi_world_render_frustum(0,0,240,160);
    for(unsigned orientation=0;orientation<4;orientation++)for(unsigned i=0;i<world.np;i++){
        TaxiWorldRenderPolygon packet;memset(&packet,0x5a,sizeof packet);
        TaxiWorldRenderPolygon retained=packet;
        int rc=taxi_world_render_polygon(&world,i,world_camera,(int32_t)orientation*1024,
            0,&trig,&wp,frustum.horizontal,frustum.vertical,&packet);
        assert(rc>=0);
        if(!rc){assert(!memcmp(&packet,&retained,sizeof packet));continue;}
        world_visible++;world_partial+=packet.clip_flags!=0;
        assert(packet.count==3||packet.count==4);
        const uint8_t *poly=world.polygons+24*i;uint32_t sum=0;
        for(unsigned j=0;j<packet.count;j++){
            unsigned vi=poly[8+2*j]|((unsigned)poly[9+2*j]<<8);
            int32_t xyz[3];memcpy(xyz,world.vertices+16*vi,sizeof xyz);
            TaxiTrafficActor vertex_actor={.x=xyz[0],.y=xyz[1],.z=xyz[2]};
            TaxiTrafficView vertex_view;
            assert(!taxi_traffic_view(&vertex_actor,world_camera,(int32_t)orientation*1024,
                0,&trig,&angles,&vertex_view));
            assert(!memcmp(&packet.vertices[j],&vertex_view.camera_space,sizeof(TaxiPoint)));
            sum+=(uint32_t)vertex_view.camera_space.z;
        }
        int32_t signed_sum;memcpy(&signed_sum,&sum,4);
        int32_t expected=packet.count==3?signed_sum/3:
            signed_sum/4-(signed_sum<0&&signed_sum%4!=0);
        assert(packet.depth==expected);
        unsigned bucket;assert(!taxi_traffic_depth_bucket(expected,0,wp.far_plane,&trig,&bucket));
        assert(packet.bucket==bucket);
        TaxiWorldMaterialPolygon material_packet;
        assert(!taxi_world_material_polygon(&world,i,&packet,12345,256,&material_packet));
        assert(material_packet.count==packet.count&&material_packet.texture==world.materials[28*poly[0]+16]);
        for(unsigned j=0;j<packet.count;j++){
            assert(material_packet.vertices[j].x==packet.vertices[j].x&&
                material_packet.vertices[j].y==packet.vertices[j].y&&material_packet.vertices[j].z==packet.vertices[j].z);
            if(material_packet.texture==255){
                unsigned shade=poly[16+2*j]|((unsigned)poly[17+2*j]<<8);
                assert(material_packet.vertices[j].attribute==(int32_t)(shade<<12));
            }else{
                assert(material_packet.vertices[j].u==(int32_t)poly[16+2*j]<<14);
                assert(material_packet.vertices[j].v==(int32_t)poly[17+2*j]<<14);
            }
        }
        if(packet.clip_flags&48){
            TaxiWorldClippedPolygon clipped;
            int far_clip=!(packet.clip_flags&16);
            int rc=taxi_world_clip_depth(material_packet.vertices,packet.count,
                far_clip?wp.far_plane:wp.near_plane,far_clip,&clipped);
            assert(rc>=0);
            if(rc){
                world_depth_clipped++;
                for(unsigned j=0;j<clipped.count;j++)assert(far_clip?
                    clipped.vertices[j].z<=wp.far_plane:clipped.vertices[j].z>=wp.near_plane);
                assert(taxi_world_clip_project(&clipped,&wp,0,0,240,160)>=0);
            }
        }
    }
    assert(world_visible&&world_partial);
    assert(world_depth_clipped);
    printf("City%u original world polygon producer: %u candidates, %u partial, %u depth-clipped PASS\n",city,world_visible,world_partial,world_depth_clipped);
    unsigned special_visible=0;
    for(unsigned orientation=0;orientation<4;orientation++)for(unsigned i=0;i<world.special_count;i++){
        TaxiWorldRenderSpecial result;memset(&result,0x5a,sizeof result);
        TaxiWorldRenderSpecial retained=result;
        int rc=taxi_world_render_special(&world,i,world_camera,(int32_t)orientation*1024,
            0,&trig,&wp,frustum,&result);
        assert(rc>=0);
        if(!rc){assert(!memcmp(&result,&retained,sizeof result));continue;}
        special_visible++;
        const uint8_t *record=world.special_storage+32*i;int32_t words[8];memcpy(words,record,32);
        TaxiTrafficActor anchor={.x=words[1],.y=words[2],.z=words[3]};TaxiTrafficView reference;
        assert(!taxi_traffic_view(&anchor,world_camera,(int32_t)orientation*1024,0,&trig,&angles,&reference));
        assert(result.material==record[0]&&result.depth==reference.camera_space.z);
        assert(result.billboard.top_left.x==(int32_t)((int64_t)reference.camera_space.x+words[4]));
        assert(result.billboard.top_left.y==(int32_t)((int64_t)reference.camera_space.y+words[7]));
        assert(result.billboard.bottom_right.x==(int32_t)((int64_t)reference.camera_space.x+words[6]));
        assert(result.billboard.bottom_right.y==(int32_t)((int64_t)reference.camera_space.y+words[5]));
    }
    printf("City%u native special billboard projection: %u visible requests across4 poses PASS\n",city,special_visible);
    unsigned anchor_visible=0;
    for(unsigned i=0;i<world.special_count;i++){
        const uint8_t *record=world.special_storage+32*i;int32_t words[8];memcpy(words,record,32);
        TaxiPoint camera={words[1],words[2],words[3]-4096};TaxiWorldRenderSpecial result;
        int rc=taxi_world_render_special(&world,i,camera,0,0,&trig,&wp,frustum,&result);
        assert(rc>=0);
        if(rc){anchor_visible++;assert(result.depth>wp.near_plane&&result.depth<wp.far_plane);}
    }
    assert(anchor_visible);
    printf("City%u native special scenery anchor fixtures: %u visible requests PASS\n",city,anchor_visible);
    uint8_t *seen=malloc(world.np);assert(seen);unsigned visits[2]={0};
    assert(!taxi_world_render_visit(&world,0,0,(int32_t)world.nx-1,(int32_t)world.nz-1,
        seen,world.np,0,count_visit,visits));
    unsigned seen_count=0;for(unsigned i=0;i<world.np;i++)seen_count+=seen[i]!=0;
    assert(visits[0]==seen_count&&seen_count);
    printf("City%u row-major source grid: %u unique normal polygons, %u raw special refs PASS\n",city,visits[0],visits[1]);
    free(seen);
    unsigned bounded_visits=0;
    seen=malloc(world.np);assert(seen);
    for(unsigned orientation=0;orientation<4;orientation++){
        TaxiWorldRenderBounds bounds;
        assert(!taxi_world_render_bounds(&world,world_camera,(int32_t)orientation*1024,
            128,&trig,&wp,0,0,240,160,&bounds));
        unsigned selected[2]={0};
        assert(!taxi_world_render_visit(&world,bounds.min_x,bounds.min_z,bounds.max_x,bounds.max_z,
            seen,world.np,0,count_visit,selected));
        assert(selected[0]<=visits[0]);bounded_visits+=selected[0];
    }
    assert(bounded_visits);free(seen);
    printf("City%u source frustum grid bounds: %u total normal visits across4 poses PASS\n",city,bounded_visits);
    size_t frame_capacity=(size_t)world.np+world.special_count;
    TaxiWorldFramePacket *frame_packets=calloc(frame_capacity,sizeof *frame_packets);
    TaxiRenderPacket *queue_packets=calloc(frame_capacity+1,sizeof *queue_packets);
    uint8_t *normal_seen=malloc(world.np),*special_seen=malloc(world.special_count);
    assert(frame_packets&&queue_packets&&normal_seen&&special_seen);
    uint8_t *packet_ids=calloc(frame_capacity,1);assert(packet_ids);
    TaxiRenderQueue render_queue;
    TaxiWorldFrame frame={.world=&world,.camera=world_camera,.trig=&trig,.projection=wp,
        .left=0,.top=0,.right=240,.bottom=160,.texture_count=149,.phase=123,
        .queue=&render_queue,.packets=frame_packets,.capacity=frame_capacity,
        .normal_seen=normal_seen,.normal_seen_size=world.np,
        .special_seen=special_seen,.special_seen_size=world.special_count};
    unsigned frame_total=0;
    for(unsigned orientation=0;orientation<4;orientation++){
        taxi_render_queue_init(&render_queue,queue_packets,frame_capacity+1);
        assert(!taxi_render_queue_push(&render_queue,60,TAXI_RENDER_TRAFFIC,999));
        frame.yaw=(int32_t)orientation*1024;
        assert(!taxi_world_frame_build(&frame));
        memset(packet_ids,0,frame_capacity);
        for(size_t i=0;i<frame.count;i++){
            size_t id=frame_packets[i].source_index+
                (frame_packets[i].kind==TAXI_RENDER_SPECIAL?world.np:0);
            assert(id<frame_capacity&&!packet_ids[id]);packet_ids[id]=1;
        }
        assert(render_queue.count==frame.count+1);frame_total+=(unsigned)frame.count;
        FrameLog log={.frame=&frame,.last_bucket=128,.last_ordinal=SIZE_MAX};
        assert(!taxi_render_queue_consume(&render_queue,frame_dispatch,&log));
        assert(log.world_count==frame.count&&log.traffic_count==1&&!render_queue.count);
    }
    assert(frame_total);
    printf("City%u source scenery frame composition: %u queued packets across4 poses; mixed traffic order PASS\n",city,frame_total);
    if(argc>=5) {
        FILE *dest=fopen(argv[4],"rb");assert(dest);
        uint32_t header[4];assert(fread(header,4,4,dest)==4&&!memcmp(header,"CTP1",4));
        assert(fseek(dest,(long)header[1]*32,SEEK_CUR)==0);
        unsigned total=0,shaded=0,unshaded=0,clipped=0,flat=0,zero_depth_drawn=0;
        unsigned normal_callbacks[128]={0},zero_callbacks[128]={0};
        unsigned far_expanded=0;
        unsigned normal_total=0,flat_cache_captured=0,special_captured[3]={0};
        uint8_t diagnostic_cache[512]={0};
        frame.material_cache=diagnostic_cache;frame.material_cache_size=512;
        frame.shade_depth=wp.far_plane; /* Artificial producer-far shading stress fixture. */
        TaxiIndexedAsset indexed={0};unsigned drawn=0,changed=0;
        uint8_t image[38400],identity[2560];
        if(argc>=6) assert(!taxi_indexed_asset_load(&indexed,argv[5]));
        for(unsigned j=0;j<2560;++j)identity[j]=(uint8_t)j; /* Diagnostic identity shade banks. */
        for(unsigned i=0;i<header[3];i++) {
            uint8_t record[48];assert(fread(record,1,48,dest)==48);
            TaxiViewTransform view;assert(!taxi_view_transform_destination(&view,record,48,&trig));
            assert(view.words[5]==0); /* All original stock destination cameras have zero roll. */
            frame.camera=(TaxiPoint){view.words[0],view.words[1],view.words[2]};
            frame.pitch=view.words[3];frame.yaw=view.words[4];frame.phase=0;
            taxi_render_queue_init(&render_queue,queue_packets,frame_capacity+1);
            assert(!taxi_world_frame_build(&frame));
            assert(frame.count==render_queue.count);
            size_t initial_count=frame.count;
            memset(image,111,sizeof image);
            for (size_t p=0;p<frame.count;++p) {
                TaxiWorldFramePacket *packet=frame.packets+p;
                if (packet->kind!=TAXI_RENDER_WORLD) {
                    assert(packet->kind==TAXI_RENDER_SPECIAL && packet->raster_flags_valid);
                    ++shaded;continue;
                }
                ++normal_total;
                assert(packet->raster_flags_valid && !(packet->raster_flags&256));
                uint32_t callback=taxi_raster_callback_global(packet->raster_flags,
                    packet->texture!=255,world.materials[packet->material*28+20]);
                uint32_t near_target=callback==0x0300691c?taxi_raster_callback_global(
                    (uint16_t)((packet->raster_flags&~(1u|2u|512u))|4u),
                    packet->texture!=255,world.materials[packet->material*28+20]):0;
                if (callback==0x0300693c || callback==0x0300694c || callback==0x03006a2c || callback==0x030069b0 || near_target==0x03006950 || near_target==0x03006918 || near_target==0x03006934 || near_target==0x030069ac) ++shaded;
                assert(callback>=0x03006900 && callback<0x03006b00 && !(callback&3));
                ++normal_callbacks[(callback-0x03006900)/4];
                if(callback==0x030069b0 && packet->polygon.count>4)++far_expanded;
            }
            if(argc>=6) {
                IndexedLog log={&frame,&indexed,image,identity,wp.far_plane,0,128,SIZE_MAX};
                assert(!taxi_render_queue_consume(&render_queue,indexed_dispatch,&log));
                drawn+=log.drawn;
                unsigned pixels=0;
                for(unsigned pixel=0;pixel<38400;++pixel)pixels+=image[pixel]!=111;
                if(!changed && pixels) {
                    char path[256];snprintf(path,sizeof path,
                        "outputs/crazy-taxi-native/reports/city%u-partial-indexed.raw",city);
                    FILE *capture=fopen(path,"wb");assert(capture);
                    assert(fwrite(image,1,sizeof image,capture)==sizeof image);fclose(capture);
                    printf("City%u partial capture: original destination%u, %u polygons, %u changed pixels.\n",city,i,log.drawn,pixels);
                }
                changed+=pixels;
            }
            if(argc>=6 && i==0)for(size_t j=0;j<frame.count;j++) {
                const TaxiWorldFramePacket *packet=frame.packets+j;
                if(packet->kind!=TAXI_RENDER_WORLD||taxi_world_indexed_callback(packet,&world)!=0x0300693c)continue;
                int minx=10000,maxx=-10000,miny=10000,maxy=-10000;
                for(unsigned k=0;k<packet->polygon.count;k++) {
                    const TaxiWorldClipVertex *v=packet->polygon.vertices+k;
                    if(v->screen_x<minx)minx=v->screen_x;if(v->screen_x>maxx)maxx=v->screen_x;
                    if(v->screen_y<miny)miny=v->screen_y;if(v->screen_y>maxy)maxy=v->screen_y;
                }
                if(maxx-minx<24||maxy-miny<24)continue;
                char path[256];snprintf(path,sizeof path,
                    "outputs/crazy-taxi-native/reports/city%u-shaded-stress-packet.bin",city);
                FILE *capture=fopen(path,"wb");assert(capture);
                uint32_t info[11]={0x31504853,city,i,packet->polygon.count,packet->material,
                    packet->texture,packet->raster_flags,packet->source_index,0x0300693c,
                    (uint32_t)wp.far_plane,(uint32_t)wp.far_plane};
                assert(fwrite(info,sizeof info,1,capture)==1);
                assert(fwrite(world.materials+packet->material*28,28,1,capture)==1);
                for(unsigned k=0;k<packet->polygon.count;k++) {
                    const TaxiWorldClipVertex *v=packet->polygon.vertices+k;
                    int32_t values[5]={v->screen_x,v->screen_y,v->u,v->v,v->z};
                    assert(fwrite(values,sizeof values,1,capture)==1);
                }
                fclose(capture);
                printf("City%u shaded source geometry fixture: source%u texture%u; artificial producer-far/consumer-far state, not original runtime selection.\n",city,packet->source_index,packet->texture);
                break;
            }
            if(argc>=6)capture_special_fixture(&frame,city,i,(uint32_t)wp.far_plane,(uint32_t)wp.far_plane,special_captured,1);
            /* Source normal producer state; panel projection ownership is unresolved. */
            frame.shade_depth=0;
            taxi_render_queue_init(&render_queue,queue_packets,frame_capacity+1);
            assert(!taxi_world_frame_build(&frame) && frame.count==initial_count);
            if(argc>=6)capture_special_fixture(&frame,city,i,(uint32_t)wp.far_plane,0,special_captured,0);
            unsigned supported=0;
            for(size_t p=0;p<frame.count;++p) {
                TaxiWorldFramePacket *packet=frame.packets+p;
                if(packet->kind!=TAXI_RENDER_WORLD) {
                    assert(packet->kind==TAXI_RENDER_SPECIAL && packet->raster_flags_valid);
                    uint32_t special_callback=taxi_raster_callback_global(packet->raster_flags,
                        packet->texture!=255,world.materials[packet->material*28+20]);
                    supported+=special_callback==0x030069a4 || special_callback==0x03006a28;
                    continue;
                }
                uint32_t callback=taxi_raster_callback_global(packet->raster_flags,
                    packet->texture!=255,world.materials[packet->material*28+20]);
                if(callback==0x03006948)++unshaded;
                assert(callback>=0x03006900 && callback<0x03006b00 && !(callback&3));
                ++zero_callbacks[(callback-0x03006900)/4];
                if(callback==0x03006914)++clipped;
                int is_flat=callback==0x03006950 || callback==0x0300690c || callback==0x03006918;
                flat+=(unsigned)is_flat;
                uint32_t near_target=callback==0x0300691c?taxi_raster_callback_global(
                    (uint16_t)((packet->raster_flags&~(1u|2u|512u))|4u),
                    packet->texture!=255,world.materials[packet->material*28+20]):0;
                supported+=callback==0x03006948 || callback==0x0300693c || callback==0x03006914 || callback==0x030069b0 || callback==0x03006934 || callback==0x03006944 || callback==0x030069ac || callback==0x030069b8 || is_flat || near_target==0x03006950 || near_target==0x03006918 || near_target==0x03006934 || near_target==0x030069ac;
            }
            if(argc>=6) {
                memset(image,111,sizeof image);
                assert(!taxi_world_background_scene(image,&indexed,0,
                    frame.pitch,frame.yaw,0,0,240,160));
                /* b474 creates packets while engine+44 is still0; b47c
                   changes it to far only for the following queue consumer. */
                IndexedLog log={&frame,&indexed,image,identity,wp.far_plane,0,128,SIZE_MAX};
                assert(!taxi_render_queue_consume(&render_queue,indexed_dispatch,&log));
                assert(log.drawn==supported);zero_depth_drawn+=log.drawn;
                assert(frame.shade_depth==0); /* Consumer owns separate draw state. */
                if(i==0) {
                    char path[256];snprintf(path,sizeof path,
                        "outputs/crazy-taxi-native/reports/city%u-source-phase-indexed.raw",city);
                    FILE *capture=fopen(path,"wb");assert(capture);
                    assert(fwrite(image,1,sizeof image,capture)==sizeof image);fclose(capture);
                }
            }
            if(argc>=6 && i==0) {
                unsigned captured[4]={0};
                for(size_t j=0;j<frame.count;j++) {
                    const TaxiWorldFramePacket *packet=frame.packets+j;
                    if(packet->kind!=TAXI_RENDER_WORLD)continue;
                    uint32_t callback=taxi_world_indexed_callback(packet,&world);
                    int axis=(callback==0x03006934||callback==0x03006944)?0:
                        (callback==0x030069ac||callback==0x030069b8)?1:callback==0x03006914?2:callback==0x03006948?3:-1;
                    if(axis<0||captured[axis])continue;
                    const char *names[4]={"rows","columns","clipped","unclipped"};
                    if(axis>=2) {
                        int minx=10000,maxx=-10000,miny=10000,maxy=-10000;
                        for(unsigned k=0;k<packet->polygon.count;k++) {
                            const TaxiWorldClipVertex *v=packet->polygon.vertices+k;
                            if(v->screen_x<minx)minx=v->screen_x;if(v->screen_x>maxx)maxx=v->screen_x;
                            if(v->screen_y<miny)miny=v->screen_y;if(v->screen_y>maxy)maxy=v->screen_y;
                        }
                        if(maxx-minx<24||maxy-miny<24)continue;
                    }
                    char path[256];snprintf(path,sizeof path,
                        "outputs/crazy-taxi-native/reports/city%u-perspective-%s-packet.bin",city,names[axis]);
                    FILE *capture=fopen(path,"wb");assert(capture);
                    uint32_t info[9]={0x31505655,city,i,packet->polygon.count,
                        packet->material,packet->texture,packet->raster_flags,packet->source_index,callback};
                    assert(fwrite(info,sizeof info,1,capture)==1);
                    assert(fwrite(world.materials+packet->material*28,28,1,capture)==1);
                    for(unsigned vertex=0;vertex<packet->polygon.count;vertex++) {
                        const TaxiWorldClipVertex *v=packet->polygon.vertices+vertex;
                        int32_t values[5]={v->screen_x,v->screen_y,v->u,v->v,v->z};
                        assert(fwrite(values,sizeof values,1,capture)==1);
                    }
                    fclose(capture);captured[axis]=1;
                    printf("City%u source phase perspective %s fixture: source%u texture%u callback%08x.\n",
                        city,names[axis],packet->source_index,packet->texture,callback);
                }
                printf("City%u camera0 perspective fixture availability: rows%u columns%u.\n",city,captured[0],captured[1]);
            }
            frame.shade_depth=wp.far_plane;
            memset(diagnostic_cache,1,sizeof diagnostic_cache);
            taxi_render_queue_init(&render_queue,queue_packets,frame_capacity+1);
            assert(!taxi_world_frame_build(&frame) && frame.count==initial_count);
            for (size_t p=0;p<frame.count;++p) if (frame.packets[p].kind==TAXI_RENDER_WORLD)
                assert(frame.packets[p].raster_flags_valid && (frame.packets[p].raster_flags&256));
            if(argc>=6) {
                unsigned normal_world=0;
                for(size_t p=0;p<frame.count;++p)normal_world+=frame.packets[p].kind==TAXI_RENDER_WORLD;
                memset(image,111,sizeof image);
                IndexedLog log={&frame,&indexed,image,identity,wp.far_plane,0,128,SIZE_MAX};
                assert(!taxi_render_queue_consume(&render_queue,indexed_dispatch,&log));
                assert(log.drawn==frame.count && log.drawn>=normal_world);
            }
            if(argc>=6 && !flat_cache_captured)for(size_t j=0;j<frame.count;j++) {
                const TaxiWorldFramePacket *packet=frame.packets+j;
                if(packet->kind!=TAXI_RENDER_WORLD || packet->texture==255 || !(taxi_world_indexed_callback(packet,&world)==0x03006918 || taxi_world_indexed_callback(packet,&world)==0x03006950 || taxi_world_indexed_callback(packet,&world)==0x0300690c))continue;
                char path[256];snprintf(path,sizeof path,"outputs/crazy-taxi-native/reports/city%u-flat-cache-packet.bin",city);
                FILE *capture=fopen(path,"wb");assert(capture);
                uint32_t info[8]={0x31505441,city,i,packet->polygon.count,packet->material,packet->texture,packet->raster_flags,packet->source_index};
                assert(fwrite(info,sizeof info,1,capture)==1);
                assert(fwrite(world.materials+packet->material*28,28,1,capture)==1);
                for(unsigned vertex=0;vertex<packet->polygon.count;vertex++) {
                    const TaxiWorldClipVertex *v=packet->polygon.vertices+vertex;
                    int32_t values[4]={v->screen_x,v->screen_y,v->attribute,v->z};
                    assert(fwrite(values,sizeof values,1,capture)==1);
                }
                fclose(capture);flat_cache_captured=1;
                printf("City%u actual cached-flat packet capture: destination%u, source%u, material%u, texture%u, flags%u; artificial cache1 shading fixture.\n",city,i,packet->source_index,packet->material,packet->texture,packet->raster_flags);
                break;
            }
            memset(diagnostic_cache,0,sizeof diagnostic_cache);
            if(argc>=6) {
                uint8_t panel_image[4096];
                frame.shade_depth=0;
                TaxiWorldFrame saved=frame;
                assert(!taxi_destination_panel(panel_image,image,&frame,record,
                    &indexed,0,identity,111));
                assert(!memcmp(&saved,&frame,sizeof frame));
                for(unsigned x=0;x<64;++x) {
                    assert(panel_image[x]==111 && panel_image[63*64+x]==111);
                    assert(panel_image[x*64]==111 && panel_image[x*64+63]==111);
                }
                TaxiMaterialBlink selection={.clock=23,.destination=-1};
                memset(selection.cache,1,sizeof selection.cache);
                TaxiDestinationPanelContext panel_context={panel_image,image,&frame,
                    &indexed,0,identity,111};
                assert(!taxi_material_blink_select(&selection,(int32_t)i,record,48,0,
                    taxi_destination_panel_selection,&panel_context));
                assert(selection.clock==23 && selection.destination==(int32_t)i);
                for(unsigned j=0;j<512;++j)assert(!selection.cache[j]);
                assert(taxi_material_blink_update(&selection,1,0)==1);
                assert(selection.clock==24);
                assert(!taxi_material_blink_select(&selection,-1,NULL,0,0,NULL,NULL));
                assert(!memcmp(&saved,&frame,sizeof frame));
                frame.shade_depth=wp.far_plane;
            }
            total+=(unsigned)frame.count;
        }
        fclose(dest);assert(total);
        if(argc>=6) {
            assert(drawn==total && zero_depth_drawn==total);
            printf("City%u original unshaded special billboards drawn: %u PASS.\n",city,zero_depth_drawn-normal_total);
            printf("City%u all normal scenery packets rendered: %u PASS; special packets %u also rendered.\n",
                city,normal_total,total-normal_total); /* Remaining in normal shade state only. */
        }
        assert(far_expanded);
        printf("City%u original far-clipped polygons exceeding4 vertices: %u PASS.\n",city,far_expanded);
        for(unsigned c=0;c<128;++c)if(normal_callbacks[c] || zero_callbacks[c])
            printf("City%u original callback%08x: normal%u zero-depth%u selections.\n",
                city,0x03006900+c*4,normal_callbacks[c],zero_callbacks[c]);
        assert(unshaded);
        printf("City%u zero shade-depth diagnostic: %u original unshaded selections, %u supported draws PASS.\n",city,unshaded,zero_depth_drawn);
        assert(clipped);
        printf("City%u clipped unshaded selections: %u original polygons PASS.\n",city,clipped);
        assert(flat);
        printf("City%u original material flat selections: %u polygons PASS.\n",city,flat);
        if(argc>=6){assert(drawn==shaded && changed);printf("City%u partial indexed diagnostic: %u original polygons, %u changed pixels; identity shade fixture, all tested packets rendered.\n",city,drawn,changed);taxi_indexed_asset_free(&indexed);}
        printf("City%u diagnostic original-camera packets: %u shaded-family selections; both cache states PASS.\n",city,shaded);
        printf("City%u destination panel camera queues: %u records, %u packets PASS (raster pending).\n",city,header[3],total);
    }
    free(frame_packets);free(queue_packets);free(normal_seen);free(special_seen);free(packet_ids);
    taxi_traffic_assets_free(&bank);
    taxi_ai_path_free(&original);taxi_world_asset_free(&world);
    puts("Original traffic terrain/impulse/rejoin composition fixtures PASS");return 0;
}





