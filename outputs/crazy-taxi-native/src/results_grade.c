#include "results_grade.h"
#include <string.h>
#include <stdio.h>
int taxi_results_scores_load(uint32_t scores[100],const char *path) {
    if(!scores||!path)return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    uint8_t header[8],row[8];uint32_t loaded[100];
    if(fread(header,1,8,f)!=8||memcmp(header,"CTS1\x64\0\0\0",8))goto bad;
    for(unsigned i=0;i<100;i++) {
        if(fread(row,1,8,f)!=8)goto bad;
        loaded[i]=row[4]|((uint32_t)row[5]<<8)|((uint32_t)row[6]<<16)|((uint32_t)row[7]<<24);
    }
    if(fgetc(f)!=EOF||ferror(f))goto bad;
    fclose(f);memcpy(scores,loaded,sizeof loaded);return 0;
bad:fclose(f);return -1;
}
uint8_t taxi_results_license(int32_t whole,int32_t fraction) {
    if(!whole&&!fraction)return 8;
    if(whole>20000)return 0;
    if(whole>10000)return 1;
    if(whole>5000)return 2;
    if(whole>4000)return 3;
    if(whole>3000)return 4;
    if(whole>2000)return 5;
    if(whole>1000)return 6;
    return 7;
}
uint8_t taxi_results_rank(int32_t whole,int32_t fraction,const uint32_t scores[100],uint8_t prior) {
    uint8_t rank=prior;
    for(unsigned i=0;i<100;i++) {
        uint32_t saved=scores[i]>>15;int32_t saved_fraction=(int32_t)((scores[i]>>8)&127);
        if(whole>(int32_t)saved||(whole==(int32_t)saved&&fraction>saved_fraction)) {rank=(uint8_t)(i+1);break;}
    }
    int8_t signed_rank;memcpy(&signed_rank,&rank,1);
    if(signed_rank>99)rank=99;else if(signed_rank<=0)rank=1;
    return rank;
}
