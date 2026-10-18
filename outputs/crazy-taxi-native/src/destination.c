#include "destination.h"
#include <stddef.h>
int taxi_destination_accepts(int32_t selected_index,const uint8_t *record,
                             uint32_t material) {
    if(selected_index == -1 || record == NULL) return 0;
    for(unsigned offset=40;offset<48;offset+=2) {
        uint32_t slot=(uint32_t)record[offset] | ((uint32_t)record[offset+1]<<8);
        if(slot != 0xffff && slot == material) return 1;
    }
    return 0;
}

