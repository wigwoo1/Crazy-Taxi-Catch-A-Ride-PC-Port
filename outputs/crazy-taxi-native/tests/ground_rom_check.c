/* Development check only: scans original cell centres using native height math.
   ROM is data, never executed. Python launcher verifies its full SHA-256. */
#include "../src/ground.h"
#include "../src/movement.h"
#include "../src/motion.h"
#include "../src/body_pose.h"
#include "../src/height_update.h"
#include "../src/world_update.h"
#include "../src/drive_world.h"
#include "../src/vehicle_config.h"
#include "../src/spawn_placement.h"
#include "../src/simulation_start.h"
#include "../src/world_asset.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "rom_vehicle_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
static uint8_t *rom;
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24); }
static unsigned u16(const uint8_t *p) { return p[0]|(p[1]<<8); }
static const uint8_t *region(uint32_t address,size_t size) {
    if(address<0x8000000 || address>0x8800000 || size>0x8800000-address) { fputs("Invalid ROM reference\n",stderr);exit(1); }
    return rom+address-0x8000000;
}
typedef struct {
    const uint8_t *header,*grid,*vertices,*faces,*materials;
    unsigned nx,nz,nv,np,nm;
} Scene;
static int obstacle_query(void *context,TaxiPoint point,TaxiObstacleHit *hit) {
    Scene *s=context;
    int32_t sx=(int32_t)u32(s->header+0x40),sz=(int32_t)u32(s->header+0x44);
    if(sx<=0 || sz<=0) return -1;
    int32_t x=(int32_t)((uint32_t)point.x-u32(s->header+0x48))/sx;
    int32_t z=(int32_t)((uint32_t)point.z-u32(s->header+0x50))/sz;
    *hit=(TaxiObstacleHit){.height=INT32_MIN,.polygon=65535};
    if(x<0 || z<0 || (unsigned)x>=s->nx || (unsigned)z>=s->nz) return 0;
    uint32_t address=u32(s->grid+((size_t)z*s->nx+(unsigned)x)*4);
    if(!address) return 0;
    const uint8_t *cell=region(address,28);unsigned count=u32(cell+24);
    return taxi_obstacle_candidates(s->vertices,s->nv,s->faces,s->np,s->materials,s->nm,
        region(address+28,(size_t)count*2),count,point,hit);
}
static int ground_query(void *context,TaxiPoint point,TaxiGroundHit *hit) {
    Scene *s=context;
    int32_t sx=(int32_t)u32(s->header+0x40),sz=(int32_t)u32(s->header+0x44);
    if(sx<=0||sz<=0) return -1;
    int32_t x=(int32_t)((uint32_t)point.x-u32(s->header+0x48))/sx;
    int32_t z=(int32_t)((uint32_t)point.z-u32(s->header+0x50))/sz;
    *hit=(TaxiGroundHit){.height=INT32_MIN,.polygon=65535};
    if(x<0||z<0||(unsigned)x>=s->nx||(unsigned)z>=s->nz) return 0;
    uint32_t address=u32(s->grid+((size_t)z*s->nx+(unsigned)x)*4);
    if(!address) return 0;
    unsigned count=u32(region(address,28)+24);
    return taxi_ground_candidates(s->vertices,s->nv,s->faces,s->np,
        region(address+28,(size_t)count*2),count,point,hit);
}
int main(int argc,char **argv) {
    if(argc!=3) return 1;
    FILE *file=fopen(argv[1],"rb");if(!file) return 1;
    rom=malloc(0x800000);if(!rom) { fclose(file);return 1; }
    if(fread(rom,1,0x800000,file)!=0x800000 || fgetc(file)!=EOF) { fclose(file);free(rom);return 1; }fclose(file);
    uint32_t scenes[]={0x87556cc,0x8755734,0x87ebb90};
    static TaxiTrig trig;
    static TaxiNormalAngles angles;
    static TaxiAngleTable traction_angles;
    taxi_angle_table_decode(&traction_angles,region(0x87f09ee,514));
    if(taxi_normal_angles_decode(&angles,region(0x87ec9ec,8193*2),8193*2)) {free(rom);return 1;}
    taxi_trig_decode(&trig,region(0x84a0d08,8192),region(0x84a2d08,8192));
    for(int scene=0;scene<3;scene++) {
        const char *names[]={"city1","city2","startup"};char asset_path[4096];
        assert(snprintf(asset_path,sizeof asset_path,"%s/%s.ctw",argv[2],names[scene])<(int)sizeof asset_path);
        TaxiWorldAsset native={0};assert(!taxi_world_asset_load(&native,asset_path));
        const uint8_t *h=region(scenes[scene],104);
        unsigned nx=u16(h+0x3c),nz=u16(h+0x3e),nv=u16(h+8),np=u16(h+10);
        const uint8_t *grid=region(u32(h+0x60),(size_t)nx*nz*4);
        const uint8_t *v=region(u32(h+0x24),(size_t)nv*16),*f=region(u32(h+0x28),(size_t)np*24);
        unsigned nm=u16(h+4);
        Scene view={h,grid,v,f,region(u32(h+0x1c),(size_t)nm*28),nx,nz,nv,np,nm};
        unsigned path_count=u16(h+0x10);const uint8_t *paths=region(u32(h+0x34),(size_t)path_count*20);
        int found_spawn=0;
        for(unsigned path=0;path<path_count;path++) {
            const uint8_t *entry=paths+path*20;
            if(memcmp(entry+4,"GRID_01\0",8)) continue;
            TaxiSpawnMarker marker;assert(!taxi_spawn_marker_decode(&marker,region(u32(entry+16),16),16));
            TaxiWorldState placed={0};int32_t back=0;
            assert(taxi_spawn_place(&placed,&back,&marker,0,ground_query,&view)==1);
            assert(placed.height.position.y==placed.ground_36c+64);
            for(unsigned config=0;config<2;config++) {
                TaxiDriveWorld fresh;TaxiDriveWorldContext fresh_context;
                const uint8_t *template=region(config?0x84a5794:0x84a554c,0x248);
                assert(taxi_simulation_start(&fresh,&fresh_context,template,0x248,&marker,0,ground_query,&view)==1);
                assert(!fresh.driver.power.gears.gear&&!fresh.driver.power.gears.rpm_538);
                assert(fresh.driver.motion.motion.scale_7c8==4096&&fresh.driver.motion.motion.orientation.flags_358==5);
                assert(fresh.world.height.flags_28==16&&fresh.world.material_e60==65535);
                assert(!memcmp(&fresh.world.height.position,&placed.height.position,sizeof(TaxiPoint)));
                assert(fresh.world.height.body.denominator_754==57344&&fresh.world.height.body.denominator_758==28672);
                TaxiDriveWorldEffects fresh_effects;
                for(unsigned tick=0;tick<600;tick++) {
                    TaxiCommand cmd={0};cmd.bytes[4]=(uint8_t)(tick<400?4:8);
                    TaxiGearInput input={.delta=1,.frame_delta=1,.action2=(uint8_t)(tick<400),.action3=(uint8_t)(tick>=400)};
                    assert(!taxi_drive_world_step(&fresh,&fresh_context,&cmd,&input,0,&trig,&traction_angles,&angles,
                        taxi_world_asset_obstacle,taxi_world_asset_ground,&native,&fresh_effects));
                    if(!tick) assert(fresh.driver.power.gears.gear==1);
                }
                printf("  original-start config %u after600ticks: %d,%d,%d speed%d\n",config,
                    fresh.world.height.position.x,fresh.world.height.position.y,fresh.world.height.position.z,fresh.world.speed_544);
                TaxiDriveWorld old=fresh;TaxiDriveWorldContext old_context=fresh_context;
                assert(taxi_simulation_start(&fresh,&fresh_context,template,4,&marker,0,ground_query,&view)==-1);
                assert(!memcmp(&fresh,&old,sizeof fresh)&&!memcmp(&fresh_context,&old_context,sizeof fresh_context));
            }
            printf("scene %d original GRID_01: %d,%d,%d heading %u\n",scene,placed.height.position.x,
                placed.height.position.y,placed.height.position.z,marker.heading);found_spawn=1;break;
        }
        assert(found_spawn);
        unsigned hits=0,misses=0,contacts=0,nonzero=0;
        unsigned blockers=0,motion_collisions=0,motion_queries=0;
        unsigned vehicle_impacts=0;uint32_t impact_checksum=0;
        unsigned generated_probes=0,generated_collisions=0;
        unsigned aligned=0,slopes=0,height_cycles=0;uint32_t alignment_checksum=0;
        unsigned world_steps=0,world_hits=0,world_collisions=0;uint32_t world_checksum=0;
        unsigned drive_runs=0,drive_steps=0,drive_collisions=0,drive_misses=0,drive_airborne=0;
        uint32_t drive_checksum=0;
        for(unsigned z=0;z<nz;z++) for(unsigned x=0;x<nx;x++) {
            uint32_t ptr=u32(grid+((size_t)z*nx+x)*4);if(!ptr) continue;
            const uint8_t *cell=region(ptr,28);uint32_t count=u32(cell+24);
            const uint8_t *refs=region(ptr+28,(size_t)count*2);
            TaxiPoint p={(int32_t)(u32(h+0x48)+x*u32(h+0x40)+u32(h+0x40)/2),0,
                         (int32_t)(u32(h+0x50)+z*u32(h+0x44)+u32(h+0x44)/2)};
            TaxiGroundHit hit;
            int result=taxi_ground_candidates(v,nv,f,np,refs,count,p,&hit);
            TaxiGroundHit native_hit;int native_result=taxi_world_asset_ground(&native,p,&native_hit);
            assert(native_result==result);
            if(result==1) {
                assert(native_hit.height==hit.height&&native_hit.polygon==hit.polygon&&native_hit.material==hit.material);
                assert(!memcmp(&native_hit.normal,&hit.normal,sizeof(TaxiPoint)));
                assert(native_hit.contact_valid==hit.contact_valid);
                if(hit.contact_valid) assert(native_hit.contact_value==hit.contact_value);
            }
            if(result<0) {free(rom);return 1;}
            if(result) {
                hits++;
                TaxiWorldContact contact={0};
                if(!taxi_world_contact(&contact,&hit)) {free(rom);return 1;}
                TaxiBodyPose pose={.denominator_754=57344,.denominator_758=28672};
                for(unsigned tick=0;tick<64;tick++) {
                    if(taxi_body_springs(&pose)||taxi_body_align(&pose,contact.normal,1,&angles)) {free(rom);return 1;}
                }
                aligned++;if(contact.normal.x||contact.normal.z) slopes++;
                alignment_checksum=alignment_checksum*16777619u^(uint32_t)pose.orientation_6f4.x;
                alignment_checksum=alignment_checksum*16777619u^(uint32_t)pose.orientation_6f4.z;
                TaxiHeightState height={.position={p.x,hit.height,p.z},.displacement_y=33,.scale_e84=4096,.body=pose};
                TaxiHeightGlobals height_globals={0};TaxiHeightEffects height_effects;
                TaxiHeightInput height_input={.delta=1};
                if(taxi_height_update(&height,&height_globals,&contact,&height_input,&angles,&height_effects)||
                   !height_effects.took_off||height.position.y!=hit.height+33) {free(rom);return 1;}
                height.displacement_y=-1;
                if(taxi_height_update(&height,&height_globals,&contact,&height_input,&angles,&height_effects)||
                   height_effects.landed||height.position.y!=hit.height+32) {free(rom);return 1;}
                if(taxi_height_update(&height,&height_globals,&contact,&height_input,&angles,&height_effects)||
                   !height_effects.landed||height.position.y!=hit.height||(height.flags_28&8)) {free(rom);return 1;}
                height_cycles++;
                /* Distributed diagnostic starts, not original spawn points.
                   Both templates use first gear and unit movement scaling. */
                if((hits-1)%256==0) for(unsigned config=0;config<2;config++) {
                    const uint8_t *template=region(config?0x84a5794:0x84a554c,0x248);
                    TaxiVehicleConfig setup;assert(!taxi_vehicle_config_decode(&setup,template,0x248));
                    TaxiDriveWorld car={0};car.driver.power=load(template);
                    car.driver.steering_rate_57a=half(template,0x57a);
                    car.driver.motion.motion.orientation.flags_358=4;
                    car.driver.motion.motion.orientation.state_564=1;
                    car.driver.motion.motion.dimension_594=setup.dimension_594;
                    car.driver.motion.motion.scale_7c8=4096;
                    car.driver.motion.capacity_570=setup.capacity_570;car.driver.motion.capacity_574=setup.capacity_574;
                    car.world.height.position=(TaxiPoint){p.x,hit.height,p.z};car.world.ground_36c=hit.height;
                    car.world.height.body.denominator_754=57344;car.world.height.body.denominator_758=28672;
                    car.world.heading_1c=car.world.heading_20=(int32_t)(((x+z)*37u&4095)*4096);
                    TaxiDriveWorldContext ctx={0};TaxiDriveWorldEffects fx;
                    for(unsigned tick=0;tick<600;tick++) {
                        TaxiCommand cmd={0};cmd.bytes[4]=(uint8_t)(tick<400?4:8);
                        if(tick>=120&&tick<180) cmd.bytes[4]|=(uint8_t)(config?2:1);
                        TaxiGearInput input={.delta=1,.frame_delta=1,.action2=(uint8_t)(tick<400),.action3=(uint8_t)(tick>=400)};
                        if(taxi_drive_world_step(&car,&ctx,&cmd,&input,0,&trig,&traction_angles,&angles,
                            taxi_world_asset_obstacle,taxi_world_asset_ground,&native,&fx)) {
                            fprintf(stderr,"Combined scene drive failed: scene %d cell %u,%u config %u tick %u speed %d\n",
                                scene,x,z,config,tick,car.driver.power.gears.speed_544);free(rom);return 1;
                        }
                        drive_steps++;drive_collisions+=(unsigned)fx.world.movement.collided;
                        drive_misses+=(unsigned)!fx.world.ground_hit;drive_airborne+=(unsigned)((car.world.height.flags_28&8)!=0);
                        assert(car.driver.power.gears.speed_544==car.world.speed_544);
                        assert(!memcmp(&car.driver.motion.motion.orientation.position,&car.world.height.position,sizeof(TaxiPoint)));
                        drive_checksum=drive_checksum*16777619u^(uint32_t)car.world.height.position.x;
                        drive_checksum=drive_checksum*16777619u^(uint32_t)car.world.height.position.y;
                        drive_checksum=drive_checksum*16777619u^(uint32_t)car.world.height.position.z;
                    }
                    drive_runs++;
                }
                TaxiWorldState world={.height={.position={p.x,hit.height,p.z},.scale_e84=4096,
                    .body={.denominator_754=57344,.denominator_758=28672}},
                    .ground_36c=hit.height,.displacement_x=128,.speed_544=50000,
                    .red_374=128,.green_375=128,.blue_376=128};
                TaxiHeightGlobals world_globals={0};TaxiWorldEffects world_effects;
                for(unsigned tick=0;tick<8;tick++) {
                    if(taxi_world_update(&world,&world_globals,&height_input,&trig,&angles,
                        obstacle_query,ground_query,&view,&world_effects)) {free(rom);return 1;}
                    world_steps++;world_hits+=(unsigned)world_effects.ground_hit;
                    world_collisions+=(unsigned)world_effects.movement.collided;
                    world_checksum=world_checksum*16777619u^(uint32_t)world.height.position.x;
                    world_checksum=world_checksum*16777619u^(uint32_t)world.height.position.y;
                    world_checksum=world_checksum*16777619u^(uint32_t)world.height.position.z;
                    world_checksum=world_checksum*16777619u^world.angle_18;
                }
                if(hit.contact_valid) {
                    contacts++;
                    if(hit.contact_value>0xffffff) {free(rom);return 1;}
                    if(hit.contact_value) nonzero++;
                }
            }else misses++;
            TaxiObstacleHit obstacle;
            int obstacle_kind=obstacle_query(&view,p,&obstacle);
            TaxiObstacleHit native_obstacle;
            assert(taxi_world_asset_obstacle(&native,p,&native_obstacle)==obstacle_kind);
            if(obstacle_kind>0) {
                assert(native_obstacle.height==obstacle.height&&native_obstacle.polygon==obstacle.polygon&&native_obstacle.edge_count==obstacle.edge_count);
                for(unsigned j=0;j<obstacle.edge_count;j++) assert(!memcmp(&native_obstacle.vertices[j],&obstacle.vertices[j],sizeof(TaxiPoint)));
            }
            if(obstacle_kind<0) {free(rom);return 1;}
            if(obstacle_kind==2) blockers++;
            TaxiMovementResult movement;
            if(taxi_move_position(p,(TaxiPoint){512,0,0},0,obstacle_query,&view,&movement)<0) {free(rom);return 1;}
            motion_queries+=movement.queries;
            if(movement.collided) motion_collisions++;
            TaxiVehicleBody body={.position=p,.impact={.angle_18=17,.angle_1a=9,
                .angle_1c=(int32_t)(((x+z)*37u&4095)*4096),.speed_544=45520}};
            TaxiImpactEffects effects;
            if(taxi_move_vehicle(&body,(TaxiPoint){512,0,0},0,0,&trig,obstacle_query,&view,&movement,&effects)<0) {free(rom);return 1;}
            if(movement.collided) {
                vehicle_impacts++;
                if(body.state_e84!=4096 || effects.effect_slot>5 || effects.sound_table_index<41 || effects.sound_table_index>43) {free(rom);return 1;}
                impact_checksum=impact_checksum*16777619u^(uint32_t)body.impact.speed_544;
                impact_checksum=impact_checksum*16777619u^(uint32_t)body.impact.state_748;
            }
            TaxiMotionState motion={.orientation={.position=p,.angle_18=17,.angle_1a=9,
                .heading_1c=(int32_t)(((x+z)*37u&4095)*4096),.speed_544=45520,.flags_358=4},
                .steering={.angle_578=(uint16_t)((x&1)?512:3584)},
                .scale_7c8=4096,.scale_e84=4096,.dimension_594=2000};
            motion.orientation.heading_20=motion.orientation.heading_1c;
            TaxiOrientationContext turn_context={.delta=1};TaxiOrientationEffects turn_effects;
            TaxiCommand command={0};
            if(taxi_motion_prepare(&motion,&turn_context,&command,&trig,&turn_effects)<0) {free(rom);return 1;}
            body=(TaxiVehicleBody){.position=motion.orientation.position,.impact={
                .angle_18=motion.orientation.angle_18,.angle_1a=motion.orientation.angle_1a,
                .angle_1c=motion.orientation.heading_1c,.angle_69c=motion.orientation.drift_69c,
                .speed_544=motion.orientation.speed_544}};
            if(taxi_move_vehicle(&body,motion.displacement,0,0,&trig,obstacle_query,&view,&movement,&effects)<0) {free(rom);return 1;}
            generated_probes++;if(movement.collided) generated_collisions++;
        }
        printf("scene %d: %u hits, %u misses, %u contact values (%u nonzero)\n",scene,hits,misses,contacts,nonzero);
        printf("  obstacle centres: %u; 512-unit movement probes: %u collisions, %u queries\n",blockers,motion_collisions,motion_queries);
        printf("  integrated vehicle impacts: %u; diagnostic checksum: %08x\n",vehicle_impacts,(unsigned)impact_checksum);
        printf("  steering-generated movement probes: %u; collisions: %u\n",generated_probes,generated_collisions);
        printf("  native normal-alignment probes: %u (%u sloped); checksum: %08x\n",aligned,slopes,(unsigned)alignment_checksum);
        printf("  native takeoff/flight/landing cycles on sampled ground: %u\n",height_cycles);
        printf("  connected world steps: %u; ground hits: %u; collisions: %u; checksum: %08x\n",world_steps,world_hits,world_collisions,(unsigned)world_checksum);
        printf("  combined driver: %u runs, %u steps, %u collisions, %u ground misses, %u airborne; checksum: %08x\n",
            drive_runs,drive_steps,drive_collisions,drive_misses,drive_airborne,(unsigned)drive_checksum);
        if(!hits) {free(rom);return 1;}
        taxi_world_asset_free(&native);
    }
    free(rom);return 0;
}

