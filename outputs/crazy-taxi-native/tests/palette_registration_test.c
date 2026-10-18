#ifdef NDEBUG
#undef NDEBUG
#endif
#include "palette_registration.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint32_t read32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
        ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int main(void) {
    uint16_t palette[256] = {0};
    palette[1] = 0;
    palette[2] = 0x7fff;
    for (unsigned i=3; i<256; ++i) palette[i]=(uint16_t)i;
    for (unsigned flags=0; flags<8; ++flags) {
        TaxiPaletteRegistration state = {0};
        state.flags=(uint8_t)flags;
        TaxiPaletteTransfer transfer={0x11111111,0x22222222,0x33333333};
        assert(taxi_palette_register(&state,palette,0x084a5a34,0x12f,1,&transfer)==0);
        uint32_t source=(flags&1)?0x084a50b0u:(flags&2)?0x084a52b0u:
            (flags&4)?0x084a5a34u:0x084a50b0u;
        assert(transfer.source_identity==source);
        assert(transfer.destination_identity==0x05000000u&&transfer.control==0x84000080u);
        assert(state.count==1&&state.bg_bytes==512&&state.obj_bytes==0);
        const uint8_t *record=(const uint8_t *)state.records;
        assert(record[0]==0&&record[1]==1&&record[2]==0&&record[3]==0);
        assert(record[4]==32&&record[5]==0&&record[6]==1&&record[7]==1);
        assert(record[14]==1&&record[15]==2&&read32(record+16)==0x084a5a34u);
        TaxiPaletteRegistration before=state;
        TaxiPaletteTransfer previous=transfer;
        assert(taxi_palette_register(&state,palette,0x084a5a34,32,1,&transfer)==-1);
        assert(!memcmp(&state,&before,sizeof state)&&!memcmp(&transfer,&previous,sizeof transfer));
        assert(taxi_palette_registration_reset(&state)==0);
        assert(state.count==0&&state.bg_bytes==0&&state.obj_bytes==0&&state.flags==flags);
        assert(taxi_palette_register(&state,palette,0x084a5a34,31,0,&transfer)==0);
        assert(transfer.destination_identity==0x05000200u&&state.obj_bytes==512&&state.bg_bytes==0);
        assert(((uint8_t *)state.records)[4]==31);
    }
    TaxiPaletteRegistration full={0};full.count=128;
    TaxiPaletteRegistration before=full;
    TaxiPaletteTransfer transfer={1,2,3},prior=transfer;
    assert(taxi_palette_register(&full,palette,0x084a5a34,32,0,&transfer)==-2);
    assert(!memcmp(&full,&before,sizeof full)&&!memcmp(&transfer,&prior,sizeof transfer));
    assert(taxi_palette_register(&full,NULL,0,0,0,&transfer)==-2);
    puts("PASS current palette registration: source precedence, capped records, both destinations, atomic duplicate/capacity rejection and reset; ROM equivalence is tracked separately.");
    return 0;
}
