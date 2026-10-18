#include "game_settings.h"
void taxi_game_settings_defaults(TaxiGameSettings *s) {
    *s=(TaxiGameSettings){.bytes={0,32,64,68,0,50,0,0}};
}
int32_t taxi_game_initial_clock(const TaxiGameSettings *s,int32_t previous) {
    switch(s->bytes[0]&224) {
        case 0:return (int32_t)s->bytes[5]*60;
        case 32:return 180*60;
        case 64:return 300*60;
        case 96:return 600*60;
        default:return previous;
    }
}
