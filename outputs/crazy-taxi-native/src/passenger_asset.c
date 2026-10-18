#include "passenger_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p) {return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t signed_word(const uint8_t *p) {uint32_t u=word(p);int32_t s;memcpy(&s,&u,4);return s;}
void taxi_passenger_asset_free(TaxiPassengerAsset *a) {
    if(!a) return;
    if(a->routes) for(uint32_t i=0;i<a->route_count;i++) free(a->routes[i].points);
    free(a->source);free(a->destinations);free(a->routes);memset(a,0,sizeof *a);
}
int taxi_passenger_asset_load(TaxiPassengerAsset *out,const char *path) {
    if(!out||!path||out->source||out->destinations||out->routes||out->count||out->route_count||out->destination_count) return -1;
    FILE *f=fopen(path,"rb");if(!f) return -1;
    TaxiPassengerAsset a={0};uint8_t row[16];uint32_t total=0;
    if(fread(row,1,16,f)!=16||memcmp(row,"CTP1",4)) goto bad;
    a.count=word(row+4);a.route_count=word(row+8);a.destination_count=word(row+12);
    if(a.count>128||a.route_count>1024||a.destination_count>256) goto bad;
    if(a.count) {a.source=calloc(a.count,32);if(!a.source||fread(a.source,32,a.count,f)!=a.count) goto bad;}
    if(a.destination_count) {a.destinations=calloc(a.destination_count,48);if(!a.destinations||fread(a.destinations,48,a.destination_count,f)!=a.destination_count) goto bad;}
    for(uint32_t i=0;i<a.count;i++) if(a.source[i][3]<=9&&a.source[i][1]>=a.destination_count) goto bad;
    if(a.route_count) {a.routes=calloc(a.route_count,sizeof *a.routes);if(!a.routes) goto bad;}
    for(uint32_t i=0;i<a.route_count;i++) {
        if(fread(row,1,8,f)!=8) goto bad;
        TaxiPassengerRouteAsset *r=a.routes+i;r->source_index=word(row);r->count=word(row+4);
        if(!r->count||r->count>4096||(i&&r->source_index<=a.routes[i-1].source_index)) goto bad;
        total+=r->count;if(total>1048576) goto bad;
        r->points=calloc(r->count,sizeof *r->points);if(!r->points) goto bad;
        for(uint32_t j=0;j<r->count;j++) {
            if(fread(row,1,16,f)!=16) goto bad;
            r->points[j]=(TaxiRoutePoint){signed_word(row+4),signed_word(row+8)};
        }
    }
    if(fgetc(f)!=EOF||ferror(f)) goto bad;
    fclose(f);*out=a;return 0;
bad:
    fclose(f);taxi_passenger_asset_free(&a);return -1;
}
