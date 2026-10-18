#include "../src/runtime_asset.h"
#include "../src/taxi_shadow_render.h"
#include <stdio.h>
#include <limits.h>
#include "../src/world_asset.h"
int main(int argc,char **argv){static TaxiRuntimeAsset runtime;static TaxiWorldAsset world;if(argc!=3||taxi_runtime_asset_load(&runtime,argv[1])||taxi_world_asset_load(&world,argv[2]))return 1;
 int x,y,z,g,cx,cy,cz,yaw,pitch,h,v;
 while(scanf("%d %d %d %d %d %d %d %d %d %d %d",&x,&y,&z,&g,&cx,&cy,&cz,&yaw,&pitch,&h,&v)==11){if(g==INT_MIN&&taxi_world_asset_height(&world,(TaxiPoint){x,y,z},&g)<0)return 3;TaxiShadowRender out={0};int rc=taxi_shadow_render((TaxiPoint){x,y,z},g,(TaxiPoint){cx,cy,cz},yaw,pitch,&runtime.trig,h,v,&out);printf("%d %d",g,rc);if(rc==1)printf(" %d %d %d %d %d %d %d %d %d %d %u",out.billboard.top_left.x,out.billboard.top_left.y,out.billboard.top_left.z,out.billboard.bottom_right.x,out.billboard.bottom_right.y,out.billboard.bottom_right.z,out.screen.left,out.screen.top,out.screen.right,out.screen.bottom,out.bucket);putchar('\n');}return ferror(stdin)?2:0;}
