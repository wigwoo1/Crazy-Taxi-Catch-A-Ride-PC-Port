#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "startup_intro_viewer.h"
#include "startup_intro_pixels.h"
#include "startup_intro_fade.h"
#include "xbox.h"
#include "startup_screens.h"
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
  if(read_asset("Crazy Taxi - Catch a Ride (USA).gba",rom,sizeof rom))return 12;
  TaxiScreenDescriptor d=descriptor(rom,0);
  if(d.image<0x08000000||d.image>=0x08800000||taxi_startup_screens_load(&screens,&d,rom+d.image-0x08000000,sizeof rom-(d.image-0x08000000)))return 13;
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
 unsigned maximum=full_startup?1559:584;if(!limit||limit>maximum)limit=maximum;
 while(frames<limit&&!((!full_startup||screen>=3)&&s.fades&&!fade.state.fade.active_177b)&&!quit){MSG message;while(PeekMessageA(&message,NULL,0,0,PM_REMOVE)){if(message.message==WM_QUIT)quit=1;TranslateMessage(&message);DispatchMessageA(&message);}if(quit)break;
  TaxiXboxSample pad=taxi_xbox_poll_any();uint8_t confirm=(GetAsyncKeyState(VK_RETURN)&0x8000)!=0,start_key=(GetAsyncKeyState(VK_SPACE)&0x8000)!=0;
  if(pad.connected){confirm|=(pad.active_low_keys&1)==0;start_key|=(pad.active_low_keys&8)==0;}
  int bitmap_done=0;
  if(full_startup&&screen<3){
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
  frames++;QueryPerformanceCounter(&now);double target=(double)frames/60.0,elapsed=(double)(now.QuadPart-start.QuadPart)/frequency.QuadPart;if(target>elapsed)Sleep((DWORD)((target-elapsed)*1000));
 }
 unsigned final_nonblack=0;for(unsigned i=0;i<240*160;i++)if(pixels[i])final_nonblack++;
 SelectObject(surface,old_bitmap);DeleteObject(bitmap);DeleteDC(surface);ReleaseDC(window,dc);if(IsWindow(window))DestroyWindow(window);
 if(full_startup)printf("NATIVE STARTUP PREFIX: bitmap_frames=%u bitmap_completed=%u waits=%u inputs=%u objects=%u flushes=%u stops=%u title_handoff=unbound\n",bitmap_frames,bitmap_completed,backend.waits,backend.inputs,backend.objects,backend.flushes,backend.stops);
 printf("NATIVE INTRO PREVIEW: frames=%u fade_requests=%u accepted=%u clipped=%u center_readbacks=%u surface_readbacks=%u final_nonblack=%u result=%d\n",frames,s.fades,s.accepted,s.clipped,readbacks,surface_readbacks,final_nonblack,result);return result;
}
int taxi_startup_intro_viewer(unsigned limit){return viewer(limit,0);}
int taxi_startup_screens_viewer(unsigned limit){return viewer(limit,1);}
#ifdef TAXI_INTRO_STANDALONE
#include <stdlib.h>
int main(int argc,char **argv){if(argc>1&&!strcmp(argv[1],"--startup"))return taxi_startup_screens_viewer(argc>2?(unsigned)strtoul(argv[2],NULL,10):1559);return taxi_startup_intro_viewer(argc>1?(unsigned)strtoul(argv[1],NULL,10):584);}
#endif
