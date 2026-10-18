#ifndef TAXI_GROUND_H
#define TAXI_GROUND_H
#include <stdint.h>
#include <stddef.h>
typedef struct { int32_t x,y,z; } TaxiPoint;
typedef struct {
    int32_t height;
    TaxiPoint normal;
    uint16_t polygon;
    uint16_t material;
    uint32_t contact_value;
    int contact_valid;
} TaxiGroundHit;
typedef struct {
    /* Original contact offset 0 is the polygon's material byte, stored as
       a word by 08029664. Geometry indices remain in TaxiGroundHit. */
    uint32_t material;
    int32_t height;
    TaxiPoint normal;
    uint8_t flags,red,green,blue;
    uint32_t contact_value;
} TaxiWorldContact;
/* State packaging of 0802aba0 after a ground query. A missing height leaves
   output unchanged. A zero normal is packaged but returns false, as original. */
int taxi_world_contact(TaxiWorldContact *out,const TaxiGroundHit *hit);
/* Raw little-endian ROM-format vertices (16-byte stride), polygons (24),
   and one cell's uint16 candidate list. No ARM execution or GBA runtime.
   Returns 1 for a hit, 0 for none, -1 for invalid referenced data.
   Includes 0802a978 interpolation of the fourth vertex word. Its gameplay
   meaning remains unproven; contact_valid indicates a value was written.
   y=INT32_MIN requests the original first-hit mode. */
int taxi_ground_candidates(const uint8_t *vertices, size_t vertex_count,
    const uint8_t *polygons, size_t polygon_count,
    const uint8_t *references, size_t count, TaxiPoint point, TaxiGroundHit *hit);
/* Height-only08029b44 candidate projection: same geometry/selection as ground,
   without contact-word interpolation. No hit writes INT32_MIN; invalid data
   leaves output unchanged. Input Y is explicit: nearest-height or first hit. */
int taxi_ground_height_candidates(const uint8_t *vertices,size_t vertex_count,
    const uint8_t *polygons,size_t polygon_count,const uint8_t *references,
    size_t count,TaxiPoint point,int32_t *height);
/* Complete scalar interpolation routine 0802a978. Returns 0 without writing
   output if no crossing edge pair exists or output is null. */
int taxi_ground_contact(int32_t x,int32_t z,const uint8_t *a,
    const uint8_t *b,const uint8_t *c,uint32_t *output);
typedef struct {
    int32_t height;
    uint16_t polygon;
    unsigned edge_count;
    TaxiPoint vertices[4];
} TaxiObstacleHit;
/* 08029e84 candidate portion: 2=material-flag blocker, 1=positive-Y
   surface, 0=none, -1=invalid data. Materials have original 28-byte stride. */
int taxi_obstacle_candidates(const uint8_t *vertices,size_t vertex_count,
    const uint8_t *polygons,size_t polygon_count,const uint8_t *materials,
    size_t material_count,const uint8_t *references,size_t count,
    TaxiPoint point,TaxiObstacleHit *hit);
#endif
