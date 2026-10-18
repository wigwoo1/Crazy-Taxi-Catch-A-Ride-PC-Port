#ifndef TAXI_HUD_START_H
#define TAXI_HUD_START_H
#include <stdint.h>
/* Legacy driver field represents original388D language index; driver owners are1539/1690. */
typedef struct {uint8_t demo,settings,driver,challenge_driver,session_driver;} TaxiHudStartInput;
/* Source addresses identify recovered owners/resources, never host pointers. */
typedef struct {
 void *context;
 uint32_t (*resource_word)(void *,uint32_t);
 void (*write)(void *,uint32_t,uint32_t,unsigned);
 int (*replace)(void *,uint32_t,unsigned,unsigned,unsigned,unsigned);
} TaxiHudStartBackend;
int taxi_hud_start(const TaxiHudStartInput *,const TaxiHudStartBackend *);
#endif
