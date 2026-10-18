#include "title_publish.h"
int taxi_title_objects_publish(TaxiTitleLoad *s){
 if(!s)return -1;
 return taxi_startup_obj_publish(&s->queue,&s->transport,s->base_template,s->matrix_template);
}
