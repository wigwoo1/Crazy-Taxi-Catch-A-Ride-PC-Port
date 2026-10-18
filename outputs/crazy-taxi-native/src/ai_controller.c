#include "ai_controller.h"
#include "ai_command.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t asr12(int32_t x){return bits(((uint32_t)x>>12)|(x<0?0xfff00000u:0));}
int taxi_ai_world_prepare(TaxiDriveWorld *v,const TaxiDriveWorld *player,TaxiCommand *command,uint32_t *counter,const TaxiAiPathAsset *path){
    if(!v||!player||!command||!counter)return -1;
    TaxiAiVehicle ai={.position={v->world.height.position.x,v->world.height.position.y,v->world.height.position.z},
        .heading_1c=v->world.heading_1c,.heading_20=v->world.heading_20,.speed_544=v->world.speed_544,
        .phase_378=v->world.contact_378,.command_4=command->bytes[4],.command_c=(uint16_t)(command->bytes[12]|command->bytes[13]<<8)};
    int result=taxi_ai_controller_step(&ai,player->world.contact_378,counter,path);
    if(result<0)return result;
    v->world.height.position.x=ai.position[0];v->world.height.position.z=ai.position[2];
    v->world.heading_1c=ai.heading_1c;v->world.heading_20=ai.heading_20;
    command->bytes[4]=ai.command_4;command->bytes[12]=(uint8_t)ai.command_c;command->bytes[13]=(uint8_t)(ai.command_c>>8);
    return result;
}
static int32_t target_angle(const int32_t from[3],const int32_t target[3]){
    return bits((uint32_t)taxi_ai_vector_angle(bits((uint32_t)target[0]-(uint32_t)from[0]),bits((uint32_t)target[2]-(uint32_t)from[2]))-1024u);
}
int taxi_ai_controller_step(TaxiAiVehicle *v,uint32_t player,uint32_t *counter,const TaxiAiPathAsset *path){
    if(!v||!counter||!path||!path->points)return -1;
    TaxiAiVehicle next=*v;uint32_t sequence=*counter;int32_t target[3]={0};
    uint32_t query=(v->phase_378>>8)+300;
    if(taxi_ai_path_target(path->points,path->count,query,target))return -1;
    int32_t error=taxi_ai_angle_difference((uint32_t)asr12(v->heading_1c),(uint32_t)target_angle(v->position,target));
    next.command_c=0;next.command_4=taxi_ai_command_prefix(v->command_4,v->speed_544,error);
    uint16_t relocation;
    int moved=taxi_ai_reposition_query(player,v->phase_378,&sequence,&relocation);
    if(moved<0)return -1;
    if(moved){
        if(taxi_ai_path_target(path->points,path->count,relocation,next.position)||taxi_ai_path_target(path->points,path->count,(uint32_t)relocation+300,target))return -1;
        next.heading_1c=next.heading_20=bits((uint32_t)target_angle(next.position,target)<<12);
    }
    *v=next;*counter=sequence;return moved;
}
