#include "../src/driving.h"
#include <stdio.h>
int main(void) {
    TaxiInput input = {0};
    TaxiCommand command;
    unsigned value;
    for (unsigned i=0; i<10; ++i) {
        if (scanf("%u", &value)!=1 || value>255) return 2;
        input.held[i]=(uint8_t)value;
    }
    for (unsigned i=0; i<16; ++i) {
        if (scanf("%u", &value)!=1 || value>255) return 2;
        command.bytes[i]=(uint8_t)value;
    }
    taxi_command_pack(&command,&input);
    for (unsigned i=0; i<16; ++i) printf("%u%c",command.bytes[i],i==15?'\n':' ');
    return 0;
}
