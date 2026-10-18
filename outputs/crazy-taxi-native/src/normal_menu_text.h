#ifndef TAXI_NORMAL_MENU_TEXT_H
#define TAXI_NORMAL_MENU_TEXT_H
#include "font_asset.h"
#include "normal_menu_background.h"
#include "audio_test_render.h"
/* Original17c34/17dfc/17e30: heading4,6 and centered font1 at120.
   labels contains original IDs130..145; diagram/footer are separate. */
int taxi_control_menu_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *const labels[16],unsigned page,unsigned layout,unsigned cap);
/* Source font1/flags4 numeric request, caller-owned row/slide anchors. */
int taxi_menu_number_text(uint8_t output[153600],const TaxiFontAsset *font,
    const TaxiAudioTestRender *request,unsigned cap);
int taxi_audio_menu_test_arrows(uint8_t output[153600],const TaxiFontAsset *font,
    const TaxiAudioTestRender *request);
/* Audio source1bd1c..be02: index+1 padded3/2 chars, flag4 centeredX,
   no vertical adjustment, original font1. Settled240x160 view. */
int taxi_audio_menu_test_text(uint8_t output[153600],const TaxiFontAsset *font,
    int16_t sound_index,int16_t music_index,unsigned cap);
/* Original normal menu heading/font0 at4/6; centered font1 rows at72.
   Draw on an already composed BG/strip buffer. Footer excluded. */
int taxi_normal_menu_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
                         const char *heading,const char *const *choices,unsigned count,unsigned cap);
/* 1b1d4 heading uses094f0; c074 negates it after each list row.
   Scoped original240-pixel viewport displacement, expressed Q12. */
int taxi_normal_menu_text_motion(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *heading,const char *const *choices,unsigned count,unsigned cap,int32_t slide_q12);
/* Null prompt omits its icon/text; Back can appear alone (audio volumes). */
int taxi_normal_menu_footer(uint8_t output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap);
int taxi_normal_menu_footer_motion(uint8_t output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap,int32_t slide_q12);
/* Same footer result, but keep strip/icon pixels in output and ROM glyphs in
   the transparent text_output layer for high-resolution text presentation. */
int taxi_normal_menu_footer_layers_motion(uint8_t output[153600],uint8_t text_output[153600],const TaxiFontAsset *font,
    const uint8_t front[131072],const uint8_t back[131072],const uint16_t bg[256],
    const uint16_t obj[16],const TaxiMenuStrip strips[6],const uint8_t icons[2048],
    const char *confirm,const char *back_text,unsigned cap,int32_t slide_q12);
int taxi_challenge_briefing_text(uint8_t output[153600],const TaxiFontAsset fonts[2],const char *title,unsigned submode,const char *page_header,const char *const lines[4],unsigned cap);
/* Settled challenge1B488/1B614/1B6EC text only. Badge bits0..8
   suppress numbered text; actual buttons/check sprite are separate. */
int taxi_challenge_grid_text(uint8_t output[153600],const TaxiFontAsset fonts[2],
    const char *heading,const char *name_label,const char *driver_label,
    uint16_t badge_mask,unsigned cap);
typedef struct {
    const TaxiFontAsset *fonts;
    const uint8_t *front,*back,*icons;
    const uint16_t *bg,*obj;
    const TaxiMenuStrip *strips;
} TaxiChallengeScreenAssets;
/* Settled briefing composition; atomic output on error. Caller provides
   source labels/page lines and owns slide/fade timing and input. */
int taxi_challenge_briefing_screen(uint8_t output[153600],
    const TaxiChallengeScreenAssets *assets,const char *title,unsigned submode,
    const char *page_header,const char *const lines[4],const char *toggle_label,
    const char *back_label,unsigned cap);
/* Source201EA..202B2. Gameplay group0 font (at least3 banks),
   original labels601/609/610/611; caller owns voice/hold visibility gates. */
int taxi_challenge_failure_text(uint8_t output[153600],const TaxiFontAsset *font,
    const char *const labels[4],unsigned selected,unsigned cap);
/* Source1FDEE..201D8: caller supplies original formatted result and
   ranking strings; labels600/602/603/606/607/608, gameplay font0. */
int taxi_challenge_success_text(uint8_t output[153600],const TaxiFontAsset *font,
    const char *const labels[6],const char *name,const char *result,
    const char *const records[3],const char *const names[3],unsigned cap);
#endif
