#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "normal_menu_text.h"
static uint8_t output[153600],front[131072],back[131072];
static uint16_t bgpal[256];
static const struct {unsigned mode;const char *title;const char *lines[4];} cases[]={
{0,"Crazy Jump",{"Jump past the K point."," "," "," "}},
{0,"Crazy Jump",{"For Quick acceleration, nothing","is better than the CRAZY DASH!!"," "," "}},
{0,"Crazy Jump",{"To do a CRAZY DASH, let off","the accelerator, shift into D,","then press the accelerator."," "}},
{1,"Crazy Flag",{"Run over the flag behind the","car within the time limit."," "," "}},
{1,"Crazy Flag",{"The CRAZY DRIFT is useful for","making sudden turns after","starting."," "}},
{1,"Crazy Flag",{"To execute a CRAZY DRIFT, shift","into R gear, then shift into D","gear."," "}},
{2,"Crazy Balloons",{"Pop all of the balloons","within the time limit."," "," "}},
{2,"Crazy Balloons",{"The CRAZY DRIFT is great to use","for crash turns."," "," "}},
{2,"Crazy Balloons",{"For a really cool ride, execute","a CRAZY DASH right after a","CRAZY DRIFT!"," "}},
{3,"Crazy Drift",{"Perform Crazy Drifts to earn a","minimum of 15 COMBO's within","the time limit."," "}},
{3,"Crazy Drift",{"Do the CRAZY DRIFT to earn","combos!!"," "," "}},
{3,"Crazy Drift",{"To execute a CRAZY DRIFT, shift","into R gear then shift into D","gear."," "}},
{4,"Crazy Turn",{"Take the customer to their","destination within the time","limit."," "}},
{4,"Crazy Turn",{"The CRAZY DRIFT is indispensable","when making U-turns."," "," "}},
{4,"Crazy Turn",{"Execute a CRAZY DASH right after","cornering to gain speed."," "," "}},
{5,"Crazy Bound",{"Without falling into the sea,","deliver the customer within","the time limit."," "}},
{5,"Crazy Bound",{"Fly cool with the CRAZY DRIFT","JUMP!"," "," "}},
{5,"Crazy Bound",{"Do a CRAZY DRIFT to perform a","CRAZY DRIFT JUMP!"," "," "}},
{6,"Crazy Rush",{"Take all of the customers to","their destinations within the","time limit."," "}},
{6,"Crazy Rush",{"Execute the CRAZY DRIFT STOP so","that the car is facing towards","the next destination."," "}},
{6,"Crazy Rush",{"A CRAZY DRIFT STOP is made by","stopping while doing a CRAZY","DRIFT!"," "}},
{7,"Crazy ZigZag",{"Without falling into the sea,","deliver the customer within","the time limit."," "}},
{7,"Crazy ZigZag",{"The CRAZY DRIFT is a must when","cornering."," "," "}},
{7,"Crazy ZigZag",{"You'll never make it if you don't","do a CRAZY DASH after cornering!"," "," "}},
{8,"Crazy Pole",{"Take all of the customers to","their destinations within the","time limit."," "}},
{8,"Crazy Pole",{"Crash into a pole while braking","to execute a crashing stop!"," "," "}},
{8,"Crazy Pole",{"Use the CRAZY DRIFT when stopping","to make it easier to continue on!"," "," "}}};
int main(void){TaxiFontAsset fonts[2]={{0}};
for(unsigned i=0;i<2;i++){char m[256],r[256];snprintf(m,sizeof m,"outputs/crazy-taxi-native/assets/normal-menu/font%u-metrics.ctf",i);snprintf(r,sizeof r,"outputs/crazy-taxi-native/assets/normal-menu/font%u.rgba",i);assert(!taxi_font_asset_load(fonts+i,m,r,i?3:2,32,1));}
FILE *capture=fopen("outputs/crazy-taxi-native/reports/challenge-briefing-pages.rgba","wb");assert(capture);
for(unsigned c=0;c<27;c++){memset(output,0,sizeof output);assert(!taxi_challenge_briefing_text(output,fonts,cases[c].title,cases[c].mode," ",cases[c].lines,31));unsigned colored=0;for(unsigned i=0;i<38400;i++)colored+=output[i*4+3]!=0;assert(colored);assert(fwrite(output,1,sizeof output,capture)==sizeof output);}
assert(!fclose(capture));
memset(output,123,sizeof output);assert(taxi_challenge_briefing_text(output,fonts,cases[0].title,9," ",cases[0].lines,31)==-1);for(unsigned i=0;i<sizeof output;i++)assert(output[i]==123);
memset(output,0,sizeof output);
assert(!taxi_challenge_grid_text(output,fonts,"Crazy Box","Name","Cabbie",0,31));
FILE *grid=fopen("outputs/crazy-taxi-native/reports/challenge-grid-text.rgba","wb");assert(grid);assert(fwrite(output,1,sizeof output,grid)==sizeof output);assert(!fclose(grid));
unsigned gridpixels=0;for(unsigned y=70;y<125;y++)for(unsigned x=0;x<110;x++)gridpixels+=output[(y*240+x)*4+3]!=0;assert(gridpixels);
memset(output,0,sizeof output);assert(!taxi_challenge_grid_text(output,fonts,"Crazy Box","Name","Cabbie",511,31));
for(unsigned y=70;y<125;y++)for(unsigned x=0;x<110;x++)assert(!output[(y*240+x)*4+3]);
memset(output,123,sizeof output);assert(taxi_challenge_grid_text(output,fonts,"Crazy Box","Name","Cabbie",512,31)==-1);
for(unsigned i=0;i<sizeof output;i++)assert(output[i]==123);
const char *bgpaths[]={"outputs/crazy-taxi-native/assets/profile/background.indices","outputs/crazy-taxi-native/assets/profile-layer1/background.indices","outputs/crazy-taxi-native/assets/normal-menu/background-palette.rgb555"};
void *buffers[]={front,back,bgpal};size_t sizes[]={sizeof front,sizeof back,sizeof bgpal};
for(unsigned f=0;f<3;f++){FILE *in=fopen(bgpaths[f],"rb");assert(in);assert(fread(buffers[f],1,sizes[f],in)==sizes[f]);assert(fgetc(in)==EOF);assert(!fclose(in));}
assert(!taxi_menu_background_only(output,front,back,bgpal,31));
assert(!taxi_challenge_briefing_text(output,fonts,cases[0].title,0," ",cases[0].lines,31));
uint16_t objpal[16];uint8_t icons[2048],stripdata[6][4096];TaxiMenuStrip strips[6];
FILE *asset=fopen("outputs/crazy-taxi-native/assets/normal-menu/strip-palette.rgb555","rb");assert(asset);assert(fread(objpal,1,sizeof objpal,asset)==sizeof objpal);assert(!fclose(asset));
asset=fopen("outputs/crazy-taxi-native/assets/normal-menu/footer-icons.rgba","rb");assert(asset);assert(fread(icons,1,sizeof icons,asset)==sizeof icons);assert(!fclose(asset));
asset=fopen("outputs/crazy-taxi-native/assets/normal-menu/strips.nms","rb");assert(asset);char magic[4];assert(fread(magic,1,4,asset)==4&&!memcmp(magic,"NMS1",4));
for(unsigned f=0;f<6;f++){uint16_t m[4];assert(fread(m,2,4,asset)==4);assert(m[0]*m[1]<=4096);assert(fread(stripdata[f],1,m[0]*m[1],asset)==m[0]*m[1]);strips[f]=(TaxiMenuStrip){m[0],m[1],(int16_t)m[2],(int16_t)m[3],stripdata[f]};}assert(!fclose(asset));
/* Briefingcallback1AB75 uses same5middle strip layout as ordinary menus. */
assert(!taxi_normal_menu_footer(output,fonts+1,front,back,bgpal,objpal,strips,icons,"-Advice ","-Back ",31));
FILE *composite=fopen("outputs/crazy-taxi-native/reports/challenge-briefing-background.rgba","wb");assert(composite);assert(fwrite(output,1,sizeof output,composite)==sizeof output);assert(!fclose(composite));
assert(!taxi_menu_background_only(output,front,back,bgpal,0));for(unsigned i=0;i<38400;i++)assert(!output[i*4]&&!output[i*4+1]&&!output[i*4+2]&&output[i*4+3]==255);
TaxiChallengeScreenAssets screen={fonts,front,back,icons,bgpal,objpal,strips};
FILE *screens=fopen("outputs/crazy-taxi-native/reports/challenge-composed-briefings.rgba","wb");assert(screens);
for(unsigned c=0;c<27;c++){
    assert(!taxi_challenge_briefing_screen(output,&screen,cases[c].title,cases[c].mode," ",cases[c].lines,c%3?"-Rules ":"-Advice ","-Back ",31));
    for(unsigned i=0;i<38400;i++)assert(output[i*4+3]==255);
    assert(fwrite(output,1,sizeof output,screens)==sizeof output);
}
assert(!fclose(screens));
memset(output,123,sizeof output);
assert(taxi_challenge_briefing_screen(output,&screen,cases[0].title,9," ",cases[0].lines,"-Advice ","-Back ",31)==-1);
for(unsigned i=0;i<sizeof output;i++)assert(output[i]==123);
uint8_t grid_sprites[12288];asset=fopen("outputs/crazy-taxi-native/assets/challenge-menu/grid-sprites.rgba","rb");assert(asset);assert(fread(grid_sprites,1,sizeof grid_sprites,asset)==sizeof grid_sprites);assert(fgetc(asset)==EOF);assert(!fclose(asset));
for(unsigned selected=0;selected<11;selected++){
 assert(!taxi_menu_background_only(output,front,back,bgpal,31));
 assert(!taxi_challenge_profile_panels(output,front,back,bgpal,objpal,strips,selected,31));
 assert(!taxi_challenge_grid_buttons(output,front,back,bgpal,grid_sprites,selected,0,31));
 assert(!taxi_challenge_grid_text(output,fonts,"Crazy Box","Name","Cabbie",0,31));
}
assert(!taxi_menu_background_only(output,front,back,bgpal,31));
assert(!taxi_challenge_profile_panels(output,front,back,bgpal,objpal,strips,2,31));
assert(!taxi_challenge_grid_buttons(output,front,back,bgpal,grid_sprites,2,1,31));
assert(!taxi_challenge_grid_text(output,fonts,"Crazy Box","Name","Cabbie",1,31));
FILE *grid_composite=fopen("outputs/crazy-taxi-native/reports/challenge-grid-buttons.rgba","wb");assert(grid_composite);assert(fwrite(output,1,sizeof output,grid_composite)==sizeof output);assert(!fclose(grid_composite));
memset(output,123,sizeof output);assert(taxi_challenge_grid_buttons(output,front,back,bgpal,grid_sprites,11,0,31)==-1);
for(unsigned i=0;i<sizeof output;i++)assert(output[i]==123);
/* Actual gameplay group0: same08070E50 container used by challenge startup. */
TaxiFontAsset gameplay={0};
assert(!taxi_font_asset_load(&gameplay,"outputs/crazy-taxi-native/assets/loading/font0-metrics.ctf","outputs/crazy-taxi-native/assets/loading/font0.rgba",4,32,1));
const char *failure_labels[]={"Failed!","Try Again?","Yes","No"};
uint8_t other[153600];
memset(output,0,sizeof output);assert(!taxi_challenge_failure_text(output,&gameplay,failure_labels,0,31));
memset(other,0,sizeof other);assert(!taxi_challenge_failure_text(other,&gameplay,failure_labels,1,31));
unsigned changed=0,yes=0,no=0;
for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
 unsigned off=(y*240+x)*4;
 if(memcmp(output+off,other+off,4)){assert(y>=50&&y<85);changed++;if(y<75)yes++;else no++;}
}
assert(changed&&yes&&no);
memset(other,123,sizeof other);assert(taxi_challenge_failure_text(other,&gameplay,failure_labels,2,31)==-1);
for(unsigned i=0;i<sizeof other;i++)assert(other[i]==123);
assert(taxi_challenge_failure_text(other,fonts,failure_labels,0,31)==-1);
for(unsigned i=0;i<sizeof other;i++)assert(other[i]==123);
memset(other,0,sizeof other);assert(!taxi_challenge_failure_text(other,&gameplay,failure_labels,0,0));
for(unsigned i=0;i<38400;i++)assert(!other[i*4]&&!other[i*4+1]&&!other[i*4+2]);
const char *success_labels[]={"Cleared!","Your Name","Result","Ranking","Record","Name"};
const char *records[]={"173.81m","150.00m","---"},*names[]={"AAA","BBB","---"};
memset(output,0,sizeof output);assert(!taxi_challenge_success_text(output,&gameplay,success_labels,"AAA","173.81m",records,names,31));
unsigned ink=0;for(unsigned y=85;y<135;y++)for(unsigned x=0;x<240;x++)ink+=output[(y*240+x)*4+3]!=0;assert(ink);
memset(other,123,sizeof other);assert(taxi_challenge_success_text(other,fonts,success_labels,"AAA","173.81m",records,names,31)==-1);
for(unsigned i=0;i<sizeof other;i++)assert(other[i]==123);
taxi_font_asset_free(&gameplay);
taxi_font_asset_free(fonts);taxi_font_asset_free(fonts+1);puts("PASS27 composed original-asset briefing pages, grid text and atomic error checks; liveUI remains excluded.");return 0;}

