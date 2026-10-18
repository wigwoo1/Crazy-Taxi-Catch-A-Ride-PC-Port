#include <stdio.h>
#include <stdlib.h>
#define taxi_vehicle_start_scene native_vehicle_start_scene
#include "../src/vehicle_start_scene.c"
#undef taxi_vehicle_start_scene
static void dump_actor(FILE *f,const uint8_t *actor){for(unsigned i=0;i<0xea0;i++)fprintf(f,"%02x",actor[i]);}
int taxi_vehicle_start_scene(TaxiVehicleStartPacked *out,const TaxiSpawnMarker *marker,const uint8_t *data,size_t bytes,uint8_t settings,uint8_t demo,TaxiGroundQuery query,void *context,TaxiVehicleStartPoint *resolved){
 const char *path=getenv("TAXI_START_TRACE");TaxiVehicleStartPacked before=*out;
 int rc=native_vehicle_start_scene(out,marker,data,bytes,settings,demo,query,context,resolved);
 if(path){static unsigned calls;FILE *f=fopen(path,calls++?"a":"w");if(!f){perror("startup trace");exit(2);}
 fprintf(f,"%u %u %d %d %u %u %ld %ld %d ",settings,demo,marker->query.x,marker->query.z,marker->heading,before.rng,(long)before.state_2630,(long)before.last_sprite_53e0,rc);dump_actor(f,before.actor);fputc(' ',f);dump_actor(f,out->actor);fprintf(f," %u %ld %ld\n",out->rng,(long)out->state_2630,(long)out->last_sprite_53e0);
 if(ferror(f)||fclose(f)){perror("startup trace write");exit(2);}}
 return rc;
}
