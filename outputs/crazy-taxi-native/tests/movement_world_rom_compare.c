#include "../src/movement.h"
#include "../src/world_asset.h"
#include <stdio.h>
typedef struct {TaxiWorldAsset a;} Fixture;
static int query(void *ctx,TaxiPoint pos,TaxiObstacleHit *hit) {return taxi_world_asset_obstacle(&((Fixture*)ctx)->a,pos,hit);}
int main(void){Fixture f={0};if(taxi_world_asset_load(&f.a,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge5.ctw"))return 1;TaxiPoint s,d;int preserve;while(scanf("%d%d%d%d%d%d%d",&s.x,&s.y,&s.z,&d.x,&d.y,&d.z,&preserve)==7){TaxiMovementResult o;int rc=taxi_move_position(s,d,preserve,query,&f,&o);printf("%d %d %d %d %d %d %d %u %d\n",rc,o.position.x,o.position.y,o.position.z,o.normal.x,o.normal.y,o.normal.z,o.queries,o.collided);}taxi_world_asset_free(&f.a);return 0;}
