#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "normal_menu_background.h"
#include "normal_menu_text.h"
#include "audio_volume_render.h"
#include "options_settings_render.h"
static uint8_t front[131072],back[131072],output[153600],expected[153600],text_layer[153600],data[6][4096];
static void bitmap(const char *path){
    uint8_t header[54]={0x42,0x4d};uint32_t size=54+240*160*4,offset=54,dib=40,width=240,height=160;
    memcpy(header+2,&size,4);memcpy(header+10,&offset,4);memcpy(header+14,&dib,4);
    memcpy(header+18,&width,4);memcpy(header+22,&height,4);header[26]=1;header[28]=32;
    FILE *file=fopen(path,"wb");assert(file);assert(fwrite(header,1,54,file)==54);
    for(int y=159;y>=0;y--)for(unsigned x=0;x<240;x++){
        const uint8_t *p=output+(y*240+x)*4;uint8_t bgra[]={p[2],p[1],p[0],p[3]};
        assert(fwrite(bgra,1,4,file)==4);
    }
    assert(!fclose(file));
}
static void read_file(const char *path,void *out,size_t size){
    FILE *file=fopen(path,"rb");assert(file);assert(fread(out,1,size,file)==size);
    assert(fgetc(file)==EOF&&!ferror(file));fclose(file);
}
int main(void){
    uint16_t palette[256],obj[16];TaxiMenuStrip strips[6];
    read_file("outputs/crazy-taxi-native/assets/profile/background.indices",front,sizeof front);
    read_file("outputs/crazy-taxi-native/assets/profile-layer1/background.indices",back,sizeof back);
    read_file("outputs/crazy-taxi-native/assets/normal-menu/background-palette.rgb555",palette,sizeof palette);
    read_file("outputs/crazy-taxi-native/assets/normal-menu/strip-palette.rgb555",obj,sizeof obj);
    FILE *file=fopen("outputs/crazy-taxi-native/assets/normal-menu/strips.nms","rb");assert(file);
    char magic[4];assert(fread(magic,1,4,file)==4&&!memcmp(magic,"NMS1",4));
    for(unsigned f=0;f<6;f++){
        uint16_t m[4];assert(fread(m,2,4,file)==4);assert(m[0]*m[1]<=4096);
        assert(fread(data[f],1,m[0]*m[1],file)==m[0]*m[1]);
        strips[f]=(TaxiMenuStrip){m[0],m[1],(int16_t)m[2],(int16_t)m[3],data[f]};
    }
    assert(fgetc(file)==EOF&&!ferror(file));fclose(file);
    for(unsigned count=4;count<=5;count++)for(unsigned selected=0;selected<count;selected++){
        char path[256];snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/normal-menu/%s-strips-%u.rgba",count==5?"main":"mode",selected);
        read_file(path,expected,sizeof expected);
        assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,count,selected,31));
        assert(!memcmp(output,expected,sizeof output));
        assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,count,selected,0));
        for(unsigned i=0;i<38400;i++)assert(!output[i*4]&&!output[i*4+1]&&!output[i*4+2]&&output[i*4+3]==255);
    }
    memset(output,123,sizeof output);uint8_t original=data[0][0];data[0][0]=16;
    assert(taxi_normal_menu_background(output,front,back,palette,obj,strips,5,0,31)==-1);
    for(unsigned i=0;i<sizeof output;i++)assert(output[i]==123);
    data[0][0]=original;
    TaxiFontAsset fonts[2]={{0}};
    for(unsigned i=0;i<2;i++){
        char metrics[256],pixels[256];
        snprintf(metrics,sizeof metrics,"outputs/crazy-taxi-native/assets/normal-menu/font%u-metrics.ctf",i);
        snprintf(pixels,sizeof pixels,"outputs/crazy-taxi-native/assets/normal-menu/font%u.rgba",i);
        assert(!taxi_font_asset_load(fonts+i,metrics,pixels,i?3:2,32,1));
    }
    uint8_t volume_indices[1280];uint16_t volume_palette[16];
    read_file("outputs/crazy-taxi-native/assets/normal-menu/volume.indices",volume_indices,sizeof volume_indices);
    read_file("outputs/crazy-taxi-native/assets/normal-menu/volume.rgb555",volume_palette,sizeof volume_palette);
    const char *audio_labels[]={"BGM Volume","SFX Volume","Effect","Music"};
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,4,0,31));
    assert(!taxi_normal_menu_text(output,fonts,"Sound",audio_labels,4,31));
    for(unsigned row=0;row<2;row++){
        TaxiAudioVolumeSprite requests[2];
        assert(!taxi_audio_volume_render(240,54+(int)row*16,row?64:32,requests));
        assert(!taxi_audio_volume_compose(output,volume_indices,volume_palette,requests));
    }
    bitmap("outputs/crazy-taxi-native/reports/audio-menu-partial.bmp");
    assert(!taxi_audio_menu_test_text(output,fonts+1,0,0,31));
    bitmap("outputs/crazy-taxi-native/reports/audio-menu-test-values.bmp");
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,4,2,31));
    assert(!taxi_normal_menu_text(output,fonts,"Sound",audio_labels,4,31));
    assert(!taxi_audio_menu_test_text(output,fonts+1,1,0,31));
    for(unsigned row=0;row<2;row++){
        TaxiAudioVolumeSprite requests[2];
        assert(!taxi_audio_volume_render(240,54+(int)row*16,row?64:32,requests));
        assert(!taxi_audio_volume_compose(output,volume_indices,volume_palette,requests));
    }
    TaxiAudioTestRender test_request;
    assert(!taxi_audio_test_render(240,86,2,2,1,32,&test_request));
    assert(!taxi_audio_menu_test_arrows(output,fonts+1,&test_request));
    bitmap("outputs/crazy-taxi-native/reports/audio-menu-arrows.bmp");
    const char *labels[]={"Crazy Taxi","Crazy Box","Options","Records","Save & Load"};
    memset(output,0,sizeof output);
    assert(!taxi_normal_menu_text(output,fonts,"Mode Selection",labels,5,31));
    unsigned ink=0;for(unsigned i=0;i<38400;i++)ink+=output[i*4+3]!=0;assert(ink>0);
    memcpy(expected,output,sizeof output);
    memset(output,0,sizeof output);
    assert(!taxi_normal_menu_text_motion(output,fonts,"Mode Selection",labels,5,31,0));
    assert(!memcmp(output,expected,sizeof output));
    assert(taxi_normal_menu_text_motion(output,fonts,"Mode Selection",labels,5,31,240*4096+1)==-1);
    assert(!memcmp(output,expected,sizeof output));
    /* All English glyph ink lies within its row band. Compare the moving
       render to independently translated settled pixels, including clipping. */
    memset(output,0,sizeof output);
    assert(!taxi_normal_menu_text_motion(output,fonts,"Mode Selection",labels,5,31,-8*4096));
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        int shift=-8;
        if(y>=40&&y<126)shift=((y-40)/16)%2?8:-8;
        int source=(int)x-shift;
        uint8_t zero[4]={0,0,0,0};
        const uint8_t *p=source>=0&&source<240?expected+(y*240+source)*4:zero;
        assert(!memcmp(output+(y*240+x)*4,p,4));
    }
    memcpy(output,expected,sizeof output);
    const char *bad[]={"Crazy Taxi","\x01","Options","Records","Save & Load"};
    assert(taxi_normal_menu_text(output,fonts,"Mode Selection",bad,5,31)==-1);
    assert(!memcmp(output,expected,sizeof output));
    memset(output,0,sizeof output);
    assert(!taxi_normal_menu_text(output,fonts,"Mode Selection",labels,5,0));
    for(unsigned i=0;i<38400;i++)assert(!output[i*4]&&!output[i*4+1]&&!output[i*4+2]);
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,5,0,31));
    assert(!taxi_normal_menu_text(output,fonts,"Mode Selection",labels,5,31));
    uint8_t icons[2048];read_file("outputs/crazy-taxi-native/assets/normal-menu/footer-icons.rgba",icons,sizeof icons);
    /* Source kind2 frame5: bank0, offset1024, ox0/oy5. Anchor
       (4,149) places each opaque exported pixel at (4+x,144+y).
       Empty text isolates the icon from glyph overlap. */
    memset(output,0,sizeof output);
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,NULL,"",31));
    unsigned button_ink=0;
    for(unsigned y=0;y<16;y++)for(unsigned x=0;x<16;x++){
        const uint8_t *pixel=icons+1024+(y*16+x)*4;
        if(pixel[3]){assert(!memcmp(output+((144+y)*240+4+x)*4,pixel,4));button_ink++;}
    }
    assert(button_ink);
    /* Isolate original OBJ pixels on a black, ineligible backdrop to
       independently check movement/clipping without translating the BG. */
    memset(front,0,sizeof front);memset(back,0,sizeof back);uint16_t saved=palette[0];palette[0]=0;
    assert(!taxi_normal_menu_background(expected,front,back,palette,obj,strips,5,0,31));
    assert(!taxi_normal_menu_background_motion(output,front,back,palette,obj,strips,5,0,31,-8*4096));
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        int shift=y>=42&&y<122?(((y-42)/16)%2?8:-8):0;
        int source=(int)x-shift;uint8_t black[4]={0,0,0,255};
        const uint8_t *p=source>=0&&source<240?expected+(y*240+source)*4:black;
        assert(!memcmp(output+(y*240+x)*4,p,4));
    }
    memset(expected,0,sizeof expected);memset(output,0,sizeof output);
    assert(!taxi_normal_menu_footer(expected,fonts+1,front,back,palette,obj,strips,icons,"-Confirm ","-Back ",31));
    memset(output,0,sizeof output);memset(text_layer,0,sizeof text_layer);
    assert(!taxi_normal_menu_footer_layers_motion(output,text_layer,fonts+1,front,back,palette,obj,strips,icons,"-Confirm ","-Back ",31,0));
    for(unsigned pixel=0;pixel<240*160;pixel++)if(text_layer[pixel*4+3])memcpy(output+pixel*4,text_layer+pixel*4,4);
    assert(!memcmp(output,expected,sizeof output));
    memset(output,0,sizeof output);memset(text_layer,0,sizeof text_layer);
    assert(!taxi_normal_menu_footer_motion(output,fonts+1,front,back,palette,obj,strips,icons,"-Confirm ","-Back ",31,8*4096));
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
        uint8_t zero[4]={0};const uint8_t *p=x>=8?expected+(y*240+x-8)*4:zero;
        assert(!memcmp(output+(y*240+x)*4,p,4));
    }
    palette[0]=saved;
    read_file("outputs/crazy-taxi-native/assets/profile/background.indices",front,sizeof front);
    read_file("outputs/crazy-taxi-native/assets/profile-layer1/background.indices",back,sizeof back);
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,5,0,31));
    assert(!taxi_normal_menu_text(output,fonts,"Mode Selection",labels,5,31));
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,"-Confirm ",NULL,31));
    bitmap("outputs/crazy-taxi-native/reports/normal-main-text-native.bmp");
    const char *modes[]={"Normal Rules","3 Minutes","5 Minutes","10 Minutes"};
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,4,2,31));
    assert(!taxi_normal_menu_text(output,fonts,"Crazy Taxi",modes,4,31));
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,"-Confirm ","-Back ",31));
    bitmap("outputs/crazy-taxi-native/reports/normal-mode-text-native.bmp");
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,4,2,31));
    assert(!taxi_normal_menu_text(output,fonts,"Sound",audio_labels,4,31));
    assert(!taxi_audio_menu_test_text(output,fonts+1,1,0,31));
    for(unsigned row=0;row<2;row++){
        TaxiAudioVolumeSprite requests[2];
        assert(!taxi_audio_volume_render(240,54+(int)row*16,row?64:32,requests));
        assert(!taxi_audio_volume_compose(output,volume_indices,volume_palette,requests));
    }
    assert(!taxi_audio_test_render(240,86,2,2,1,32,&test_request));
    assert(!taxi_audio_menu_test_arrows(output,fonts+1,&test_request));
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,"-Play ","-Back ",31));
    bitmap("outputs/crazy-taxi-native/reports/audio-menu-footer.bmp");
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,4,0,31));
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,NULL,"-Back ",31));
    bitmap("outputs/crazy-taxi-native/reports/audio-menu-back-only.bmp");
    const char *options_labels[]={"Time","Time Diff","Traffic","Sound","Control"};
    TaxiGameSettings settings={{0,32,64,0x44,0,50,0,0}};
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,5,0,31));
    assert(!taxi_normal_menu_text(output,fonts,"Options",options_labels,5,31));
    const char *pc_options_labels[]={"Time","Time Diff","Traffic","Sound","Control","PC Display"};
    assert(!taxi_normal_menu_background(output,front,back,palette,obj,strips,6,5,31));
    assert(!taxi_normal_menu_text(output,fonts,"Options",pc_options_labels,6,31));
    unsigned pc_menu_ink=0;for(unsigned i=0;i<38400;i++)pc_menu_ink+=output[i*4+3]!=0;assert(pc_menu_ink>0);
    for(unsigned row=0;row<3;row++){
        TaxiOptionsSettingsRender request;
        assert(!taxi_options_settings_render(240,46+(int)row*16,(uint8_t)row,0,&settings,32,&request));
        if(row==0){
            assert(!taxi_menu_number_text(output,fonts+1,&request.time,31));
            assert(!taxi_audio_menu_test_arrows(output,fonts+1,&request.time));
            memcpy(expected,output,sizeof output);
            request.time.number[0]='x';
            assert(taxi_menu_number_text(output,fonts+1,&request.time,31)==-1);
            assert(!memcmp(expected,output,sizeof output));
        }else assert(!taxi_audio_volume_compose(output,volume_indices,volume_palette,request.bar));
    }
    assert(!taxi_normal_menu_footer(output,fonts+1,front,back,palette,obj,strips,icons,NULL,"-Back ",31));
    bitmap("outputs/crazy-taxi-native/reports/options-menu-native.bmp");
    const char *control_labels[16]={"Steer","Left","Steer","Right","Accelerate"," ","Brake"," ","Drive","Gear","Reverse","Gear","Toggle","Distance","Config.","Control"};
    for(unsigned layout=0;layout<3;layout++)for(unsigned page=0;page<7;page++){
        memset(output,0,153600);
        assert(!taxi_control_menu_text(output,fonts,control_labels,page,layout,31));
        unsigned signal=0;for(unsigned p=0;p<153600;p++)signal|=output[p];assert(signal);
    }
    memcpy(expected,output,153600);
    assert(taxi_control_menu_text(output,fonts,control_labels,7,0,31)==-1);
    assert(!memcmp(expected,output,153600));
    assert(taxi_control_menu_text(output,fonts,control_labels,0,3,31)==-1);
    assert(!memcmp(expected,output,153600));
    for(unsigned i=0;i<2;i++)taxi_font_asset_free(fonts+i);
    puts("PASS native menu BG/strip pixels: nine source references; text assets/caps/invalid-label rollback and native footer snapshots.");
    return 0;
}
