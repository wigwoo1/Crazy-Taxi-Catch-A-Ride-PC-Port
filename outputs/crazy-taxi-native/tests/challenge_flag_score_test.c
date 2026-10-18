#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "score_insert.h"
static uint32_t word(const uint8_t *r){return r[0]|((uint32_t)r[1]<<8)|((uint32_t)r[2]<<16)|((uint32_t)r[3]<<24);}
static void put(uint8_t *r,uint32_t u){for(unsigned i=0;i<4;i++)r[i]=(uint8_t)(u>>(8*i));}
int main(void){
 uint8_t rows[12],before[12];
 for(int remaining=-32768;remaining<=32767;remaining++){
  memset(rows,0,sizeof rows);rows[0]=30;
  assert(taxi_challenge_flag_insert(rows,1,3,(int16_t)remaining)==1);
  assert(word(rows)==(((uint32_t)(1200-remaining)<<15)|255u));
 }
 put(rows,(100u<<15)|1);put(rows+4,(200u<<15)|31);put(rows+8,(300u<<15)|1);
 memcpy(before,rows,12);assert(taxi_challenge_flag_insert(rows,0,2,1000)==3);assert(word(rows+8)==((200u<<15)|65));assert(!memcmp(rows,before,8));
 memcpy(before,rows,12);assert(!taxi_challenge_flag_insert(rows,0,0,900));assert(!memcmp(rows,before,12));
 assert(taxi_challenge_flag_insert(rows,1,1,1050)==2);assert(word(rows+4)==((150u<<15)|191));assert(word(rows+8)==word(before+4));
 memcpy(before,rows,12);assert(!taxi_challenge_flag_insert(rows,2,0,1200));assert(!memcmp(rows,before,12));assert(!taxi_challenge_flag_insert(0,0,0,1200));
 puts("PASS Crazy Flag time records: all65536 signed remaining values, lower-is-better strict ties, shifts, metadata, atomic invalid profile.");return 0;
}
