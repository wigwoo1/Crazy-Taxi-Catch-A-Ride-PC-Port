#ifndef TAXI_RESULTS_GRADE_H
#define TAXI_RESULTS_GRADE_H
#include <stdint.h>
/* Original0800c59c..c646; callers normalize fractional carry first. */
uint8_t taxi_results_license(int32_t whole,int32_t fraction);
/* Original0800c648..c71a examines100 packed scores (entry+4 word,
   8-byte stride); ties are skipped. Prior rank retained if no insertion. */
uint8_t taxi_results_rank(int32_t whole,int32_t fraction,const uint32_t scores[100],uint8_t prior);
int taxi_results_scores_load(uint32_t scores[100],const char *path);
#endif
