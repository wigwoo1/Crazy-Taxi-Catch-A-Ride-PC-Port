#include "world_asset.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
_Static_assert(sizeof(TaxiWorldContact)==28,"original contact size");
_Static_assert(offsetof(TaxiWorldContact,contact_value)==24,"original contact word");
int main(int argc,char **argv){
    if(argc!=2)return 2;
    TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,argv[1]))return 3;
    TaxiWorldContact out,old;
    memset(&out,0xa5,sizeof out);old=out;
    if(taxi_world_asset_contact(NULL,(TaxiPoint){0},&out)!=-1||memcmp(&out,&old,sizeof out))return 4;
    if(taxi_world_asset_contact(&a,(TaxiPoint){0},NULL)!=-1)return 5;
    int x,y,z;unsigned seed;
    while(scanf("%d %d %d %u",&x,&y,&z,&seed)==4){
        unsigned char *bytes=(unsigned char *)&out;
        for(unsigned j=0;j<sizeof out;j++)bytes[j]=(unsigned char)(seed+j*37);
        TaxiPoint p={x,y,z};int result=taxi_world_asset_contact(&a,p,&out);
        printf("%d ",result);
        for(unsigned j=0;j<sizeof out;j++)printf("%02x",bytes[j]);
        printf(" %d %d %d\n",p.x,p.y,p.z);
    }
    taxi_world_asset_free(&a);return ferror(stdin)?6:0;
}
