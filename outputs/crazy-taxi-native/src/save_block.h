#ifndef TAXI_SAVE_BLOCK_H
#define TAXI_SAVE_BLOCK_H
#include <stdint.h>
/* Original settings buffer02001eb0..020020af. Source10a80 sums504 bytes;
  10154 clears transient flags0x18 then writes LE checksum at offset508.
   Bytes504..507 are retained, not assumed padding or a new PC signature. */
uint32_t taxi_save_block_checksum(const uint8_t block[512]);
void taxi_save_block_prepare(uint8_t block[512]);
int taxi_save_block_checksum_valid(const uint8_t block[512]);
/* Original0800ffe0 initializer. first_ten is the original80-byte ROM table;
   runtime contains100 complete8-byte records, including names/flags. */
void taxi_save_block_defaults(uint8_t block[512],uint8_t runtime[800],
    const uint8_t first_ten[80]);
/* Source1039a checksum acceptance and1043c..1045e post-read effects.
   Rejects zero checksum, copies only first ten runtime rows, clears bit0.
   Device read/retry/fallback and any other validation remain caller-owned. */
int taxi_save_block_accept_read(uint8_t block[512],uint8_t runtime[800],
    const uint8_t incoming[512]);
#endif
