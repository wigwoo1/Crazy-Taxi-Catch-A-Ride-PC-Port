#ifndef TAXI_TEXT_LAYOUT_H
#define TAXI_TEXT_LAYOUT_H
#include <stdint.h>
/* Original f99c..fa38/fc30..fcb6 spacing correction. Metrics are the
   nine source halfwords; pair table excludes its count header. */
int32_t taxi_text_correction_q8(const uint16_t *metrics,uint16_t count,
    uint16_t current,uint16_t next,const uint8_t *pairs,int32_t spacing,int16_t scale);
uint32_t taxi_text_measure(const uint16_t *metrics,uint16_t count,const char *text,const uint8_t *pairs,int32_t spacing);
/* Original F948..F95E truncates Q8 before glyph offsets. */
int32_t taxi_text_glyph_coordinate(int32_t q8,int16_t offset);
#endif
