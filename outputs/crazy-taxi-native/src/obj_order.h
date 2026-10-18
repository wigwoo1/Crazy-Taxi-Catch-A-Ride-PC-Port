#ifndef TAXI_OBJ_ORDER_H
#define TAXI_OBJ_ORDER_H
#include <stddef.h>
#include <stdint.h>
typedef struct {int16_t anchor[4];uint16_t attributes[4];} TaxiObjRequest;
/* Source280c4 paired sort. 128 is an explicit host bound, not a recovered
   producer overflow rule. Equal-depth ordering is intentionally not stable. */
int taxi_obj_sort(TaxiObjRequest *requests,size_t count);
/* Source27e68 merge: base bucket0 storage retained outside appended ranges.
   Optional resolved 32 matrices occupy attr3 of four consecutive slots each.
   Input buffers remain unchanged; host capacity errors preserve output. */
int taxi_obj_merge(const uint16_t base[128][4],size_t count0,
    const uint16_t (*bucket1)[4],size_t count1,
    const TaxiObjRequest *bucket2,size_t count2,
    const int16_t (*matrices)[4],uint16_t output[128][4]);
typedef struct {uint32_t state;int16_t values[4];} TaxiObjMatrixSlot;
/* Source28a86: match state1 before taking first state0; -1 means full.
   Matrix generation remains separate; exact signed halfword values supplied. */
int taxi_obj_matrix_slot(TaxiObjMatrixSlot slots[32],const int16_t values[4]);
/* Source28a14..62 with resolved Q8 sine/cosine and signed scale. */
int taxi_obj_matrix_values(int32_t sine,int32_t cosine,int16_t scale,int16_t values[4]);
/* Native center-relative inverse mapping; local coordinates in OBJ field.
   Returns1 for source texel,0 outside field/image,-1 invalid input. */
int taxi_obj_affine_point(const int16_t values[4],unsigned width,unsigned height,
    int double_size,int x,int y,int *source_x,int *source_y);
typedef struct {int32_t left,top;unsigned width,height;int double_size;} TaxiObjField;
/* Source28718..6e: signed scale>256 OR nonzero signed angle doubles field. */
int taxi_obj_field(int32_t center_x,int32_t center_y,unsigned width,unsigned height,
    int16_t scale,int16_t angle,TaxiObjField *field);
/* Source player offsets and Q8 forward rotation; trig resolved for -bodyroll.
   Anchor projection and alternate-bank rotate suppression remain caller-owned. */
int taxi_obj_half_centers(int32_t anchor_x,int32_t anchor_y,int32_t scale,
    int rotate,int32_t sine,int32_t cosine,int32_t centers[2][2]);
/* Source2852a..40 /28984..9a: OBJ palette mask400 adds attr0 bit1000.
   Other attribute bits are retained; pixel behavior belongs to renderer. */
uint16_t taxi_obj_palette_mask_attribute(uint16_t attr0,uint16_t obj_mask);
/* Source28774..87EA post-field origin mode from02007E2E.
   Modes1/2/3 add signedASR12 offset94F0/94F4/94F8 to X/Y/X.
   Mode4 uses unsigned fieldX<=255, source16-byte row+4 ASR8 on Y.
   Mode ownership and original table producer remain caller-owned.
   Invalid required buffers preserve field; inactive modes need no buffers. */
int taxi_obj_screen_origin(TaxiObjField *field,uint8_t mode,
    const int32_t offsets[3],const uint8_t *rows,size_t rows_size);

#endif

