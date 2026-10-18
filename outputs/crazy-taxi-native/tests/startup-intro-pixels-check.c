#include "../src/startup_intro_pixels.h"
#include <stdio.h>
static TaxiStartupIntro s;static uint8_t bg0[512*256],bg1[256*256],tiles[64];static uint16_t palette[256],out[240*160];
static int expect(uint16_t value){int result=taxi_startup_intro_pixels(&s,bg0,bg1,palette,tiles,sizeof tiles,out);if(result||out[0]!=value){fprintf(stderr,"expected %04x got %04x result%d\n",value,out[0],result);return 1;}return 0;}
int main(void){
 for(unsigned i=0;i<128;i++)s.transport.output[i][0]=0x200;
 palette[1]=31;palette[2]=31<<10;s.palette[1]=31<<5;bg0[0]=1;bg1[0]=2;s.alpha=0x0808;
 s.blend=0x1f40;if(expect(31))return 1;
 s.blend=0x241;if(expect(15|(15<<10)))return 2;
 s.transport.output[0][0]=0x2400;s.transport.output[0][1]=0;s.transport.output[0][2]=0;tiles[0]=1;
 s.blend=0x100;if(expect(15|(15<<5)))return 3;
 s.blend=0;if(expect(31<<5))return 4;
 s.transport.output[0][0]=0x2000;s.blend=0x100;if(expect(31<<5))return 5;
 s.transport.output[0][2]=1<<10;if(expect(31))return 6;
 s.transport.output[0][0]=0x200;s.scroll=1;bg0[1]=2;if(expect(31<<10))return 7;
 s.scroll=0;s.blend=0xff;s.brightness=16;if(expect(0))return 8;
 s.brightness=8;if(expect(16))return 9;
 s.transport.output[0][0]=0x2400;s.transport.output[0][2]=0;if(expect(31<<5))return 10;
 puts("PASS native pixel checks: BG priority, normalalpha, forced semiOBJ, target exclusion, OBJ priority, scroll, brightness, semiOBJ fade exclusion");return 0;}
