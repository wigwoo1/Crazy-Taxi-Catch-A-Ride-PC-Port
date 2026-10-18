#include "../src/ground.c"
#include <stdio.h>
int main(void){int px,pz,ax,az,bx,bz,cx,cz;while(scanf("%d%d%d%d%d%d%d%d",&px,&pz,&ax,&az,&bx,&bz,&cx,&cz)==8)printf("%d\n",contains((TaxiPoint){px,0,pz},(TaxiPoint){ax,0,az},(TaxiPoint){bx,0,bz},(TaxiPoint){cx,0,cz}));return 0;}
