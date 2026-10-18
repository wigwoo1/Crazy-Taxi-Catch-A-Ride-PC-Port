#include "../src/score_insert.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned cases=0;for(unsigned bits=0;bits<65536;bits++){
 uint16_t u=(uint16_t)bits;int16_t rem;memcpy(&rem,&u,2);uint8_t rows[12]={0};unsigned rank=taxi_challenge_bound_insert(rows,1,3,rem);uint32_t word;memcpy(&word,rows,4);
 uint32_t want=1u|96u|128u|((uint32_t)(2400-(int32_t)rem)<<15);if(rank!=1||word!=want)return 1;cases++;
 }
 uint8_t rows[12]={0};taxi_challenge_bound_insert(rows,0,0,1400);taxi_challenge_bound_insert(rows,0,1,1500);taxi_challenge_bound_insert(rows,1,2,1450);
 uint32_t a,b,c;memcpy(&a,rows,4);memcpy(&b,rows+4,4);memcpy(&c,rows+8,4);if((a>>15)!=900||(b>>15)!=950||(c>>15)!=1000)return 2;
 uint8_t before[12];memcpy(before,rows,12);if(taxi_challenge_bound_insert(rows,0,0,1400)||memcmp(rows,before,12))return 3;
 printf("PASS Bound %u signedtimer packing cases, ordered faster records, full-table tie rejection. Native arithmetic only.\n",cases);return 0;}
