#include "music_note.h"
int taxi_music_note(TaxiMusicNote *out,uint8_t event_note,uint8_t transpose,
                    uint8_t tuning,const uint8_t *pitch,uint32_t output_rate){
    if(!out||!pitch||!output_rate||event_note==0||event_note==128)return 0;
    int t=transpose<128?transpose:(int)transpose-256;
    int fine=tuning<128?tuning:(int)tuning-256;
    /* Arithmetic shift one rounds negative odd values downward. */
    int half=fine>=0?fine/2:-((-fine+1)/2);
    TaxiMusicNote n;
    n.note=(uint8_t)(t+event_note-1);
    n.period=(uint16_t)(7616-(int)n.note*64-half);
    unsigned index=(uint16_t)(7680-n.period),octave=index/768;
    unsigned offset=(index%768)*2;
    uint32_t value=(uint32_t)(pitch[offset]|(unsigned)pitch[offset+1]<<8)<<2;
    unsigned shift=(7-octave)&255;
    n.frequency=shift>=32?0:value>>shift;
    n.step=(n.frequency<<8)/output_rate;
    *out=n;return 1;
}
