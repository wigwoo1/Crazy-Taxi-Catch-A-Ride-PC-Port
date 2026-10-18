#include "../src/body_pose.h"
#include <stdio.h>
static TaxiNormalAngles table;
int main(void){unsigned char b[16386];FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fseek(f,16388,SEEK_SET)||fread(b,1,sizeof b,f)!=sizeof b)return 1;fclose(f);if(taxi_normal_angles_decode(&table,b,sizeof b))return 2;int x,z,v,w,nx,nz,d;while(scanf("%d%d%d%d%d%d%d",&x,&z,&v,&w,&nx,&nz,&d)==7){TaxiBodyPose s={.orientation_6f4={x,0,z},.velocity_730=(short)v,.velocity_732=(short)w};int rc=taxi_body_align(&s,(TaxiPoint){nx,4000,nz},d,&table);printf("%d %d %d %d %d\n",rc,s.orientation_6f4.x,s.orientation_6f4.z,s.velocity_730,s.velocity_732);}return 0;}
