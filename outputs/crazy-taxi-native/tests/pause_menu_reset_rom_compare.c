#include "pause_menu.h"
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiPauseMenu s={(uint8_t)seed,(uint8_t)(seed+1),(uint8_t)(seed+2),(uint8_t)(seed+3),(uint8_t)(seed+4),(uint8_t)(seed+5),(uint8_t)(seed+6),(uint8_t)(seed+7),(int16_t)(seed*257),(int16_t)(-((int)seed*257))};taxi_pause_menu_reset(&s);printf("%u %u %u %u %u %u %u %u %d %d\n",s.frozen,s.selection,s.dialog,s.no_selected,s.outer_state,s.exit_flag,s.palette_request,s.view_marker,s.zoom,s.view_mode);}return 0;}
