#include "challenge_assets.h"
#include <stdio.h>
#include <string.h>
static int path(char out[1024],const char *dir,unsigned mode,const char *suffix){
 int n=snprintf(out,1024,"%s/challenge%u%s",dir,mode,suffix);return n<0||n>=1024?-1:0;
}
static int read_exact(const char *p,void *out,size_t size){
 FILE *f=fopen(p,"rb");if(!f)return -1;
 int ok=fread(out,1,size,f)==size&&fgetc(f)==EOF;int closed=fclose(f);return ok&&!closed?0:-1;
}
void taxi_challenge_assets_free(TaxiChallengeAssets *a){if(!a)return;taxi_world_asset_free(&a->world);taxi_indexed_asset_free(&a->textures);memset(a,0,sizeof *a);}
int taxi_challenge_assets_load(TaxiChallengeAssets *out,const char *dir,unsigned mode){
 if(!out||!dir||mode>8)return -1;
 TaxiChallengeAssets a={0};char p[1024];uint8_t marker[16],palette[512];a.submode=mode;
 if(path(p,dir,mode,".ctw")||taxi_world_asset_load(&a.world,p))goto bad;
 if(path(p,dir,mode,"-special.cts")||taxi_world_asset_load_special(&a.world,p))goto bad;
 if(path(p,dir,mode,".cti")||taxi_indexed_asset_load(&a.textures,p))goto bad;
 if(path(p,dir,mode,"-spawn.bin")||read_exact(p,marker,16)||taxi_spawn_marker_decode(&a.spawn,marker,16))goto bad;
 if(path(p,dir,mode,"-material-prefix.bin")||read_exact(p,a.material_prefix,28))goto bad;
 if(path(p,dir,mode,"-palette.rgb555")||read_exact(p,palette,512))goto bad;
 for(unsigned i=0;i<256;i++)a.palette[i]=(uint16_t)(palette[i*2]|(unsigned)palette[i*2+1]<<8);
 taxi_challenge_assets_free(out);*out=a;return 0;
bad:taxi_challenge_assets_free(&a);return -1;
}
