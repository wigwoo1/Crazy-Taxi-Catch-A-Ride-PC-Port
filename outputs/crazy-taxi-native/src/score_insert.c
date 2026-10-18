#include "score_insert.h"
#include <string.h>
static uint32_t profile_word(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
unsigned taxi_profile_score_insert(uint8_t rows[12],uint8_t profile,
    uint8_t driver,uint8_t license,int32_t whole,int32_t fraction) {
    if(!rows||profile>1)return 0;
    for(unsigned i=0;i<3;i++) {
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&
            (whole<(int32_t)(prior>>15)||
             (whole==(int32_t)(prior>>15)&&fraction<=(int32_t)((prior>>8)&127))))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=((uint32_t)whole<<15)|(((uint32_t)fraction&127)<<8)|
            ((uint32_t)profile<<7)|((driver&3)<<5)|((license&15)<<1)|1;
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}
int taxi_score_insert(uint8_t saved[10][8],uint8_t runtime[100][8],
    uint8_t settings,uint8_t rank,uint32_t choice,uint8_t driver,uint8_t license,
    const uint8_t name[3],int32_t whole,int32_t fraction) {
    if(!choice||(settings&224)||(uint8_t)(rank-1)>9)return 0;
    if(!saved||!runtime||!name)return -1;
    /* Source e7f0 copies eight bytes from row i-1 to i, descending. */
    for(unsigned i=9;i>=rank;i--)memcpy(saved[i],saved[i-1],8);
    uint8_t *row=saved[rank-1];
    memcpy(row,name,3); /* Original byte3 remains in the destination row. */
    uint32_t word=((uint32_t)whole<<15)|(((uint32_t)fraction&127)<<8)|
        (((choice-1)&1)<<7)|((driver&3)<<5)|((license&15)<<1)|1;
    for(unsigned i=0;i<4;i++)row[4+i]=(uint8_t)(word>>(i*8));
    memcpy(runtime,saved,80);return 1;
}

unsigned taxi_challenge_distance_insert(uint8_t rows[12],uint8_t profile,
    uint8_t driver,int32_t measurement) {
    if(!rows||profile>1)return 0;
    uint32_t raw=(uint32_t)measurement;
    int32_t whole;uint32_t shifted=(raw>>8)|(measurement<0?0xff000000u:0);
    memcpy(&whole,&shifted,4);
    uint32_t fraction=((raw&255u)*100u)>>8;
    for(unsigned i=0;i<3;i++){
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&(whole<(int32_t)(prior>>15)||
            (whole==(int32_t)(prior>>15)&&fraction<=((prior>>8)&127u))))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|
            (fraction<<8)|((uint32_t)whole<<15);
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}

unsigned taxi_challenge_flag_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining){
    if(!rows||profile>1)return 0;
    int32_t elapsed=1200-(int32_t)remaining;
    for(unsigned i=0;i<3;i++){
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&elapsed>=(int32_t)(prior>>15))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|((uint32_t)elapsed<<15);
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}

unsigned taxi_challenge_ball_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining){
    if(!rows||profile>1)return 0;
    int32_t elapsed=2700-(int32_t)remaining;
    for(unsigned i=0;i<3;i++){
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&elapsed>=(int32_t)(prior>>15))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|((uint32_t)elapsed<<15);
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}

unsigned taxi_challenge_drift_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t count){
 if(!rows||profile>1)return 0;
 for(unsigned i=0;i<3;i++){
  uint32_t prior=profile_word(rows+i*4);
  if((prior&1)&&(int32_t)count<=(int32_t)(prior>>15))continue;
  for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
  uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|((uint32_t)(int32_t)count<<15);
  for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
  return i+1;
 }
 return 0;
}

unsigned taxi_challenge_turn_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining){
    if(!rows||profile>1)return 0;
    int32_t elapsed=2100-(int32_t)remaining;
    for(unsigned i=0;i<3;i++){
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&elapsed>=(int32_t)(prior>>15))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|((uint32_t)elapsed<<15);
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}


unsigned taxi_challenge_bound_insert(uint8_t rows[12],uint8_t profile,uint8_t driver,int16_t remaining){
    if(!rows||profile>1)return 0;
    int32_t elapsed=2400-(int32_t)remaining;
    for(unsigned i=0;i<3;i++){
        uint32_t prior=profile_word(rows+i*4);
        if((prior&1)&&elapsed>=(int32_t)(prior>>15))continue;
        for(unsigned j=2;j>i;j--)memcpy(rows+j*4,rows+(j-1)*4,4);
        uint32_t word=(prior&30u)|1u|((driver&3u)<<5)|((uint32_t)profile<<7)|((uint32_t)elapsed<<15);
        for(unsigned j=0;j<4;j++)rows[i*4+j]=(uint8_t)(word>>(j*8));
        return i+1;
    }
    return 0;
}

