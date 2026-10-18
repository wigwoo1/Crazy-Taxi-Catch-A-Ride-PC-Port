#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/destination.h"
int main(void) {
    uint8_t record[48]; memset(record,255,sizeof record);
    assert(!taxi_destination_accepts(0,record,65535));
    for(unsigned slot=0;slot<4;slot++) {
        memset(record+40,255,8);
        record[40+slot*2]=0x34;record[41+slot*2]=0x12;
        assert(taxi_destination_accepts(0,record,0x1234));
        assert(!taxi_destination_accepts(-1,record,0x1234));
        assert(!taxi_destination_accepts(0,NULL,0x1234));
        assert(!taxi_destination_accepts(0,record,0x11234));
        assert(!taxi_destination_accepts(0,record,0x1235));
    }
    memset(record+40,0,8);
    assert(taxi_destination_accepts(0,record,0));
    puts("PASS: original destination material acceptance, four slots and sentinels.");
    return 0;
}

