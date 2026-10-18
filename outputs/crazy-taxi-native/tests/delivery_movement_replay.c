#include "../src/movement.h"
#include "../src/world_asset.h"
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
typedef struct{TaxiWorldAsset a;int walls;int64_t max_step;} Fixture;
static int query(void*ctx,TaxiPoint p,TaxiObstacleHit*h){Fixture*f=ctx;int kind=taxi_world_asset_obstacle(&f->a,p,h);if(kind==2)f->walls++;if(kind==1&&h->height!=INT32_MIN){int64_t step=(int64_t)h->height-p.y;if(step>f->max_step)f->max_step=step;}return kind;}
int main(void){Fixture f={0};if(taxi_world_asset_load(&f.a,"outputs/crazy-taxi-native/assets/city1.ctw"))return 1;TaxiPoint s,d;int preserve;while(scanf("%d%d%d%d%d%d%d",&s.x,&s.y,&s.z,&d.x,&d.y,&d.z,&preserve)==7){f.walls=0;f.max_step=INT64_MIN;TaxiMovementResult o={0};int rc=taxi_move_position(s,d,preserve,query,&f,&o);printf("%d %d %d %d %d %d %d %u %d %d %lld\n",rc,o.position.x,o.position.y,o.position.z,o.normal.x,o.normal.y,o.normal.z,o.queries,o.collided,f.walls,(long long)f.max_step);}taxi_world_asset_free(&f.a);return ferror(stdin)?1:0;}
