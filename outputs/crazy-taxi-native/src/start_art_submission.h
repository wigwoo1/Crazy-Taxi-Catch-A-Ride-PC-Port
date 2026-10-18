#ifndef TAXI_START_ART_SUBMISSION_H
#define TAXI_START_ART_SUBMISSION_H
#include <stdint.h>
typedef struct {unsigned count,affine;int32_t x[6],y;uint8_t frame[6];int16_t scale;} TaxiStartArtSubmission;
/* Original1FB7C..1FD22 request geometry; callbacks28288/28658 remain separate. */
int taxi_start_art_submission(TaxiStartArtSubmission *,uint8_t active,int16_t phase,uint16_t width,int16_t offset,int16_t scale);
#endif
