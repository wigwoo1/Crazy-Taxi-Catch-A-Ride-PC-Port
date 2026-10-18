#include "runtime_asset.h"
#include "vehicle_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int taxi_runtime_asset_load(TaxiRuntimeAsset *out,const char *path) {
    if(!out||!path) return -1;
    FILE *f=fopen(path,"rb");if(!f) return -1;
    uint8_t *bytes=malloc(34504);TaxiRuntimeAsset *a=malloc(sizeof *a);
    if(!bytes||!a) {free(bytes);free(a);fclose(f);return -1;}
    int ok=fread(bytes,1,34504,f)==34504&&fgetc(f)==EOF;fclose(f);
    if(!ok||memcmp(bytes,"CTR1",4)) goto bad;
    taxi_trig_decode(&a->trig,bytes+4,bytes+8196);
    if(taxi_normal_angles_decode(&a->normals,bytes+16388,16386)) goto bad;
    taxi_angle_table_decode(&a->traction,bytes+32774);
    memcpy(a->templates,bytes+33288,sizeof a->templates);
    for(unsigned i=0;i<2;i++) {TaxiVehicleConfig c;if(taxi_vehicle_config_decode(&c,a->templates[i],584)) goto bad;}
    for(unsigned i=0;i<3;i++) if(taxi_spawn_marker_decode(&a->starts[i],bytes+34456+16*i,16)) goto bad;
    *out=*a;free(bytes);free(a);return 0;
bad:free(bytes);free(a);return -1;
}

unsigned taxi_runtime_template_bank(uint8_t override_1708){return override_1708!=0;}
