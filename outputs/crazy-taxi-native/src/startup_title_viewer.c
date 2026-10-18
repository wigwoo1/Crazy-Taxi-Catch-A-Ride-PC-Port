#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "startup_intro_viewer.h"
#include "startup_intro_pixels.h"
#include "startup_intro_fade.h"
#include "xbox.h"
#include "startup_screens.h"
#include "title_entry.h"
#include "title_requests.h"
#include "title_publish.h"
#include "title_pixels.h"
#include "taxi_graphics_package.h"
#include "startup_init_owned.h"
#include "startup_save_preview.h"
#include "title_parent.h"
#include "title_attract_owned.h"
#include "attract_entry.h"
static const char *preview_save_path=NULL;
#include <stdlib.h>
static LRESULT CALLBACK window_proc(HWND h,UINT m,WPARAM w,LPARAM l){if(m==WM_CLOSE){DestroyWindow(h);return 0;}if(m==WM_DESTROY){PostQuitMessage(0);return 0;}return DefWindowProcA(h,m,w,l);}
static int read_asset(const char *name,void *out,size_t n){FILE *f=fopen(name,"rb");if(!f)return -1;size_t got=fread(out,1,n,f);fclose(f);return got==n?0:-1;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|p[1]<<8);}
static uint32_t word(const uint8_t *p){return (uint32_t)half(p)|(uint32_t)half(p+2)<<16;}
static TaxiScreenDescriptor descriptor(const uint8_t *rom,unsigned state){
 const uint8_t *p=rom+0x49d560+state*28;TaxiScreenDescriptor d={0};
 d.image=word(p);d.timer=word(p+4);d.fade_in_bg=half(p+8);d.fade_in_obj=half(p+10);d.fade_out_bg=half(p+12);d.fade_out_obj=half(p+14);d.background=word(p+16);d.callback=word(p+20);d.type=(int8_t)p[24];d.flags=p[25];return d;
}
typedef struct {TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;unsigned waits,inputs,objects,flushes,stops;} BitmapBackend;
static int bitmap_platform(void *v,TaxiStartupScreens *s,uint32_t target,uint32_t arg){
 BitmapBackend *b=v;
 if(target==0x08016208){if(arg!=1)return -1;s->presentation.current_3520++;b->waits++;return 0;}
 if(target==0x08015c3c){b->inputs++;return 0;}
 if(target==0x08027cd0){b->objects++;return taxi_startup_obj_cold_clear(&b->queue,&b->transport);}
 if(target==0x08028ee0){b->flushes++;return 0;}
 if(target==0x0802763c){b->stops++;return 0;}
 return -1;
}
static int viewer(unsigned limit,int full_startup){
 static uint8_t rom[8388608];static TaxiStartupScreens screens;BitmapBackend backend={0};unsigned screen=0,bitmap_frames=0,bitmap_completed=0;
 if(full_startup){
  memset(&screens,0,sizeof screens);screens.presentation.delta_3b00=1;
  TaxiGraphicsPackage package={0};char path[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,path,MAX_PATH);
  if(!n||n>=MAX_PATH)return 12;char *tail=strrchr(path,92);if(!tail)return 12;
  snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/title/native-startup-title.ctg");
  if(taxi_graphics_package_load(&package,path))return 12;
  if(package.size!=sizeof rom){taxi_graphics_package_release(&package);return 12;}
  memcpy(rom,package.bytes,sizeof rom);taxi_graphics_package_release(&package);
  TaxiScreenDescriptor d=descriptor(rom,0);
  if(d.image<0x08000000||d.image>=0x08800000||taxi_startup_screens_load(&screens,&d,rom+d.image-0x08000000,sizeof rom-(d.image-0x08000000)))return 13;
 }
 TaxiStartupInit startup_init={0};TaxiStartupInitContinuation startup_continuation={0};TaxiInput startup_input={0};
 static uint8_t startup_settings[512],startup_rows[800];uint8_t startup_music_volume=0,startup_effects_volume=0,startup_language=0;int startup_pending=0;
 if(full_startup>=3){
  TaxiStartupInitOwners owners={startup_settings,startup_rows,rom+0x4a54c4,&startup_input,&startup_music_volume,&startup_effects_volume,&startup_language};
  startup_pending=taxi_startup_init_owned_begin(&startup_continuation,&startup_init,&owners);
  if(startup_pending!=TAXI_STARTUP_INIT_SCREEN)return 23;
  printf("NATIVE STARTUP INIT: pending=screen0 font5=%08lx volumes=%u/%u language=%u\n",(unsigned long)startup_init.font_assets[5],startup_music_volume,startup_effects_volume,startup_language);
 }
 TaxiStartupIntro s={0};TaxiStartupIntroFade fade={0};static uint8_t bg0[512*256],bg1[256*256],source_bg1[240*160],tiles[228*64],rgba[240*160*4];static uint16_t palette[256],pixels[240*160];unsigned char metrics[602];
 if(read_asset("outputs/crazy-taxi-native/assets/startup-animation/sprites.crs",metrics,sizeof metrics)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/sprites.palette.rgb555",s.palette,sizeof s.palette)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-0.indices",bg0,sizeof bg0)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-1.indices",source_bg1,sizeof source_bg1)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-0.palette.rgb555",palette,sizeof palette)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/sprites.tiles",tiles,sizeof tiles))return 2;
 for(unsigned i=0;i<33;i++)for(unsigned j=0;j<9;j++){unsigned n=8+i*18+j*2;s.metrics[i][j]=(uint16_t)(metrics[n]|metrics[n+1]<<8);}
 memset(bg1,162,sizeof bg1);for(unsigned y=0;y<160;y++)memcpy(bg1+y*256,source_bg1+y*240,240);
 s.width=240;s.height=160;s.blend=0x1f40;s.alpha=0x0808;s.animation.phase_3574=255;s.animation.remaining_359c=18000;
 if(taxi_startup_obj_setup(&s.queue,&s.transport,s.base_template,s.matrix_template,1)||taxi_startup_intro_fade_begin(&s,&fade,1))return 3;
 HINSTANCE instance=GetModuleHandleA(NULL);WNDCLASSA cls={0};cls.lpfnWndProc=window_proc;cls.hInstance=instance;cls.lpszClassName="TaxiNativeIntroPreview";cls.hCursor=LoadCursor(NULL,IDC_ARROW);
 if(!RegisterClassA(&cls)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return 4;
 HWND window=CreateWindowA(cls.lpszClassName,full_startup?"Crazy Taxi - Native Startup Preview":"Crazy Taxi - Native Intro Preview",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1280,760,NULL,NULL,instance,NULL);if(!window)return 5;
 ShowWindow(window,SW_SHOW);HDC dc=GetDC(window);if(!dc){DestroyWindow(window);return 6;}
 BITMAPINFO info={0};info.bmiHeader.biSize=sizeof info.bmiHeader;info.bmiHeader.biWidth=240;info.bmiHeader.biHeight=-160;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
 HDC surface=CreateCompatibleDC(NULL);void *surface_pixels=NULL;HBITMAP bitmap=CreateDIBSection(NULL,&info,DIB_RGB_COLORS,&surface_pixels,NULL,0);if(!surface||!bitmap){if(bitmap)DeleteObject(bitmap);if(surface)DeleteDC(surface);ReleaseDC(window,dc);DestroyWindow(window);return 9;}HGDIOBJ old_bitmap=SelectObject(surface,bitmap);
 LARGE_INTEGER frequency,start,now;QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&start);unsigned frames=0,readbacks=0,surface_readbacks=0;int result=0,quit=0;
 static TaxiTitleLoad title;TaxiTitleRuntime title_runtime={0};int title_active=0;unsigned title_frames=0;uint32_t title_random=0;TaxiTitleParent title_parent={0};TaxiTitleParentContinuation title_continuation={0};
 unsigned maximum=(full_startup==2||full_startup==5)?2600:full_startup?1559:584;if(!limit||limit>maximum)limit=maximum;
 while(frames<limit&&((full_startup==2||full_startup==5)?!title_runtime.finished:!((!full_startup||screen>=3)&&s.fades&&!fade.state.fade.active_177b))&&!quit){MSG message;while(PeekMessageA(&message,NULL,0,0,PM_REMOVE)){if(message.message==WM_QUIT)quit=1;TranslateMessage(&message);DispatchMessageA(&message);}if(quit)break;
  TaxiXboxSample pad=taxi_xbox_poll_any();uint8_t confirm=(GetAsyncKeyState(VK_RETURN)&0x8000)!=0,start_key=(GetAsyncKeyState(VK_SPACE)&0x8000)!=0;
  if(pad.connected){confirm|=(pad.active_low_keys&1)==0;start_key|=(pad.active_low_keys&8)==0;}
  if(full_startup>=3){
   uint16_t keys=pad.connected?pad.active_low_keys:1023;
   if(confirm)keys&=(uint16_t)~1u;if(start_key)keys&=(uint16_t)~8u;
   /* Host sample boundary remains diagnostic; original presentation ordering pending. */
   taxi_input_update(&startup_input,(TaxiInputSample){keys,0,0,0,1});
  }
  int bitmap_done=0;
  if(title_active){
   TaxiTitleRuntimeOutput output;
   if(taxi_title_runtime_frame(&title_runtime,&title_random,(uint16_t)((confirm?1:0)|(start_key?8:0)),0,&output)||taxi_title_requests_queue(&title,&output.callback,rom,sizeof rom,title_runtime.obj_palette_mask)||taxi_title_objects_publish(&title)||taxi_title_pixels(&title,(unsigned)title_runtime.callback.scroll.x_q8/256,title_runtime.bldy,pixels)){result=20;break;}
   title_frames++;
  }else if(full_startup&&screen<3){
   bitmap_done=taxi_startup_screens_frame(&screens,confirm,start_key,bitmap_platform,&backend);
   if(bitmap_done<0){result=14;break;}
   const uint8_t *indices=screens.bitmap[(screens.presentation.display_control&16)?1:0];
   for(unsigned i=0;i<240*160;i++)pixels[i]=taxi_palette_fade_darken_rgb555(screens.banks.background[indices[i]],screens.fade.bldy);
   bitmap_frames++;
  }else if(taxi_startup_intro_fade_frame(&s,&fade,1,confirm,start_key)||taxi_startup_intro_pixels(&s,bg0,bg1,palette,tiles,sizeof tiles,pixels)){result=7;break;}
  for(unsigned i=0;i<240*160;i++){uint16_t v=pixels[i];for(unsigned j=0;j<3;j++){unsigned shift=(2-j)*5;unsigned c=(v>>shift)&31;rgba[i*4+j]=(uint8_t)((c<<3)|(c>>2));}rgba[i*4+3]=255;}
  if(SetDIBitsToDevice(surface,0,0,240,160,0,0,0,160,rgba,&info,DIB_RGB_COLORS)!=160){result=10;break;}
  unsigned center=(80*240+120)*4;COLORREF expected_surface=RGB(rgba[center+2],rgba[center+1],rgba[center]);GdiFlush();if(GetPixel(surface,120,80)!=expected_surface){result=11;break;}surface_readbacks++;
  RECT rect;GetClientRect(window,&rect);int w=rect.right,h=rect.bottom,dw=w,dh=w*2/3;if(dh>h){dh=h;dw=h*3/2;}int left=(w-dw)/2,top=(h-dh)/2;PatBlt(dc,0,0,w,h,BLACKNESS);SetStretchBltMode(dc,COLORONCOLOR);
  if(!StretchBlt(dc,left,top,dw,dh,surface,0,0,240,160,SRCCOPY)){result=8;break;}
  if(dw>0&&dh>0){COLORREF c=GetPixel(dc,left+dw/2,top+dh/2);unsigned p=(80*240+120)*4;COLORREF expected=RGB(rgba[p+2],rgba[p+1],rgba[p]);if(c==expected)readbacks++;if(frames==0||frames==299)printf("DISPLAY SAMPLE: frame%u client%dx%d color%08lx expected%08lx\n",frames+1,w,h,(unsigned long)c,(unsigned long)expected);}
  if(bitmap_done){
   printf("BITMAP SCREEN COMPLETE: screen%u frame%u brightness%u\n",screen,frames+1,screens.fade.bldy);bitmap_completed++;
   TaxiScreenDescriptor next=descriptor(rom,screen+1);
   if(taxi_startup_screens_advance(&screens,&next,bitmap_platform,&backend)!=1){result=15;break;}
   screen++;
   if(screen<3){
    if(next.image<0x08000000||next.image>=0x08800000||taxi_startup_screens_load(&screens,&next,rom+next.image-0x08000000,sizeof rom-(next.image-0x08000000))){result=16;break;}
   }else{
    if(next.type!=7||next.callback!=0x080167d1||next.background!=0x08082104){result=17;break;}
    fade.state=screens.fade;
    if(taxi_startup_obj_setup(&s.queue,&s.transport,s.base_template,s.matrix_template,1)||taxi_startup_intro_fade_begin(&s,&fade,1)){result=18;break;}
   }
  }
  if(full_startup==2&&!title_active&&screen>=3&&s.fades&&!fade.state.fade.active_177b){
   TaxiScreenDescriptor sentinel=descriptor(rom,4),d=descriptor(rom,5);
   if(sentinel.type!=-1||sentinel.image||d.type!=1||d.background!=0x08034634||d.callback!=0x080166b9){result=21;break;}
   memset(&title,0,sizeof title);title.fade=fade.state;memcpy(title.matrix_template,s.matrix_template,sizeof title.matrix_template);
   if(taxi_title_load(&title,rom,sizeof rom,0)||taxi_title_entry(&title_runtime,&title)){result=22;break;}
   title_active=1;printf("NATIVE TITLE ENTRY: prefix_frame=%u parent_boundary=explicit_preview_fixture retained_interval=%u retained_reload=%u\n",frames+1,title_runtime.fade.interval_1771,title_runtime.fade.reload_1772);
  }
  if(full_startup>=3&&startup_pending==TAXI_STARTUP_INIT_SCREEN&&screen>=3&&s.fades&&!fade.state.fade.active_177b){
   uint16_t pressed_mask=startup_input.pressed_mask;uint8_t pressed[10];memcpy(pressed,startup_input.pressed,10);
   startup_pending=taxi_startup_init_owned_resume(&startup_continuation);
   if(startup_pending!=TAXI_STARTUP_INIT_SAVE||startup_init.initialized_1ea4||startup_init.dispatcher_3878||startup_input.pressed_mask!=pressed_mask||memcmp(pressed,startup_input.pressed,10)){result=24;break;}
   for(unsigned button=0;button<10;button++)if(startup_input.held[button]||startup_input.previous[button]||startup_input.repeat[button]||startup_input.countdown[button])result=25;
   if(result)break;
   if(full_startup>=4){if(taxi_startup_save_preview(&startup_continuation,preview_save_path,frames+1)){result=27;break;}startup_pending=startup_continuation.pending;}
   printf("NATIVE STARTUP INIT HANDOFF: prefix_frame=%u pending=%d initialized=%u dispatcher=%08lx save_backend=%s\n",frames+1,startup_pending,startup_init.initialized_1ea4,(unsigned long)startup_init.dispatcher_3878,full_startup>=4?"native_read_only":"unbound");
  }
  if(full_startup==5&&startup_pending==TAXI_STARTUP_INIT_DONE&&!title_active){
   title_parent.callback_3878=startup_init.dispatcher_3878;
   if(startup_init.dispatcher_3878!=0x0801e275||taxi_title_parent_begin(&title_continuation,&title_parent)!=TAXI_TITLE_PARENT_TITLE){result=28;break;}
   TaxiScreenDescriptor d=descriptor(rom,5);if(d.type!=1||d.callback!=0x080166b9){result=29;break;}
   memset(&title,0,sizeof title);title.fade=fade.state;memcpy(title.matrix_template,s.matrix_template,sizeof title.matrix_template);
   if(taxi_title_load(&title,rom,sizeof rom,startup_language)||taxi_title_entry(&title_runtime,&title)){result=30;break;}
   title_active=1;printf("NATIVE TITLE PARENT ENTRY: prefix_frame=%u pending=title5 flags=%u flags_source=cold_boot_fixture\n",frames+1,title_parent.flags_3899);
  }
  if(full_startup==5&&title_active&&title_runtime.finished){
   /* Source screen-flow confirm clears3898; timeout retains it. */
   if(title_runtime.finished==1)title_parent.active_3898=0;
   int pending=taxi_title_parent_resume(&title_continuation);if(pending<0){result=31;break;}
   if(pending==TAXI_TITLE_PARENT_PREPARE_ATTRACT){
    TaxiAttractPrepare prepare={0};TaxiAttractEntry entry={0};
    /* Recorder starts cold in this diagnostic preview; shared RNG is retained. */
    memcpy(entry.settings,startup_settings,8);uint32_t before=title_random;
    if(taxi_title_attract_prepare_owned(&title_continuation,&prepare)!=TAXI_TITLE_PARENT_ATTRACT||taxi_attract_entry(&entry,prepare.recorder[0],&title_random)){result=32;break;}
    memcpy(startup_settings,entry.settings,8);pending=title_continuation.pending;
    printf("NATIVE ATTRACT ENTRY: pending=%d override=%u saved_city=%u city=%u settings0=%u random=%u->%u scene=%08lx next_boundary=AC62_unbound recorder_source=cold_preview_fixture\n",pending,prepare.recorder[0],entry.saved_city,entry.settings[4],entry.settings[0],before,title_random,(unsigned long)entry.scene_identity);
   }
   printf("NATIVE TITLE PARENT RETURN: outcome=%u pending=%d active=%u menu=%u callback=%08lx next_child=unbound\n",title_runtime.finished,pending,title_parent.active_3898,title_parent.menu_3897,(unsigned long)title_parent.callback_3878);
  }
  frames++;QueryPerformanceCounter(&now);double target=(double)frames/60.0,elapsed=(double)(now.QuadPart-start.QuadPart)/frequency.QuadPart;if(target>elapsed)Sleep((DWORD)((target-elapsed)*1000));
 }
 unsigned final_nonblack=0;for(unsigned i=0;i<240*160;i++)if(pixels[i])final_nonblack++;
 SelectObject(surface,old_bitmap);DeleteObject(bitmap);DeleteDC(surface);ReleaseDC(window,dc);if(IsWindow(window))DestroyWindow(window);
 if(full_startup>=3&&frames==1559&&startup_pending!=(full_startup>=4?TAXI_STARTUP_INIT_DONE:TAXI_STARTUP_INIT_SAVE))result=26;
 if(full_startup==2||full_startup==5)printf("NATIVE TITLE PREVIEW: title_frames=%u clock=%u parent=%u brightness=%u random=%u\n",title_frames,title_runtime.clock,title_runtime.finished,title_runtime.bldy,title_random);
 if(full_startup)printf("NATIVE STARTUP PREFIX: bitmap_frames=%u bitmap_completed=%u waits=%u inputs=%u objects=%u flushes=%u stops=%u title_handoff=%s\n",bitmap_frames,bitmap_completed,backend.waits,backend.inputs,backend.objects,backend.flushes,backend.stops,full_startup==5?"native_parent":"unbound");
 printf("NATIVE INTRO PREVIEW: frames=%u fade_requests=%u accepted=%u clipped=%u center_readbacks=%u surface_readbacks=%u final_nonblack=%u result=%d\n",frames,s.fades,s.accepted,s.clipped,readbacks,surface_readbacks,final_nonblack,result);return result;
}
int taxi_startup_title_viewer(unsigned limit){return viewer(limit,2);}
int taxi_startup_intro_viewer(unsigned limit){return viewer(limit,0);}
int taxi_startup_screens_viewer(unsigned limit){return viewer(limit,1);}
#ifdef TAXI_INTRO_STANDALONE
#include <stdlib.h>
int main(int argc,char **argv){if(argc>2&&!strcmp(argv[1],"--startup-parent-title")){preview_save_path=argv[2];return viewer(argc>3?(unsigned)strtoul(argv[3],NULL,10):2600,5);}if(argc>2&&!strcmp(argv[1],"--startup-init-save")){preview_save_path=argv[2];return viewer(argc>3?(unsigned)strtoul(argv[3],NULL,10):1559,4);}if(argc>1&&!strcmp(argv[1],"--startup-init"))return viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):1559,3);if(argc>1&&!strcmp(argv[1],"--startup-title"))return taxi_startup_title_viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):2600);if(argc>1&&!strcmp(argv[1],"--startup"))return taxi_startup_screens_viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):1559);return taxi_startup_intro_viewer(argc>1?(unsigned)strtoul(argv[1],NULL,10):584);}
#endif
