#ifndef TAXI_PRESENTATION_SERVICE_H
#define TAXI_PRESENTATION_SERVICE_H
#include <stdint.h>
#include <stddef.h>
typedef struct {
 /*5F24 is pending palette restore/alternate, never a music command. */
 uint8_t pending_5f24,mode_7541,bank_7548,frozen_5f20,parity_7540;
 uint16_t display_control;
 uint16_t blend_control,blend_alpha;
 uint32_t first_754c,second_7550,current_3520,previous_3524,delta_3b00,elapsed_755c,cursor_1e10;
} TaxiPresentationService;
/* Complete26318 caller; helpers retain their own native implementation.
   Display control is a logical retained value, never a GBA hardware access.
   Callback may mutate shared owners before subsequent source reads. */
typedef int (*TaxiPresentationCall)(void *,TaxiPresentationService *,uint32_t target,uint32_t arg);
/* Retained halfwords written by5778..57A6 from display-header+80/+82.
   This is ONLY the loader prefix, not layer setup or gameplay inheritance.
   Values are unbounded uint16, not normalized/clamped blend coefficients. */
typedef struct {uint16_t alpha_00e0,alpha_00e2;} TaxiPresentationBlend;
int taxi_presentation_blend_load(TaxiPresentationBlend *,const uint8_t *header,size_t bytes);
/* Complete26440 logical blend publication and clock baseline reset. */
int taxi_presentation_reset(TaxiPresentationService *,uint16_t alpha_00e0,uint16_t alpha_00e2);
int taxi_presentation_service(TaxiPresentationService *,TaxiPresentationCall,void *);
/* Remaining26318 body ONLY after successful16208. Host pending owner supplies
   the fresh-wait gate; this function performs no wait and is not a frame scheduler. */
int taxi_presentation_service_after_wait(TaxiPresentationService *,TaxiPresentationCall,void *);
#endif
