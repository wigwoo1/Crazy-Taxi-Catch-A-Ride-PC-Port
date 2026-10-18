/* Original normal menu CPU composition uploaded to the native GL renderer. */
static int normal_menu_preview=-1,normal_menu_cap=31,normal_menu_loaded;
static int options_menu_preview=-1;
static int pc_display_menu_active;
static uint8_t pc_display_menu_selection;
static int normal_menu_cache_check;
static int normal_menu_flow_check;
static int32_t normal_menu_slide_q12;
static TaxiNormalMenuFlow normal_live_flow;
static TaxiMenuInput normal_live_input;
static uint8_t normal_live_main_selection,normal_live_mode_selection;
static TaxiOptionsMenuFlow options_live_flow;
static TaxiAudioMenuFlow audio_live_flow;
static TaxiControlMenu control_live;
static TaxiPaletteFade control_fade;
static uint8_t control_diagrams[22][153600];
static int control_loaded;
static void control_update(int verify);
static uint8_t options_live_selection,audio_live_selection;
static uint32_t submenu_blink;
static uint8_t submenu_volume_indices[1280];
static uint16_t submenu_volume_palette[16];
static int submenu_loaded;
static GLuint normal_menu_texture;
static GLuint normal_menu_text_original_texture;
static GLuint normal_menu_text_sdf_texture;
static TaxiFontAsset normal_menu_fonts[2];
static uint8_t normal_menu_front[131072],normal_menu_back[131072],normal_menu_icons[2048],normal_menu_pixels[153600],normal_menu_nms[1588];
static uint8_t normal_menu_text_pixels[153600];
static uint8_t normal_menu_text_sdf_pixels[153600*16];
static uint16_t normal_menu_bg[256],normal_menu_obj[16];
static TaxiMenuStrip normal_menu_strips[6];
static void load_normal_menu_sources(void){
    if(normal_menu_loaded)return;
    loading_read("../profile/background.indices",normal_menu_front,sizeof normal_menu_front);
    loading_read("../profile-layer1/background.indices",normal_menu_back,sizeof normal_menu_back);
    loading_read("../normal-menu/background-palette.rgb555",normal_menu_bg,sizeof normal_menu_bg);
    loading_read("../normal-menu/strip-palette.rgb555",normal_menu_obj,sizeof normal_menu_obj);
    loading_read("../normal-menu/footer-icons.rgba",normal_menu_icons,sizeof normal_menu_icons);
    loading_read("../normal-menu/strips.nms",normal_menu_nms,sizeof normal_menu_nms);
    if(memcmp(normal_menu_nms,"NMS1",4))fail("Normal menu strip header invalid.");
    size_t offset=4;
    for(unsigned i=0;i<6;i++){
        uint16_t m[4];if(offset+8>sizeof normal_menu_nms)fail("Normal menu strip metrics truncated.");
        memcpy(m,normal_menu_nms+offset,8);offset+=8;
        size_t size=(size_t)m[0]*m[1];if(!size||size>sizeof normal_menu_nms-offset)fail("Normal menu strip pixels truncated.");
        normal_menu_strips[i]=(TaxiMenuStrip){m[0],m[1],(int16_t)m[2],(int16_t)m[3],normal_menu_nms+offset};offset+=size;
    }
    if(offset!=sizeof normal_menu_nms)fail("Normal menu strip trailing data.");
    char path[MAX_PATH],metrics[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');if(!tail)fail("Normal menu font path failed.");
    size_t prefix=(size_t)(tail+1-path);
    for(unsigned i=0;i<2;i++){
        snprintf(path+prefix,MAX_PATH-prefix,"../assets/normal-menu/font%u-metrics.ctf",i);strcpy(metrics,path);
        snprintf(path+prefix,MAX_PATH-prefix,"../assets/normal-menu/font%u.rgba",i);
        if(taxi_font_asset_load(normal_menu_fonts+i,metrics,path,i?3:2,32,1))fail("Normal menu font source invalid.");
    }
    glGenTextures(1,&normal_menu_texture);
    glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_pixels);
    glGenTextures(1,&normal_menu_text_original_texture);
    glBindTexture(GL_TEXTURE_2D,normal_menu_text_original_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    if(texture_register_original_text(&normal_menu_text_original_texture,1))fail("Original menu text filter registration failed.");
    glGenTextures(1,&normal_menu_text_sdf_texture);
    glBindTexture(GL_TEXTURE_2D,normal_menu_text_sdf_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,960,640,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    if(texture_register_original_text(&normal_menu_text_sdf_texture,1))fail("Menu text filter registration failed.");
    normal_menu_loaded=1;
}
static void normal_menu_upload_sdf_text(int verify){
    unsigned width=0,height=0;
    if(taxi_font_sdf_expand_rgba(normal_menu_text_pixels,240,160,4,128,
        normal_menu_text_sdf_pixels,sizeof normal_menu_text_sdf_pixels,&width,&height)||width!=960||height!=640)
        fail("ROM-derived menu text SDF expansion failed.");
    glBindTexture(GL_TEXTURE_2D,normal_menu_text_sdf_texture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,960,640,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_text_sdf_pixels);
    if(verify){
        uint8_t *readback=malloc(sizeof normal_menu_text_sdf_pixels);
        if(!readback)fail("Menu text SDF readback allocation failed.");
        glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
        if(memcmp(readback,normal_menu_text_sdf_pixels,sizeof normal_menu_text_sdf_pixels))fail("Menu text SDF GPU upload mismatch.");
        free(readback);
    }
    /* Existing callers verify the 240x160 base texture immediately after this. */
    glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
}
static uint8_t *normal_menu_text_target(void){
    memset(normal_menu_text_pixels,0,sizeof normal_menu_text_pixels);
    return normal_menu_text_pixels;
}
static void normal_menu_upload_composed(int verify){
    loading_upload(normal_menu_texture,240,160,normal_menu_pixels);
    if(pc_display_settings.menu_text)normal_menu_upload_sdf_text(verify);
    else {
        glBindTexture(GL_TEXTURE_2D,normal_menu_text_original_texture);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,normal_menu_text_pixels);
        if(verify){
            uint8_t readback[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
            if(memcmp(readback,normal_menu_text_pixels,sizeof readback))fail("Original-resolution menu text GPU upload mismatch.");
        }
        glBindTexture(GL_TEXTURE_2D,normal_menu_texture);
    }
}
static void control_update(int verify){
    load_normal_menu_sources();
    if(!control_loaded){
        loading_read("../control-menu/diagram-base.rgba",control_diagrams[21],153600);
        for(unsigned layout=0;layout<3;layout++)for(unsigned page=0;page<7;page++){
            char path[80];snprintf(path,sizeof path,"../control-menu/diagram-%u-%u.rgba",layout,page);
            loading_read(path,control_diagrams[layout*7+page],153600);
        }
        control_loaded=1;
    }
    unsigned layout=(native_settings[0]>>1)&3;if(layout>2)fail("Original Control layout is outside recovered range.");
    const char *labels[]={"Steer","Left","Steer","Right","Accelerate"," ","Brake"," ","Drive","Gear","Reverse","Gear","Toggle","Distance","Config.","Control"};
    uint8_t *text_target=normal_menu_text_target();
    memcpy(normal_menu_pixels,control_diagrams[(control_live.demo_keys&32)?layout*7+control_live.page:21],153600);
    if(taxi_control_menu_text(text_target,normal_menu_fonts,labels,control_live.page,layout,31))fail("Original Control text composition failed.");
    taxi_palette_fade_cap_rgba(normal_menu_pixels,normal_menu_pixels,240*160,control_fade.progress_178a);
    if(text_target!=normal_menu_pixels)taxi_palette_fade_cap_rgba(text_target,text_target,240*160,control_fade.progress_178a);
    normal_menu_upload_composed(verify);
    if(verify){uint8_t pixels[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        if(memcmp(pixels,normal_menu_pixels,153600))fail("Control GPU pixels mismatch.");}
}
static void normal_menu_update_motion(unsigned preview,unsigned cap,int32_t slide,int verify){
    if(preview>8||cap>31)fail("Normal menu update selection/cap invalid.");
    load_normal_menu_sources();
    unsigned count=preview<5?5:4,selected=preview<5?preview:preview-5;
    uint8_t *text_target=normal_menu_text_target();
    const char *main_labels[]={"Crazy Taxi","Crazy Box","Options","Records","Save & Load"};
    const char *mode_labels[]={"Normal Rules","3 Minutes","5 Minutes","10 Minutes"};
    if(taxi_normal_menu_background_motion(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,count,selected,cap,slide)||
       taxi_normal_menu_text_motion(text_target,normal_menu_fonts,count==5?"Mode Selection":"Crazy Taxi",count==5?main_labels:mode_labels,count,cap,slide)||
       taxi_normal_menu_footer_layers_motion(normal_menu_pixels,text_target,normal_menu_fonts+1,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,normal_menu_icons,"-Confirm ",count==5?NULL:"-Back ",cap,slide))fail("Normal menu composition failed.");
    normal_menu_upload_composed(verify);
    if(verify){
        uint8_t readback[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
        if(memcmp(readback,normal_menu_pixels,sizeof readback))fail("Normal menu GPU pixels mismatch.");
    }
}
static void normal_menu_update(unsigned preview,unsigned cap,int verify){
    normal_menu_update_motion(preview,cap,0,verify);
}
static void submenu_update(int audio,uint8_t selected,int32_t slide,int verify){
    load_normal_menu_sources();
    if(!submenu_loaded){
        loading_read("../normal-menu/volume.indices",submenu_volume_indices,sizeof submenu_volume_indices);
        loading_read("../normal-menu/volume.rgb555",submenu_volume_palette,sizeof submenu_volume_palette);submenu_loaded=1;
    }
    const char *options[]={"Time","Time Diff","Traffic","Sound","Control","PC Display"};
    const char *sounds[]={"BGM Volume","SFX Volume","Effect","Music"};
    if(!audio&&pc_display_menu_active){
        uint8_t *text_target=normal_menu_text_target();
        char rows[TAXI_PC_DISPLAY_SETTING_COUNT][32];const char *labels[TAXI_PC_DISPLAY_SETTING_COUNT]={"Scenery","Internal Res","Frame Rate","Window","Aspect","Text"};
        const char *values[TAXI_PC_DISPLAY_SETTING_COUNT];for(unsigned i=0;i<TAXI_PC_DISPLAY_SETTING_COUNT;i++){
            values[i]=taxi_pc_display_settings_value(&pc_display_settings,i);
            if(!values[i]||snprintf(rows[i],sizeof rows[i],"%s: %s",labels[i],values[i])<0)fail("PC display option label failed.");
        }
        const char *items[TAXI_PC_DISPLAY_SETTING_COUNT];for(unsigned i=0;i<TAXI_PC_DISPLAY_SETTING_COUNT;i++)items[i]=rows[i];
        if(taxi_normal_menu_background_motion(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,TAXI_PC_DISPLAY_SETTING_COUNT,selected,31,slide)||
           taxi_normal_menu_text_motion(text_target,normal_menu_fonts,"PC Display",items,TAXI_PC_DISPLAY_SETTING_COUNT,31,slide)||
           taxi_normal_menu_footer_layers_motion(normal_menu_pixels,text_target,normal_menu_fonts+1,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,normal_menu_icons,NULL,"-Back ",31,slide))fail("PC display menu composition failed.");
        normal_menu_upload_composed(verify);
        if(verify){uint8_t pixels[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);if(memcmp(pixels,normal_menu_pixels,sizeof pixels))fail("PC display menu GPU upload mismatch.");}
        return;
    }
    unsigned count=audio?4:6;
    uint8_t *text_target=normal_menu_text_target();
    if(taxi_normal_menu_background_motion(normal_menu_pixels,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,count,selected,31,slide)||
       taxi_normal_menu_text_motion(text_target,normal_menu_fonts,audio?"Sound":"Options",audio?sounds:options,count,31,slide))fail("Submenu background/text composition failed.");
    TaxiGameSettings settings;memcpy(settings.bytes,native_settings,sizeof settings.bytes);
    for(unsigned row=0;row<(audio?4u:3u);row++){
        int32_t value=row%2?-slide:slide;
        int shift=value/4096-(value<0&&value%4096!=0);
        int y=(audio?54:38)+(int)row*16;
        if(!audio){
            TaxiOptionsSettingsRender r;
            if(taxi_options_settings_render(240,y,(uint8_t)row,selected,&settings,submenu_blink,&r))fail("Options visual requests failed.");
            if(!row){
                r.time.x+=shift;for(unsigned a=0;a<r.time.arrows;a++)r.time.arrow[a].x+=shift;
                if(taxi_menu_number_text(text_target,normal_menu_fonts+1,&r.time,31)||taxi_audio_menu_test_arrows(text_target,normal_menu_fonts+1,&r.time))fail("Options Time composition failed.");
            }else{
                for(unsigned i=0;i<2;i++)r.bar[i].x+=shift;
                if(taxi_audio_volume_compose(normal_menu_pixels,submenu_volume_indices,submenu_volume_palette,r.bar))fail("Options bar composition failed.");
            }
        }else if(row<2){
            TaxiAudioVolumeSprite r[2];taxi_audio_volume_render(240,y,settings.bytes[row+1],r);
            for(unsigned i=0;i<2;i++)r[i].x+=shift;
            if(taxi_audio_volume_compose(normal_menu_pixels,submenu_volume_indices,submenu_volume_palette,r))fail("Sound bar composition failed.");
        }else{
            TaxiAudioTestRender r;
            if(taxi_audio_test_render(240,y,(uint8_t)row,selected,row==2?audio_live_flow.control.sound_test:audio_live_flow.control.music_test,submenu_blink,&r))fail("Sound visual request failed.");
            r.x+=shift;for(unsigned a=0;a<r.arrows;a++)r.arrow[a].x+=shift;
            if(taxi_menu_number_text(text_target,normal_menu_fonts+1,&r,31)||taxi_audio_menu_test_arrows(text_target,normal_menu_fonts+1,&r))fail("Sound test composition failed.");
        }
    }
    const char *confirm=audio?(selected>=2?"-Play ":NULL):(taxi_options_menu_confirm_prompt(selected)?"-Confirm ":NULL);
    if(taxi_normal_menu_footer_layers_motion(normal_menu_pixels,text_target,normal_menu_fonts+1,normal_menu_front,normal_menu_back,normal_menu_bg,normal_menu_obj,normal_menu_strips,normal_menu_icons,confirm,"-Back ",31,slide))fail("Submenu footer composition failed.");
    normal_menu_upload_composed(verify);
    if(verify){uint8_t pixels[153600];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);if(memcmp(pixels,normal_menu_pixels,sizeof pixels))fail("Submenu GPU upload mismatch.");}
}
static void load_normal_menu_preview(void){
    if(normal_menu_flow_check){
        TaxiNormalMenuFlow flow;TaxiMenuInput input={0};
        taxi_normal_menu_flow_begin(&flow,1,0,1,1,1);
        for(unsigned frame=0;frame<33;frame++){
            normal_menu_update_motion(flow.choice.selection,taxi_normal_menu_flow_cap(&flow),taxi_normal_menu_flow_slide(&flow),1);
            taxi_normal_menu_flow_frame(&flow,&input,0,0,1);
        }
        for(unsigned frame=0;frame<40&&!flow.finished;frame++){
            normal_menu_update_motion(flow.choice.selection,taxi_normal_menu_flow_cap(&flow),taxi_normal_menu_flow_slide(&flow),1);
            taxi_normal_menu_flow_frame(&flow,&input,0,1,1);
        }
        if(!flow.finished||taxi_normal_main_route(flow.choice.selection,flow.choice.exiting)!=TAXI_MENU_MODE)fail("Main to mode flow failed.");
        taxi_normal_menu_flow_begin(&flow,0,0,0,1,1);
        for(unsigned frame=0;frame<33;frame++){
            normal_menu_update_motion(5+flow.choice.selection,taxi_normal_menu_flow_cap(&flow),taxi_normal_menu_flow_slide(&flow),1);
            taxi_normal_menu_flow_frame(&flow,&input,0,0,1);
        }
        for(unsigned frame=0;frame<40&&!flow.finished;frame++){
            normal_menu_update_motion(5+flow.choice.selection,taxi_normal_menu_flow_cap(&flow),taxi_normal_menu_flow_slide(&flow),1);
            taxi_normal_menu_flow_frame(&flow,&input,0,2,1);
        }
        uint8_t settings=0;TaxiNormalMenuTarget target;
        if(!flow.finished||taxi_normal_mode_route(flow.choice.selection,flow.choice.exiting,&settings,&target)||target!=TAXI_MENU_MAIN)fail("Mode back to main flow failed.");
        puts("NORMAL MENU FLOW GPU: scripted main confirm/mode Back frames verified with retained input bank; live session routing pending.");
    }
    normal_menu_update_motion((unsigned)normal_menu_preview,(unsigned)normal_menu_cap,normal_menu_slide_q12,1);
    if(normal_menu_cache_check){
        GLuint retained=normal_menu_texture;
        for(unsigned preview=0;preview<9;preview++)for(unsigned cap=0;cap<32;cap++)normal_menu_update(preview,cap,1);
        for(unsigned preview=0;preview<9;preview++)for(int position=-240;position<=240;position+=15)
            normal_menu_update_motion(preview,31,position*4096,1);
        if(normal_menu_texture!=retained)fail("Normal menu updates replaced texture ownership.");
        normal_menu_update_motion((unsigned)normal_menu_preview,(unsigned)normal_menu_cap,normal_menu_slide_q12,1);
        puts("NORMAL MENU CACHE: 288 cap frames/297 motion frames byte-exact GPU readback; same texture reused/requested frame restored.");
    }
    puts("NORMAL MENU GPU: original BG/strips/fonts/footer byte-exact upload; live flow pending.");
}
static void draw_normal_menu_preview(int width,int height){
    double pixel=height/160.0,left=(width-240*pixel)/2;
    glViewport(0,0,width,height);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glDisable(GL_ALPHA_TEST);glColor4f(1,1,1,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();result_quad(normal_menu_texture,left,0,240*pixel,height);
    if(pc_display_settings.menu_text){
        glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_TEXTURE_BIT);
        glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glColor4f(1,1,1,1);
        result_quad(normal_menu_text_sdf_texture,left,0,240*pixel,height);
        glPopAttrib();
    }else {
        glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_TEXTURE_BIT);
        glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glColor4f(1,1,1,1);
        result_quad(normal_menu_text_original_texture,left,0,240*pixel,height);
        glPopAttrib();
    }
}

