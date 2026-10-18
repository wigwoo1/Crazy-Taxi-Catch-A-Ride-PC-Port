#include "../src/traction.h"
#include <stdio.h>
int main(void){unsigned char b[514];TaxiAngleTable t;FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fseek(f,32774,SEEK_SET)||fread(b,1,514,f)!=514)return 1;fclose(f);taxi_angle_table_decode(&t,b);int x,z,a;while(scanf("%d%d",&x,&z)==2){int rc=taxi_vector_angle(x,z,&t,&a);printf("%d %d\n",rc,rc?0:a);}return 0;}
