#include "../src/body_pose.h"
#include <stdio.h>
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");uint8_t b[16386];TaxiNormalAngles t;if(!f||fseek(f,0x7ec9ec,SEEK_SET)||fread(b,1,sizeof b,f)!=sizeof b)return 2;fclose(f);if(taxi_normal_angles_decode(&t,b,sizeof b))return 3;int n;while(scanf("%d",&n)==1){if(n< -4096||n>4096)return 4;printf("%d\n",t.angle[n+4096]);}return 0;}
