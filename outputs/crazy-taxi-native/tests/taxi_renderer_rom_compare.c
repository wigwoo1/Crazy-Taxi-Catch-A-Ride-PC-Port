#include "../src/camera_angle.h"
#include "../src/taxi_sprite.h"
#include "../src/obj_order.h"
#include <stdio.h>
int main(void){unsigned char b[16388];FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fread(b,1,sizeof b,f)!=sizeof b)return 2;fclose(f);TaxiTrig t;taxi_trig_decode(&t,b+4,b+8196);int cx,cy,cz,pitch,yaw,roll,heading,bodyroll,dist,base;while(scanf("%d%d%d%d%d%d%d%d%d%d",&cx,&cy,&cz,&pitch,&yaw,&roll,&heading,&bodyroll,&dist,&base)==10){TaxiPoint p;if(!taxi_camera_project(&p,(TaxiPoint){0,-192,0},(TaxiPoint){cx,cy,cz},(int16_t)pitch,(int16_t)yaw,(int16_t)roll,192,&t))return 3;int scale=192*2048/(dist-base);if(scale==256)scale=257;int centers[2][2];unsigned a=(unsigned)-bodyroll&4095u;if(taxi_obj_half_centers(p.x,p.y,scale,1,t.sine[a],t.cosine[a],centers))return 4;printf("%u %d %d %d %d %d %d\n",taxi_sprite_frame((int16_t)yaw,heading),centers[0][0],centers[0][1],centers[1][0],centers[1][1],p.z,scale);}return 0;}
