#ifndef EARNED_COMMON_CAPTURE_H
#define EARNED_COMMON_CAPTURE_H
#include "earned_traffic_capture.h"
typedef struct {EarnedTrafficCapture before;TaxiTrafficActor after[50];TaxiPoint camera;} EarnedCommonCapture;
#endif
