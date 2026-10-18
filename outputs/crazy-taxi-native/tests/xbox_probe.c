#include "xbox.h"
#include <stdio.h>
int main(void){
    unsigned connected=0;
    for(unsigned i=0;i<4;i++){
        TaxiXboxSample s=taxi_xbox_poll(i);connected+=(unsigned)s.connected;
        printf("Xbox slot%u connected%d steering%d accelerator%u brake%u keys%03x\n",
            i,s.connected,s.steering,s.accelerator,s.brake,s.active_low_keys);
    }
    printf("Native XInput enumeration: %u connected controllers; instantaneous read only, not physical control validation.\n",connected);
    return 0;
}
