#ifndef TAXI_OPTIONS_MENU_ROUTE_H
#define TAXI_OPTIONS_MENU_ROUTE_H
#include <stdint.h>
typedef struct {uint32_t callback;uint16_t restore_palette_mask;} TaxiOptionsMenuRoute;
/* Source179a2..17a12, after exit and fade completion. The caller's
   existing callback is retained for branches which do not store one.
   Row5 branch exists in source, but the entry's normal count is five. */
TaxiOptionsMenuRoute taxi_options_menu_route(uint8_t row,uint8_t exit_reason,
                                            uint32_t current_callback);
/* Source1794a..1797e: confirm prompt appears on rows3/4/5 only. */
int taxi_options_menu_confirm_prompt(uint8_t row);
#endif
