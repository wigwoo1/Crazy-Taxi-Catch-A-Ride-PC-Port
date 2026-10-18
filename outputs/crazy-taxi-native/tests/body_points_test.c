#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "body_points.h"
int main(void){
 static TaxiTrig t;t.cosine[0]=256;t.sine[1024]=256;
 TaxiBodyPoints b;TaxiPoint actor={100,200,300};
 assert(taxi_body_points(&b,actor,0,0,0,0,&t)==1);
 assert(b.positions[0].x==-79&&b.positions[0].y==456&&b.positions[0].z==-58);
 assert(b.positions[2].x==-79&&b.positions[2].y==264&&b.positions[2].z==-58);
 assert(b.positions[3].x==279&&b.positions[3].y==264&&b.positions[3].z==-58);
 assert(b.positions[5].x==100&&b.positions[5].y==264&&b.positions[5].z==658);
 assert(b.vectors[0].x==-768&&b.vectors[0].y==256&&b.vectors[0].z==-2048);
 assert(b.vectors[2].x==-256&&b.vectors[2].z==-3072);
 assert(b.vectors[3].x==256&&b.vectors[3].z==-3072);
 assert(taxi_body_points(&b,actor,0,0,1023*4096,4096,&t)==1);
 assert(b.positions[2].x==458&&b.positions[2].y==264&&b.positions[2].z==121);
 assert(b.vectors[2].x==3072&&b.vectors[2].z==-256);
 assert(taxi_body_points(&b,(TaxiPoint){INT32_MAX,0,0},0,0,0,0,&t)==1);
 assert(b.positions[1].x==INT32_MIN+178);
 assert(taxi_body_points(NULL,actor,0,0,0,0,&t)==-1);
 puts("Body points: original attachment geometry, untranslated vectors, drift and wrap PASS");
 return 0;
}
