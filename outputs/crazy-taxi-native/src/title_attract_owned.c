#include "title_attract_owned.h"
int taxi_title_attract_prepare_owned(TaxiTitleParentContinuation *c,TaxiAttractPrepare *s){
 if(!c||!c->state||c->pending!=TAXI_TITLE_PARENT_PREPARE_ATTRACT||!s)return -1;
 taxi_attract_prepare(s);return taxi_title_parent_resume(c);
}
