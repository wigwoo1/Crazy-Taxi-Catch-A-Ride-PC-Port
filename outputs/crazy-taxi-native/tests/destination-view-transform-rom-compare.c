#include "../src/view_transform.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){if(argc!=2)return 2;FILE *f=fopen(argv[1],"rb");unsigned char raw[16388];if(!f||fread(raw,1,sizeof raw,f)!=sizeof raw)return 3;fclose(f);TaxiTrig trig;taxi_trig_decode(&trig,raw+4,raw+8196);int a,b,c,d,e,g;while(scanf("%d %d %d %d %d %d",&a,&b,&c,&d,&e,&g)==6){int fields[6]={a,b,c,d,e,g};unsigned char record[48]={0};memcpy(record,fields,24);TaxiViewTransform out;if(taxi_view_transform_destination(&out,record,sizeof record,&trig))return 4;for(unsigned i=0;i<18;i++)printf("%d ",out.words[i]);puts("");}return 0;}
