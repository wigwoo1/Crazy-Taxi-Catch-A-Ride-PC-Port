import pathlib
p=pathlib.Path('outputs/crazy-taxi-native/src/scene_viewer.c')
s=p.read_text()
s='#include "title_prompt.h"\n#include "title_scroll.h"\n#include "title_flow.h"\n'+s
anchor='static void draw(int width, int height) {'
code=r'''static int title_entry_check;
static GLuint title_texture;
static uint8_t title_pixels[240*160*4];
static void load_native_title_check(void){
    uint8_t *background=malloc(512*256*4),prompt[128*16*4];
    if(!background)fail("Title allocation failed.");
    char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);
    char *tail=strrchr(path,'\\');if(!tail)fail("Title asset path failed.");tail++;
    const char *files[2]={"title/background.rgba","state5-live-strip/language-0.rgba"};
    void *dest[2]={background,prompt};size_t sizes[2]={512*256*4,sizeof prompt};
    for(unsigned i=0;i<2;i++){
        snprintf(tail,MAX_PATH-(size_t)(tail-path),"../assets/%s",files[i]);FILE *f=fopen(path,"rb");
        if(!f||fread(dest[i],1,sizes[i],f)!=sizes[i]||fgetc(f)!=EOF)fail("Original title asset invalid.");
        fclose(f);
    }
    TaxiTitleScroll scroll={0};TaxiTitleFlow flow;taxi_title_flow_init(&flow);
    TaxiPaletteFade fade={0};taxi_palette_fade_begin(&fade,1);TaxiMenuInput input={0};
    for(unsigned frame=0;frame<50;frame++){
        taxi_title_scroll_step(&scroll,1,240);
        TaxiTitleActions a;taxi_title_flow_step(&flow,1,fade.active_177b,1,input.repeat[0],input.repeat[3],0,&a);
        taxi_palette_fade_step(&fade,1);taxi_menu_input_step(&input,0,1);
    }
    TaxiTitlePrompt requests[4];
    if(scroll.timer!=34||scroll.x_q8!=272*256||fade.active_177b||
       taxi_title_prompt(scroll.timer,50,fade.active_177b,240,160,requests)!=4)fail("Native title entry gates failed.");
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)
        memcpy(title_pixels+4*(y*240+x),background+4*(y*512+x+272),4);
    for(unsigned half=0;half<4;half++)for(unsigned y=0;y<16;y++)for(unsigned x=0;x<32;x++){
        const uint8_t *art=prompt+4*(y*128+half*32+x);
        if(art[3])memcpy(title_pixels+4*((requests[half].y+(int)y)*240+requests[half].x+(int)x),art,4);
    }
    /* Native repeat adapter feeds source confirm one frame later. */
    taxi_menu_input_step(&input,1,1);TaxiTitleActions a;
    taxi_title_flow_step(&flow,1,0,1,input.repeat[0],input.repeat[3],1,&a);
    if(!a.begin_exit_fade||a.sound_index!=5||!flow.menu_requested)fail("Native title confirm gate failed.");
    glGenTextures(1,&title_texture);glBindTexture(GL_TEXTURE_2D,title_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,240,160,0,GL_RGBA,GL_UNSIGNED_BYTE,title_pixels);
    uint8_t readback[240*160*4];glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,readback);
    if(memcmp(readback,title_pixels,sizeof readback)||glGetError()!=GL_NO_ERROR)fail("Title GPU upload mismatch.");
    free(background);
    puts("PASS native title entry: original BG/prompt, 34-update scroll, fade gate, source repeat confirm and byte-exact GPU upload; menu parent/attract not integrated.");
}
static void draw_native_title_check(int width,int height){
    glClearColor(1,0.84f,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    float scale=(float)height/160,offset=width*0.5f-120*scale;
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,title_texture);glColor4f(1,1,1,1);
    glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2f(offset,0);glTexCoord2i(1,0);glVertex2f(offset+240*scale,0);
    glTexCoord2i(1,1);glVertex2f(offset+240*scale,height);glTexCoord2i(0,1);glVertex2f(offset,height);glEnd();
    glDisable(GL_TEXTURE_2D);glEnable(GL_DEPTH_TEST);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
}
static void draw(int width, int height) {
    if(title_entry_check){draw_native_title_check(width,height);return;}
'''
code=code.replace("'\\\\'","'\\'")
s=s.replace(anchor,code)
s=s.replace('else if(!strcmp(argv[i],"--startup-parent-check"))','else if(!strcmp(argv[i],"--title-entry-check"))title_entry_check=1;\n        else if(!strcmp(argv[i],"--startup-parent-check"))')
s=s.replace('    if (image_path) {','    if(title_entry_check){if(!image_path||startup_mode||drive_mode)fail("Title entry check requires standalone snapshot.");load_native_title_check();}\n    if (image_path) {')
p.write_text(s)
b=pathlib.Path('outputs/crazy-taxi-native/build.ps1')
s=b.read_text().replace("(Join-Path $PSScriptRoot 'src/scene_viewer.c')","(Join-Path $PSScriptRoot 'src/title_prompt.c') (Join-Path $PSScriptRoot 'src/title_scroll.c') (Join-Path $PSScriptRoot 'src/title_flow.c') (Join-Path $PSScriptRoot 'src/scene_viewer.c')")
b.write_text(s)
