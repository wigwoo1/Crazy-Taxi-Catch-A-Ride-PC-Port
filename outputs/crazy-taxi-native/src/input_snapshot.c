#include "input_snapshot.h"
static uint16_t word(const uint8_t *p){return (uint16_t)((unsigned)p[0]|(unsigned)p[1]<<8);}
int taxi_input_snapshot_read(TaxiInput *s,const uint8_t *p,size_t size){
 if(!s||!p||size<66)return -1;
 TaxiInput v={.auxiliary_mask=word(p),.raw_keys=word(p+2),.pressed_mask=word(p+4)};
 for(unsigned i=0;i<10;i++){v.held[i]=p[6+i];v.previous[i]=p[16+i];v.repeat[i]=p[26+i];v.pressed[i]=p[36+i];v.countdown[i]=word(p+46+i*2);}
 *s=v;return 0;
}
