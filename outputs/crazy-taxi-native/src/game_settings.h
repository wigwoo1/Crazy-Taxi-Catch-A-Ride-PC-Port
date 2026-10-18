#ifndef TAXI_GAME_SETTINGS_H
#define TAXI_GAME_SETTINGS_H
#include <stdint.h>
/* First eight original settings bytes02001eb0. Save/checksum/score data outside
   these bytes are not represented. New-settings defaults0800ffea..0801001e. */
typedef struct {uint8_t bytes[8];} TaxiGameSettings;
void taxi_game_settings_defaults(TaxiGameSettings *settings);
/* Remaining clock initialization0800b976..0800b9e4. Other modes preserve
   previous remaining clock, including challenge80. Units60/displayedsecond. */
int32_t taxi_game_initial_clock(const TaxiGameSettings *settings,int32_t previous);
#endif
