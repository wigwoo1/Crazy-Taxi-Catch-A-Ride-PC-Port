#include "../src/camera_angle.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");if(!f)return 3;
 unsigned char s[8192],c[8192];if(fseek(f,0x4a0d08,SEEK_SET)||fread(s,1,8192,f)!=8192||fread(c,1,8192,f)!=8192)return 4;fclose(f);
 TaxiTrig t;taxi_trig_decode(&t,s,c);
 for(int yaw=0;yaw<4096;yaw+=64)for(int pitch=-512;pitch<=512;pitch+=128)for(int roll=-256;roll<=256;roll+=128){
 TaxiCameraView v=taxi_camera_mode3_view((TaxiPoint){100,200,300},pitch,yaw*4096,roll,(TaxiPoint){17,-1,-19},-101,&t);
 printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",yaw,pitch,roll,v.pitch,v.yaw,v.roll,v.position.x,v.position.y,v.position.z);
 }
 return 0;
}
