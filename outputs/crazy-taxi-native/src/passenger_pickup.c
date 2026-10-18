#include "passenger_pickup.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p) {return bits((uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static int valid(int32_t index,size_t count) {return index>=0&&(size_t)index<count;}
static int32_t approach(int32_t taxi,int32_t customer) {
    return bits(((uint32_t)taxi-(uint32_t)customer)<<8)/76;
}
int taxi_passenger_pickup(TaxiPickupState *out,TaxiPassengerState *customers,
    const uint8_t *destinations,size_t count,const TaxiPickupInput *in,TaxiPickupEffects *effects) {
    if(!out||!in||!effects||count>128||(count&&(!customers||!destinations))) return -1;
    TaxiPickupState s=*out;TaxiPickupEffects e={.customer_index=-1,.destination_index=-1};
    if(s.pending_1704) {
        if(!valid(s.selected_index,count)) return -1;
        uint8_t *flags=&customers[s.selected_index].bytes[0];
        if(*flags&32) {
            *flags=(uint8_t)((*flags&254)|2);
            s.destination_index=destinations[s.selected_index];
            e.completed=1;e.customer_index=s.selected_index;e.destination_index=s.destination_index;
            s.pending_1704=0;
        }
    } else if(s.destination_index==-1&&in->candidate_index!=-1) {
        if(!valid(in->candidate_index,count)) return -1;
        const uint8_t *p=customers[in->candidate_index].bytes;
        if((p[0]&4)&&!in->gate_2b78&&!in->gate_1700) {
            s.selected_index=s.dialogue_index=in->candidate_index;
            s.approach_x_7534=approach(in->taxi_position.x,word(p+16));
            s.approach_z_7538=approach(in->taxi_position.z,word(p+24));
            s.pending_1704=1;e.began=1;e.random_calls=1;e.customer_index=in->candidate_index;
            s.animation_25d7=(uint8_t)(in->random_first%2);
            if(p[0]&8) {
                s.animation_25d7=(uint8_t)(s.animation_25d7+2);
                s.voice_25d6=(uint8_t)(((uint32_t)in->random_second&1)+20);e.random_calls=2;
            }
        }
    }
    *out=s;*effects=e;return 0;
}
