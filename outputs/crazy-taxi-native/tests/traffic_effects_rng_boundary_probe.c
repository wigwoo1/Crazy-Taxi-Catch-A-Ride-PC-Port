/* Verify the VBlank 180 collision-effects RNG/global transition from source RAM. */
#include "../src/traffic_effects.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { EWRAM_BYTES=0x40000, EWRAM_BASE=0x02000000 };

static uint32_t u32(const uint8_t *p){
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static int32_t s32(const uint8_t *p){
    uint32_t u=u32(p);int32_t s;memcpy(&s,&u,sizeof s);return s;
}
static int16_t s16(const uint8_t *p){
    uint16_t u=(uint16_t)p[0]|(uint16_t)p[1]<<8;int16_t s;memcpy(&s,&u,sizeof s);return s;
}
static int read_ewram(const char *path,uint8_t *out){
    FILE *f=fopen(path,"rb");if(!f)return -1;
    int ok=fread(out,1,EWRAM_BYTES,f)==EWRAM_BYTES&&fgetc(f)==EOF;
    if(fclose(f))return -1;return ok?0:-1;
}
static int same_globals(const TaxiTrafficGlobals *g,const uint8_t *ram){
    return g->cooldown_e4==s16(ram+0xe4)&&g->near_miss_25d3==ram[0x25d3]
        &&g->voice_25d6==ram[0x25d6]&&g->combo_flag_9aa6==ram[0x9aa6]
        &&g->combo_9aa8==s32(ram+0x9aa8)&&g->combo_9aac==s32(ram+0x9aac);
}

int main(int argc,char **argv){
    if(argc!=4){fprintf(stderr,"usage: %s <before.ewram> <after.ewram> <rng-at-first-effects-call>\n",argv[0]);return 64;}
    uint8_t *before=malloc(EWRAM_BYTES),*after=malloc(EWRAM_BYTES);
    if(!before||!after||read_ewram(argv[1],before)||read_ewram(argv[2],after)){
        fprintf(stderr,"cannot read source EWRAM checkpoints\n");free(before);free(after);return 2;
    }
    char *end=NULL;unsigned long call_rng=strtoul(argv[3],&end,0);
    if(!end||*end||call_rng>UINT32_MAX){free(before);free(after);return 64;}
    TaxiTrafficGlobals globals={.cooldown_e4=s16(before+0xe4),.near_miss_25d3=before[0x25d3],
        .voice_25d6=before[0x25d6],.combo_flag_9aa6=before[0x9aa6],
        .combo_9aa8=s32(before+0x9aa8),.combo_9aac=s32(before+0x9aac)};
    uint32_t rng=(uint32_t)call_rng;TaxiTrafficEffects effects;
    const uint32_t rng_before=rng;const uint8_t timer_before=before[0x25d5];
    if(taxi_traffic_collision_effects(&globals,1,0,&rng,&effects)){
        fprintf(stderr,"native collision-effects routine rejected source checkpoint\n");free(before);free(after);return 3;
    }
    int globals_match=same_globals(&globals,after);
    int timer_match=timer_before==after[0x25d5];
    int rng_match=rng==u32(after+0x3b04);
    if(!globals_match||!timer_match||!rng_match||effects.random_calls!=2){
        fprintf(stderr,"source/native effects boundary mismatch: globals=%d timer=%d rng=%u/%u draws=%u/2\n",
            globals_match,timer_match,rng,u32(after+0x3b04),effects.random_calls);
        free(before);free(after);return 10;
    }
    printf("PASS traffic-effects source boundary: sound=1 trip_active=0 cooldown=%d->%d voice=%u RNG %u->%u draws=%u sound_table=%d; compared source globals and timer\n",
        s16(before+0xe4),globals.cooldown_e4,before[0x25d6],rng_before,rng,effects.random_calls,effects.sound_table_index);
    free(before);free(after);return 0;
}
