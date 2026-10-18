#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "audio_mix.h"
int main(void){
    const uint8_t samples[]={1,2,3,127,128,255};
    int16_t dst[8]={0};uint32_t next=999;
    /* Threshold4 consumes padding byte4=-128 before restarting at1+step. */
    assert(!taxi_audio_mix_loop(dst,8,samples,6,0,256,4,1,1,&next));
    const int16_t expected[]={1,2,3,127,-128,3,127,-128};
    assert(!memcmp(dst,expected,sizeof dst)&&next==512);
    memset(dst,0,sizeof dst);
    assert(!taxi_audio_mix_loop(dst,5,samples,6,3*256,128,4,0,1,&next));
    const int16_t half[]={127,127,-128,1,2};
    assert(!memcmp(dst,half,sizeof half)&&next==384);
    int16_t old[8];memcpy(old,dst,sizeof old);next=999;
    assert(taxi_audio_mix_loop(dst,8,samples,4,0,256,4,0,1,&next)==-1);
    assert(!memcmp(old,dst,sizeof old)&&next==999);
    dst[0]=32760;assert(!taxi_audio_mix_loop(dst,1,samples,6,768,256,4,0,64,&next));
    assert(dst[0]==-24648);
    assert(!taxi_audio_mix_loop(NULL,0,NULL,0,123,1,4,0,1,&next)&&next==123);
    puts("PASS source loop mixer: read-before-reset padding, fractional phase, wrap and atomic bounds");return 0;
}
