#include "../src/indexed_preview_scale.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){unsigned sw,sh,dw,dh,seed;while(scanf("%u %u %u %u %u",&sw,&sh,&dw,&dh,&seed)==5){size_t sn=(size_t)sw*sh,dn=(size_t)dw*dh;unsigned char *s=malloc(sn),*d=malloc(dn+16);if(!s||!d)return 3;for(size_t j=0;j<sn;j++)s[j]=(unsigned char)(seed+j*37+(j/sw)*11);memset(d,165,dn+16);if(taxi_indexed_preview_scale(s,sn,sw,sh,d,dn,dw,dh))return 2;for(size_t j=0;j<dn+16;j++)printf("%u ",d[j]);puts("");free(s);free(d);}return 0;}
