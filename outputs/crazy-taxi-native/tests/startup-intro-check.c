#include "../src/startup_intro_pixels.h"
#include <stdio.h>
#undef NDEBUG
#include <assert.h>
static unsigned char bg0[512*256],bg1[256*256],tiles[228*64];static uint16_t bg_palette[256],pixels[240*160];
static int read_asset(const char *name,void *out,size_t n){FILE *f=fopen(name,"rb");if(!f)return -1;size_t got=fread(out,1,n,f);fclose(f);return got==n?0:-1;}
int main(void){TaxiStartupIntro s={0};unsigned char bytes[602];
 FILE *f=fopen("outputs/crazy-taxi-native/assets/startup-animation/sprites.crs","rb");assert(f);assert(fread(bytes,1,602,f)==602);fclose(f);
 for(unsigned i=0;i<33;i++)for(unsigned j=0;j<9;j++){unsigned n=8+i*18+j*2;s.metrics[i][j]=(uint16_t)(bytes[n]|bytes[n+1]<<8);}
 FILE *palette_file=fopen("outputs/crazy-taxi-native/assets/startup-animation/sprites.palette.rgb555","rb");if(!palette_file)return 5;if(fread(s.palette,1,sizeof s.palette,palette_file)!=sizeof s.palette){fclose(palette_file);return 6;}fclose(palette_file);
 if(read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-0.indices",bg0,sizeof bg0)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-0.palette.rgb555",bg_palette,sizeof bg_palette)||read_asset("outputs/crazy-taxi-native/assets/startup-animation/sprites.tiles",tiles,sizeof tiles))return 7;
 for(unsigned y=0;y<256;y++)for(unsigned x=0;x<256;x++)bg1[y*256+x]=162;
 unsigned char source_bg1[240*160];if(read_asset("outputs/crazy-taxi-native/assets/startup-animation/background-1.indices",source_bg1,sizeof source_bg1))return 8;for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)bg1[y*256+x]=source_bg1[y*240+x];
 s.blend=0x1f40;s.alpha=0x0808;s.width=240;s.height=160;s.animation.phase_3574=255;s.animation.remaining_359c=18000;
 int result=taxi_startup_obj_setup(&s.queue,&s.transport,s.base_template,s.matrix_template,1);if(result)return 2;
 unsigned peak=0,frames=0;
 for(;frames<519&&!s.fades;){result=taxi_startup_intro_step(&s,0,0,0,0,0x2001,0x2001);if(result){fprintf(stderr,"step failure %u %d\n",frames,result);return 3;}if(s.transport.previous_total>peak)peak=s.transport.previous_total;frames++;if(frames==60||frames==120||frames==200||frames==300){char name[160];snprintf(name,sizeof name,"outputs/crazy-taxi-native/reports/intro-native-frame-%u.bin",frames);FILE *capture=fopen(name,"wb");if(!capture)return 4;fwrite(s.transport.output,1,sizeof s.transport.output,capture);fwrite(s.palette,1,sizeof s.palette,capture);fclose(capture);result=taxi_startup_intro_pixels(&s,bg0,bg1,bg_palette,tiles,sizeof tiles,pixels);if(result){fprintf(stderr,"pixel failure %u %d\n",frames,result);return 9;}snprintf(name,sizeof name,"outputs/crazy-taxi-native/reports/intro-native-composite-%u.rgb555",frames);capture=fopen(name,"wb");if(!capture)return 10;fwrite(pixels,1,sizeof pixels,capture);fclose(capture);}assert(!s.queue.total&&!s.transport.matrix_count);assert(frames<=519);}
 assert(frames==519&&s.fades==1&&s.animation.remaining_359c==0&&peak<=128);
 assert(s.fade_request[0]==0&&s.fade_request[1]==32&&s.fade_request[2]==0x2001&&s.fade_request[3]==0x2001);
 printf("PASS native intro composition: %u frames, %u accepted, %u clipped, peak %u, one source fade request\n",frames,s.accepted,s.clipped,peak);return 0;}
