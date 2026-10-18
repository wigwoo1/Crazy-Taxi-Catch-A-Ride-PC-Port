#ifndef CRAZY_TAXI_INPUT_H
#define CRAZY_TAXI_INPUT_H
#include <stdint.h>

/* Recovered from USA ROM 08015c3c..08015d94; not yet game-integrated.
   Button indices use the original hardware order: A B Select Start Right Left Up Down R L.
   No CPU interpreter, GBA memory map, or hardware runtime is used here. */
typedef struct {
    uint16_t raw_keys;           /* original 020034de */
    uint16_t auxiliary_mask;     /* original 020034dc; semantic role unknown */
    uint16_t pressed_mask;       /* original 020034e0 */
    uint8_t held[10];            /* original 020034e2 */
    uint8_t previous[10];        /* original 020034ec */
    uint8_t repeat[10];          /* original 020034f6 */
    uint8_t pressed[10];         /* original 02003500 */
    uint16_t countdown[10];      /* original 0200350a */
} TaxiInput;

typedef struct {
    uint16_t active_low_keys;    /* Host adapter supplies the original ten button bits. */
    uint8_t suppress_physical;   /* original 02001eaa */
    uint8_t override_enabled;    /* original 02001708; purpose not yet identified */
    uint16_t override_mask;      /* original 0200170c */
    uint32_t elapsed_ticks;      /* original 02003b00; units not yet established */
} TaxiInputSample;

/* Original16100: clear held/previous/repeat/countdown only; retain raw keys,
   auxiliary/pressed masks and ten pressed bytes. */
void taxi_input_startup_reset(TaxiInput *state);

void taxi_input_update(TaxiInput *state, TaxiInputSample sample);
#endif
