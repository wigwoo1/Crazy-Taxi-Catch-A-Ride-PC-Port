#ifndef TAXI_SAVE_FILE_H
#define TAXI_SAVE_FILE_H
#include <stdint.h>
/* Native Windows storage boundary; original512-byte data, no GBA device.
   Load0 success/1 missing/-1 invalid or IO; failed load preserves outputs.
   Write0 success/-1 failure; successful write copies prepared bytes back. */
int taxi_save_file_load(const char *path,uint8_t block[512],uint8_t runtime[800]);
int taxi_save_file_write(const char *path,uint8_t block[512]);
#endif
