#ifndef TAXI_BODY_POINTS_H
#define TAXI_BODY_POINTS_H
#include "impact.h"
typedef struct {TaxiPoint positions[6],vectors[6];} TaxiBodyPoints;
/* 080145e4: positions at actor3d8..428, vectors438..488, stride16.
   Vectors omit actor translation; heading+drift uses signed ASR12. */
int taxi_body_points(TaxiBodyPoints *,TaxiPoint actor,uint16_t pitch,
 uint16_t roll,int32_t heading,int32_t drift,const TaxiTrig *);
#endif
