#include "../src/destination.h"
#include <stdio.h>
int main(void){int selected;unsigned present,material,a,b,c,d;while(scanf("%d %u %u %u %u %u %u",&selected,&present,&material,&a,&b,&c,&d)==7){unsigned slots[4]={a,b,c,d};unsigned char record[48]={0};for(unsigned i=0;i<4;i++){record[40+i*2]=(unsigned char)slots[i];record[41+i*2]=(unsigned char)(slots[i]>>8);}printf("%d\n",taxi_destination_accepts(selected,present?record:0,material));}return 0;}
