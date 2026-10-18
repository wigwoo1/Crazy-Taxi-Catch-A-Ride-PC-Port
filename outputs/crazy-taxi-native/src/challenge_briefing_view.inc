/* Generated only from byte-verified original English challenge assets. */
static int challenge_briefing_preview=-1;
static const struct {const char *title;const char *lines[4];} challenge_pages[27]={
{"Crazy Jump",{"Jump past the K point."," "," "," "}},
{"Crazy Jump",{"For Quick acceleration, nothing","is better than the CRAZY DASH!!"," "," "}},
{"Crazy Jump",{"To do a CRAZY DASH, let off","the accelerator, shift into D,","then press the accelerator."," "}},
{"Crazy Flag",{"Run over the flag behind the","car within the time limit."," "," "}},
{"Crazy Flag",{"The CRAZY DRIFT is useful for","making sudden turns after","starting."," "}},
{"Crazy Flag",{"To execute a CRAZY DRIFT, shift","into R gear, then shift into D","gear."," "}},
{"Crazy Balloons",{"Pop all of the balloons","within the time limit."," "," "}},
{"Crazy Balloons",{"The CRAZY DRIFT is great to use","for crash turns."," "," "}},
{"Crazy Balloons",{"For a really cool ride, execute","a CRAZY DASH right after a","CRAZY DRIFT!"," "}},
{"Crazy Drift",{"Perform Crazy Drifts to earn a","minimum of 15 COMBO's within","the time limit."," "}},
{"Crazy Drift",{"Do the CRAZY DRIFT to earn","combos!!"," "," "}},
{"Crazy Drift",{"To execute a CRAZY DRIFT, shift","into R gear then shift into D","gear."," "}},
{"Crazy Turn",{"Take the customer to their","destination within the time","limit."," "}},
{"Crazy Turn",{"The CRAZY DRIFT is indispensable","when making U-turns."," "," "}},
{"Crazy Turn",{"Execute a CRAZY DASH right after","cornering to gain speed."," "," "}},
{"Crazy Bound",{"Without falling into the sea,","deliver the customer within","the time limit."," "}},
{"Crazy Bound",{"Fly cool with the CRAZY DRIFT","JUMP!"," "," "}},
{"Crazy Bound",{"Do a CRAZY DRIFT to perform a","CRAZY DRIFT JUMP!"," "," "}},
{"Crazy Rush",{"Take all of the customers to","their destinations within the","time limit."," "}},
{"Crazy Rush",{"Execute the CRAZY DRIFT STOP so","that the car is facing towards","the next destination."," "}},
{"Crazy Rush",{"A CRAZY DRIFT STOP is made by","stopping while doing a CRAZY","DRIFT!"," "}},
{"Crazy ZigZag",{"Without falling into the sea,","deliver the customer within","the time limit."," "}},
{"Crazy ZigZag",{"The CRAZY DRIFT is a must when","cornering."," "," "}},
{"Crazy ZigZag",{"You'll never make it if you don't","do a CRAZY DASH after cornering!"," "," "}},
{"Crazy Pole",{"Take all of the customers to","their destinations within the","time limit."," "}},
{"Crazy Pole",{"Crash into a pole while braking","to execute a crashing stop!"," "," "}},
{"Crazy Pole",{"Use the CRAZY DRIFT when stopping","to make it easier to continue on!"," "," "}}};
static void challenge_briefing_update(unsigned preview){
    if(preview>=27)fail("Challenge briefing preview index invalid.");
    load_normal_menu_sources();
    TaxiChallengeScreenAssets assets={normal_menu_fonts,normal_menu_front,normal_menu_back,
        normal_menu_icons,normal_menu_bg,normal_menu_obj,normal_menu_strips};
    if(taxi_challenge_briefing_screen(normal_menu_pixels,&assets,challenge_pages[preview].title,
        preview/3," ",challenge_pages[preview].lines,preview%3?"-Rules ":"-Advice ","-Back ",31))fail("Challenge briefing composition failed.");
    glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_pixels);
    uint8_t readback[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
    if(memcmp(readback,normal_menu_pixels,sizeof readback))fail("Challenge briefing GPU upload mismatch.");
}

static int challenge_grid_preview=-1,challenge_grid_loaded;
static uint8_t challenge_grid_sprites[12288];
static void challenge_grid_update(unsigned selected){
    if(selected>10)fail("Challenge grid selection invalid.");
    load_normal_menu_sources();
    if(!challenge_grid_loaded){
        loading_read("../challenge-menu/grid-sprites.rgba",challenge_grid_sprites,sizeof challenge_grid_sprites);
        challenge_grid_loaded=1;
    }
    uint16_t badge_mask=0;
    for(unsigned mode=0;mode<9;mode++){
        int badge=taxi_challenge_record_badge(native_settings,native_current_profile,(uint8_t)mode);
        if(badge<0)fail("Challenge record profile invalid.");
        if(badge)badge_mask|=(uint16_t)(1u<<mode);
    }
    if(taxi_menu_background_only(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,31)||
       taxi_challenge_profile_panels(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,selected,31)||
       taxi_challenge_grid_buttons(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,challenge_grid_sprites,selected,badge_mask,31)||
       taxi_challenge_grid_text(normal_menu_pixels,normal_menu_fonts,"Crazy Box","Name","Cabbie",badge_mask,31))fail("Challenge grid composition failed.");
    glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_pixels);
    uint8_t readback[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
    if(memcmp(readback,normal_menu_pixels,sizeof readback))fail("Challenge grid GPU upload mismatch.");
}

static TaxiChallengeMenuFlow challenge_live_flow;
static uint8_t challenge_live_selection,challenge_live_driver,challenge_live_submode;
static void challenge_live_update(void){
    if(challenge_live_flow.is_briefing){
        unsigned page=challenge_live_flow.briefing.page3875?
            1+challenge_live_flow.briefing.blink:0;
        challenge_briefing_update(3*challenge_live_submode+page);
    }else challenge_grid_update(challenge_live_flow.grid.action.selection3886);
    /* Settled original composition; per-layer source slide remains pending. */
    if(!taxi_palette_fade_preserves_palette(challenge_live_flow.mask,challenge_live_flow.mask)){
        taxi_palette_fade_cap_rgba(normal_menu_pixels,normal_menu_pixels,240*160,
            challenge_live_flow.fade.progress_178a);
        glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_pixels);
    }
}
static int challenge_live_begin(int briefing,int reinitialize){
    if(taxi_challenge_menu_flow_begin(&challenge_live_flow,briefing,reinitialize,
        native_settings,&native_current_profile,challenge_live_selection,
        normal_live_flow.fade.interval_1771,normal_live_flow.fade.reload_1772))
        fail("Native challenge callback entry failed.");
    challenge_live_update();
    return briefing?16:15;
}
