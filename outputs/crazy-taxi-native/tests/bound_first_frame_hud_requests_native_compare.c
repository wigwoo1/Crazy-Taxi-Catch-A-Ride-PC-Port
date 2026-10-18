#include "challenge_flag_hud.h"
#include <stdio.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;uint8_t positions[60];if(fread(positions,1,60,f)!=60)return 3;fclose(f);TaxiFlagHudPlan p;if(taxi_challenge_flag_hud_plan(&p,2400,160,0)||taxi_challenge_flag_hud_positions(&p,positions,60))return 4;for(unsigned i=0;i<5;i++){TaxiFlagHudGlyph *g=p.glyphs+i;printf("%u %u %u %d %d %d\n",g->glyph,g->position_identity,g->scaled,g->x,g->y,g->z);}return 0;}
