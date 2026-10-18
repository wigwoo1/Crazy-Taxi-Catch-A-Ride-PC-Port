#ifndef TAXI_AUDIO_OUTPUT_H
#define TAXI_AUDIO_OUTPUT_H
#include <stdint.h>
#include <stddef.h>
typedef struct {uint8_t *samples;size_t count;} TaxiAudioOutputBuffer;
/* Original26E0E..26EDE conversion/silence and bank flip. Source-selected
 counts176/264/352 are multiples of4; buffers are native memory owners. */
int taxi_audio_output(uint32_t *bank,TaxiAudioOutputBuffer buffers[2],const int16_t *mix,
 size_t mix_capacity,uint8_t processing,uint8_t music_enabled,uint8_t playing,
 uint8_t effects_enabled,uint8_t effects_active);
#endif
