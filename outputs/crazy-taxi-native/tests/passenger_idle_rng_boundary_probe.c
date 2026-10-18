/* Verify the recovered passenger-idle RNG boundary against original-ROM RAM. */
#include "../src/passenger_idle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { EWRAM_BYTES=0x40000, EWRAM_BASE=0x02000000, PICKUP_PENDING=0x1704,
       PLAYER=0x2634, RNG=0x3b04, FREEZE=0x5f20, TRIP_ACTIVE=0x1694 };

static uint32_t word(const uint8_t *p){
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static int32_t signed_word(const uint8_t *p){
    uint32_t u=word(p);int32_t s;memcpy(&s,&u,sizeof s);return s;
}
static int read_file(const char *path,uint8_t *out){
    FILE *f=fopen(path,"rb");if(!f)return -1;
    int ok=fread(out,1,EWRAM_BYTES,f)==EWRAM_BYTES&&fgetc(f)==EOF;
    if(fclose(f))return -1;return ok?0:-1;
}

int main(int argc,char **argv){
    if(argc!=6){fprintf(stderr,"usage: %s <before.ewram> <after.ewram> <delta> <passenger-address> <expected-rng-after-idle>\n",argv[0]);return 64;}
    uint8_t *before=malloc(EWRAM_BYTES),*after=malloc(EWRAM_BYTES);
    if(!before||!after||read_file(argv[1],before)||read_file(argv[2],after)){
        fprintf(stderr,"cannot read source EWRAM checkpoints\n");free(before);free(after);return 2;
    }
    char *end=NULL;long delta=strtol(argv[3],&end,10);
    if(!end||*end||delta<0||delta>32767){free(before);free(after);return 64;}
    unsigned long address=strtoul(argv[4],&end,0);
    if(!end||*end||address<EWRAM_BASE||address>EWRAM_BASE+EWRAM_BYTES-44u){free(before);free(after);return 64;}
    unsigned long expected_rng=strtoul(argv[5],&end,0);
    if(!end||*end||expected_rng>UINT32_MAX){free(before);free(after);return 64;}
    size_t passenger=(size_t)(address-EWRAM_BASE);
    TaxiPassengerState state;memcpy(state.bytes,before+passenger,sizeof state.bytes);
    TaxiPickupState pickup={0};pickup.pending_1704=signed_word(before+PICKUP_PENDING);
    /* Original passenger logic runs after the driver update in each source row. */
    TaxiRoutePoint taxi={signed_word(after+PLAYER),signed_word(after+PLAYER+8)};
    uint32_t rng=word(before+RNG),source_rng_after=word(after+RNG);
    unsigned mode_before=state.bytes[10],timer_before=(unsigned)state.bytes[12]|(unsigned)state.bytes[13]<<8;
    int selected=before[TRIP_ACTIVE]!=0;
    if(taxi_passenger_idle_rng(&state,selected,&pickup,taxi,(int32_t)delta,before[FREEZE],&rng)){
        fprintf(stderr,"native passenger-idle routine rejected the source checkpoint\n");free(before);free(after);return 3;
    }
    if(taxi_passenger_frame_select(&state)){
        fprintf(stderr,"native passenger frame selector rejected its post-idle state\n");free(before);free(after);return 4;
    }
    if(memcmp(state.bytes,after+passenger,sizeof state.bytes)||rng!=expected_rng){
        fprintf(stderr,"source/native passenger boundary mismatch: passenger_equal=%d rng=%u/expected-%lu frame-end-%u\n",
            memcmp(state.bytes,after+passenger,sizeof state.bytes)==0,rng,expected_rng,source_rng_after);
        free(before);free(after);return 10;
    }
    printf("PASS passenger-idle source boundary: owner=0x%08lx delta=%ld selected=%d freeze=%u pending=%ld mode=%u->%u timer=%u->%u RNG %u->%u; source frame-end RNG=%u\n",
        address,delta,selected,(unsigned)before[FREEZE],(long)pickup.pending_1704,mode_before,(unsigned)state.bytes[10],timer_before,
        (unsigned)state.bytes[12]|(unsigned)state.bytes[13]<<8,word(before+RNG),rng,source_rng_after);
    free(before);free(after);return 0;
}
