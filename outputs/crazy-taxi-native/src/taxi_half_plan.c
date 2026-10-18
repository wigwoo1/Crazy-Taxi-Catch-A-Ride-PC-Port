#include "taxi_half_plan.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
int taxi_half_plan(TaxiHalfPlan *out,int32_t x,int32_t y,int32_t focal,int32_t distance_fixed,int32_t base,int32_t sine,int32_t cosine){
 if(!out)return -1;
 uint32_t d=(uint32_t)distance_fixed;int32_t shifted=bits((d>>12)|(distance_fixed<0?0xfff00000u:0u));
 int32_t denominator=bits((uint32_t)shifted-(uint32_t)base);if(!denominator)return -1;
 int32_t numerator=bits((uint32_t)focal<<11);TaxiHalfPlan plan;
 plan.scale=bits((uint32_t)((int64_t)numerator/denominator));if(plan.scale==256)plan.scale=257;
 if(taxi_obj_half_centers(x,y,plan.scale,1,sine,cosine,plan.centers))return -1;
 *out=plan;return 0;
}
