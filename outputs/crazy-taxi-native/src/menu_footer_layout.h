#ifndef TAXI_MENU_FOOTER_LAYOUT_H
#define TAXI_MENU_FOOTER_LAYOUT_H
#include <stdint.h>
typedef struct {uint8_t descriptor;int32_t icon_x,icon_y,text_x,text_y;} TaxiMenuFooterPrompt;
typedef struct {unsigned prompts,middle_count;int32_t strip_x,strip_y;TaxiMenuFooterPrompt prompt[2];} TaxiMenuFooterLayout;
/* Source1ae50..1b03e. Text/icon widths from original fbac/28e3c remain
   explicit; footer strip count follows callback, not prompt text width. */
int taxi_menu_footer_layout(uint16_t height,uint32_t callback,int confirm,int back,
    uint16_t confirm_text_width,uint16_t confirm_icon_width,uint16_t back_icon_width,
    TaxiMenuFooterLayout *out);
#endif
