#ifndef TAXI_AI_PATH_H
#define TAXI_AI_PATH_H
#include <stddef.h>
#include <stdint.h>
typedef struct {uint32_t distance;int32_t x,z;uint32_t unused;} TaxiAiPathPoint;
typedef struct {TaxiAiPathPoint *points;size_t count;} TaxiAiPathAsset;
int taxi_ai_path_load(TaxiAiPathAsset *out,const char *path);
void taxi_ai_path_free(TaxiAiPathAsset *asset);
/* Source5714: first inclusive segment, low16 query, ARM32 multiply/ASR.
   Original missing segment invokes diagnostic2c880; native returns-1.
   Y is not written by original routine. */
int taxi_ai_path_target(const TaxiAiPathPoint *points,size_t count,uint32_t query,
    int32_t position[3]);
#endif
