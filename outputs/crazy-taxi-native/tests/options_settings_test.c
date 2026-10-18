#ifdef NDEBUG
#undef NDEBUG
#endif
#include "options_settings.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    for(int row=0;row<3;row++)for(unsigned v=0;v<256;v++)
    for(unsigned input=0;input<4;input++)for(unsigned enabled=0;enabled<2;enabled++){
        TaxiGameSettings settings={{0x25,32,64,0xa5,7,50,8,9}},before;
        unsigned offset=row?3:5;settings.bytes[offset]=(uint8_t)v;before=settings;
        TaxiOptionsSettingsEffect e;
        assert(!taxi_options_settings_step(&settings,(int8_t)row,input&1,input&2,(uint8_t)enabled,&e));
        unsigned expected=v;int changed=0;
        if(!row){
            if((input&2)&&v!=70){expected=(v+5)&255;changed=1;}
            else if((input&1)&&v!=35){expected=(v+251)&255;changed=1;}
        }else{
            unsigned n=row==1?v%16:v/16,other=row==1?v/16:v%16;
            if((input&1)&&n!=1){n=(n+15)%16;changed=1;}
            else if((input&2)&&n!=8){n=(n+1)%16;changed=1;}
            expected=row==1?other*16+n:n*16+other;
        }
        assert(e.previous==v&&e.current==expected&&e.changed==changed);
        assert(e.sound_index==(enabled&&changed?7:-1));
        before.bytes[offset]=(uint8_t)expected;
        assert(!memcmp(&settings,&before,sizeof settings));
    }
    for(int row=-128;row<128;row++)if(row<0||row>2){
        TaxiGameSettings settings={{1,2,3,4,5,6,7,8}},before=settings;TaxiOptionsSettingsEffect e;
        assert(!taxi_options_settings_step(&settings,(int8_t)row,1,1,1,&e));
        assert(!e.changed&&e.sound_index==-1&&!memcmp(&settings,&before,sizeof settings));
    }
    puts("PASS original Options settings: all byte values/input pairs, exact bounds, direction priority, nibble isolation and sound gating.");
    return 0;
}
