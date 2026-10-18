#include "hud_start.h"
static void put(const TaxiHudStartBackend *b,uint32_t a,uint32_t v,unsigned n){b->write(b->context,a,v,n);}
static uint32_t word(const TaxiHudStartBackend *b,uint32_t a){return b->resource_word(b->context,a);}
int taxi_hud_start(const TaxiHudStartInput *i,const TaxiHudStartBackend *b){
 if(!i||!b||!b->resource_word||!b->write||!b->replace)return -1;
 if(i->demo)return 0;
 if(i->driver>4||i->challenge_driver>3||i->session_driver>3)return -1;
 put(b,0x0200532c,0,2);put(b,0x0200532e,0,2);put(b,0x02005330,0,2);put(b,0x02005318,0,4);
 put(b,0x020016c4,(uint32_t)-68,4);put(b,0x020016d4,99,1);put(b,0x020016d5,0,1);put(b,0x020016d6,0,1);put(b,0x020016d8,0xf200,4);put(b,0x02005332,0,2);
 put(b,0x0200538a,72,2);put(b,0x02005388,72,2);put(b,0x02005390,72,2);put(b,0x0200538e,72,2);
 const uint32_t starts[]={0x02005334,0x0200534a,0x02005360,0x02005374};const unsigned counts[]={20,20,18,18};
 for(unsigned k=0;k<4;k++)for(unsigned j=0;j<counts[k];j++)put(b,starts[k]+j,255,1);
 if(b->replace(b->context,0x080bee8c,0,0,2,1))return -1;
 put(b,0x03005494,0x08105424,4);put(b,0x03005490,32,1);put(b,0x03005491,64,1);put(b,0x03005492,5,1);put(b,0x03005493,6,1);
 if(b->replace(b->context,word(b,0x087ec198+20*i->driver+4),0,0,4,1))return -1;
 for(unsigned j=0;j<3;j++)if(b->replace(b->context,word(b,0x087ec210+4*i->driver),j,j,7,1))return -1;
 unsigned d=(i->settings&224)==128?i->challenge_driver:i->session_driver;uint32_t a=0x02001790;
 put(b,a+0x60,word(b,0x084a551c+12*d),4);put(b,a+0x74,0x08290520,4);put(b,a+0x88,0x08290720,4);put(b,a+0xb0,word(b,0x087ec200),4);put(b,a+0xec,word(b,0x087ec24c+4*i->driver),4);put(b,a+0xc4,0x08290af8,4);put(b,a+0xd8,0x08290b40,4);
 return 0;
}
