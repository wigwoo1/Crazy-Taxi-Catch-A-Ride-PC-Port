#ifndef TAXI_MUSIC_PLAYBACK_H
#define TAXI_MUSIC_PLAYBACK_H
#include <stdint.h>
typedef enum {TAXI_MUSIC_STOP,TAXI_MUSIC_PAUSE,TAXI_MUSIC_RESUME} TaxiMusicPlayback;
/* Original27730/2773C/27754 owner byte020075F6. Pause preserves tracker
   position/voices; resume changes only zero to1, retains other raw values. */
int taxi_music_playback(uint8_t *playing,TaxiMusicPlayback command);
#endif
