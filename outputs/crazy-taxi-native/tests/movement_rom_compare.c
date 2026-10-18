#include "../src/movement.h"
#include <stdio.h>
#include <stdint.h>
typedef struct { int32_t height; } Fixture;
static int query(void *ctx,TaxiPoint pos,TaxiObstacleHit *hit) { (void)pos;hit->height=((Fixture*)ctx)->height;return hit->height==INT32_MIN?0:1; }
int main(void) { TaxiPoint s,d;Fixture f;int preserve;while(scanf("%d%d%d%d%d%d%d%d",&s.x,&s.y,&s.z,&d.x,&d.y,&d.z,&f.height,&preserve)==8) { TaxiMovementResult o;int rc=taxi_move_position(s,d,preserve,query,&f,&o);printf("%d %d %d %d %d %d %d %u %d\n",rc,o.position.x,o.position.y,o.position.z,o.normal.x,o.normal.y,o.normal.z,o.queries,o.collided); }return 0; }
