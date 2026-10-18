#include "profile_commit.h"
#include <stdio.h>
#include <stdint.h>
#include <io.h>
#include <fcntl.h>
int main(void){
 _setmode(_fileno(stdout),_O_BINARY);
 unsigned mode,city,choice,current,fresh,exiting,rank,seed;int whole,fraction;unsigned count=0;
 while(scanf("%u%u%u%u%u%u%u%u%d%d",&mode,&city,&choice,&current,&fresh,&exiting,&rank,&seed,&whole,&fraction)==10){
  uint8_t block[512],snapshot[512],runtime[800];
  for(unsigned i=0;i<512;i++){block[i]=(uint8_t)((i*17+seed*13)^90);snapshot[i]=(uint8_t)((i*31+seed*7)^165);}
  for(unsigned i=0;i<800;i++)runtime[i]=(uint8_t)((i*19+seed*23)^60);
  block[0]=(uint8_t)(mode|(seed&3));block[4]=(uint8_t)city;snapshot[0]=(uint8_t)(((mode+32)&96)|(seed&3));snapshot[4]=(uint8_t)(1-city);
  TaxiProfileChoice s={.choice=(uint8_t)choice,.current=(uint8_t)current,.new_profile=(uint8_t)fresh,.exiting=(uint8_t)exiting};
  const uint8_t initials[3]={'X','Y','Z'};int result=taxi_profile_commit_normal(block,snapshot,runtime,&s,(uint8_t)rank,(uint8_t)(seed%8),(uint8_t)(seed*5),0,initials,whole,fraction);
  if(result<0)return 2;uint32_t status=(uint32_t)result;
  if(fwrite(&status,4,1,stdout)!=1||fwrite(block,1,512,stdout)!=512||fwrite(runtime,1,800,stdout)!=800)return 3;count++;
 }
 return count?0:4;
}
