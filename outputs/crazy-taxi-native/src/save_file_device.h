#ifndef TAXI_SAVE_FILE_DEVICE_H
#define TAXI_SAVE_FILE_DEVICE_H
#include "save_load.h"
typedef enum {TAXI_SAVE_DEVICE_READY=0,TAXI_SAVE_DEVICE_MISSING=1,TAXI_SAVE_DEVICE_WRONG_SIZE=2} TaxiSaveFileDeviceStatus;
typedef struct {uint8_t bytes[512];TaxiSaveFileDeviceStatus status;unsigned reads,verifications;} TaxiSaveFileDevice;
/* Snapshot an existing PC save read-only; absent/wrong-sized files expose blank
   EEPROM bytes for the original checksum/defaults path. Other I/O errors fail. */
int taxi_save_file_device_open(TaxiSaveFileDevice *,const char *);
/* 1 handled,0 other service,-1 invalid ownership. No file writes.
   Source320A4 records timer callback3548 identity32081 separately from the
   software frame hook352C. Timer hardware is not emulated. */
int taxi_save_file_device_call(TaxiSaveFileDevice *,TaxiSaveLoad *,uint32_t target,uint32_t argument0,uint32_t argument1,uint32_t *result);
#endif
