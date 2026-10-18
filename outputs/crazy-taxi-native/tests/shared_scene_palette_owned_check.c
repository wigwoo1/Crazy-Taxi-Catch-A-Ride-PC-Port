#ifdef NDEBUG
#undef NDEBUG
#endif
#include "ordinary_graphics_owned.h"
#include "taxi_graphics_package.h"
#include "city_palette_owned.h"
#include "hud_start_owned.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static unsigned half(const uint8_t *p){return p[0]|(unsigned)p[1]<<8;}
typedef struct {
 TaxiTitleAssets assets;TaxiPaletteRegistration registry;TaxiPaletteBanks banks;
 TaxiFontSetup fonts;TaxiStartupObjQueue queue;TaxiStartupObjTransport transport;
 uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];TaxiOrdinaryGraphicsOwners graphics;
 TaxiHudStartOwners hud;TaxiSpriteCopies copies;TaxiSpriteImmediateCopy immediate;
} Fixture;
static void init(Fixture *f,const TaxiGraphicsPackage *p,unsigned flags){
 memset(f,0,sizeof *f);memset(&f->fonts,0xa5,sizeof f->fonts);f->registry.flags=(uint8_t)flags;
 f->graphics=(TaxiOrdinaryGraphicsOwners){&f->assets,&f->registry,&f->banks,&f->fonts,&f->queue,&f->transport,f->base,f->matrices,p->bytes,p->size,0x06014000};
 assert(!taxi_ordinary_graphics_owned(&f->graphics,1)&&f->registry.count==16&&f->registry.obj_bytes==512&&!f->registry.bg_bytes);
 f->hud.graphics=&f->graphics;f->hud.copies=&f->copies;f->hud.immediate=&f->immediate;
 /* Retained native font alias bytes must survive appending the scene. */
 memcpy((uint8_t *)f->registry.records+0x680,&f->fonts,sizeof f->fonts);
 f->queue.count[1]=f->queue.total=1;f->queue.attributes[1][0][0]=77;
}
static void hud(Fixture *f,unsigned driver){
 TaxiHudStartInput in={0,0,0,(uint8_t)driver,(uint8_t)driver};
 assert(!taxi_hud_start_owned(&f->hud,&in)&&f->registry.count>=16&&!f->copies.count);
}
static void read_scene(unsigned index,TaxiSceneRegistration *scene,uint16_t colors[256]){
 const char *names[]={"city1","city2","startup"};char path[256];
 assert(snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/scene-descriptors/%s.bin",names[index])>0);
 assert(!taxi_scene_registration_load(scene,path));
 assert(snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/%s-palette.rgb555",names[index])>0);
 FILE *file=fopen(path,"rb");uint8_t bytes[512];assert(file&&fread(bytes,1,512,file)==512&&fgetc(file)==EOF&&!fclose(file));
 for(unsigned i=0;i<256;i++)colors[i]=(uint16_t)half(bytes+i*2);
}
static void unchanged(const Fixture *f,const Fixture *before){
 assert(!memcmp(&f->assets,&before->assets,sizeof f->assets)&&!memcmp(&f->fonts,&before->fonts,sizeof f->fonts));
 assert(!memcmp(&f->queue,&before->queue,sizeof f->queue)&&!memcmp(&f->transport,&before->transport,sizeof f->transport));
 assert(!memcmp(&f->hud,&before->hud,sizeof f->hud)&&!memcmp(&f->copies,&before->copies,sizeof f->copies)&&!memcmp(&f->immediate,&before->immediate,sizeof f->immediate));
}
static void source_colors(const Fixture *f,const uint8_t *original){
 for(unsigned i=0;i<f->registry.count;i++){
  const uint8_t *record=f->registry.records[i];unsigned n=half(record);uint32_t id=word(record+16);
  assert(id>=0x08000000u&&id-0x08000000u<=8388608u-n*2);
  if(memcmp(f->graphics.rom+id-0x08000000u,original+id-0x08000000u,n*2)){fprintf(stderr,"Missing package record%u identity%08x colors%u\n",i,id,n);assert(!"Registered palette source differs from supplied ROM");}
 }
}
static void check_transfer(Fixture *f,const uint8_t *original,unsigned mode){
 TaxiPaletteBanks before=f->banks;
 TaxiPaletteFadeStart fade={0};TaxiCityPaletteOwners owner={&f->registry,&f->banks,&fade,f->graphics.rom,f->graphics.rom_size,240,160};
 assert(!taxi_city_palette_gameplay_transfer(&owner,mode));
 for(unsigned i=0;i<f->registry.count;i++){
  const uint8_t *record=f->registry.records[i];unsigned n=half(record),offset=half(record+2)/2;
  uint32_t id=word(record+16);const uint8_t *source=original+id-0x08000000u;
  const uint16_t *old=record[7]?before.background:before.objects,*actual=record[7]?f->banks.background:f->banks.objects;
  for(unsigned j=0;j<n;j++){
   uint16_t expected=(uint16_t)half(source+j*2);
   if(mode)expected=(i==0||i==2||i==11)?old[offset+j]:taxi_palette_gameplay_alternate_rgb555(expected);
   assert(actual[offset+j]==expected);
  }
 }
}
int main(int argc,char **argv){
 assert(argc==5);
 TaxiGraphicsPackage candidate={0},old={0};
 assert(!taxi_graphics_package_load(&candidate,argv[1])&&!taxi_graphics_package_load(&old,argv[2]));
 uint8_t expected_records[320];FILE *file=fopen(argv[3],"rb");
 assert(file&&fread(expected_records,1,sizeof expected_records,file)==sizeof expected_records&&fgetc(file)==EOF&&!fclose(file));
 uint8_t *original=malloc(8388608);file=fopen(argv[4],"rb");assert(original&&file&&fread(original,1,8388608,file)==8388608&&fgetc(file)==EOF&&!fclose(file));
 unsigned cases=0;
 for(unsigned scene_index=0;scene_index<3;scene_index++)for(unsigned driver=0;driver<4;driver++)for(unsigned order=0;order<2;order++){
  Fixture f;init(&f,&candidate,4);assert(!memcmp(f.registry.records,expected_records,sizeof expected_records));
  if(!order)hud(&f,driver);
  TaxiSceneRegistration scene={0};uint16_t palette[256];read_scene(scene_index,&scene,palette);
  Fixture before=f;TaxiPaletteTransfer transfer={0};int index=taxi_ordinary_scene_palette_owned(&f.graphics,&scene,palette,&transfer);
  assert(index==16&&f.registry.count==17&&f.registry.bg_bytes==512&&f.registry.obj_bytes==512&&f.registry.flags==4);
  assert(!memcmp(f.registry.records,before.registry.records,320)&&!memcmp((uint8_t *)f.registry.records+340,(uint8_t *)before.registry.records+340,sizeof f.registry.records-340));
  assert(!memcmp(f.banks.objects,before.banks.objects,sizeof f.banks.objects)&&!memcmp(f.banks.background,palette,512));unchanged(&f,&before);
  assert(word(f.registry.records[16]+16)==word(scene.descriptor+24)&&transfer.source_identity==word(scene.descriptor+24)&&transfer.destination_identity==0x05000000u&&transfer.control==0x84000080u);
  if(order)hud(&f,driver);
  source_colors(&f,original);
  /* Excluded banks retain dynamic pause/current destination bytes. */
  for(unsigned bank=0;bank<16;bank++)if(bank==0||bank==1||bank==2||bank==11)
   for(unsigned j=0;j<16;j++)f.banks.objects[bank*16+j]=(uint16_t)(0x5555+j);
  check_transfer(&f,original,1);assert(memcmp(f.banks.background,palette,512));
  check_transfer(&f,original,0);assert(!memcmp(f.banks.background,palette,512));
  before=f;TaxiPaletteTransfer request_before=transfer;
  assert(taxi_ordinary_scene_palette_owned(&f.graphics,&scene,palette,&transfer)==-1);
  assert(!memcmp(&f.registry,&before.registry,sizeof f.registry)&&!memcmp(&f.banks,&before.banks,sizeof f.banks)&&!memcmp(&transfer,&request_before,sizeof transfer));unchanged(&f,&before);
  cases++;
 }
 assert(cases==24);
 /* Existing flags retain their original initial black/white/source policy. */
 const unsigned flags[]={0,1,2,4};
 for(unsigned k=0;k<4;k++){
  Fixture f;init(&f,&candidate,flags[k]);TaxiSceneRegistration scene={0};uint16_t palette[256];read_scene(0,&scene,palette);
  TaxiPaletteTransfer transfer;assert(taxi_ordinary_scene_palette_owned(&f.graphics,&scene,palette,&transfer)==16);
  for(unsigned i=0;i<256;i++)assert(f.banks.background[i]==(flags[k]==2?0x07ff:flags[k]==4?palette[i]:0));
 }
 /* The old sparse view must reject omitted city data without partial mutation. */
 Fixture absent;init(&absent,&old,4);Fixture before=absent;TaxiSceneRegistration scene={0};uint16_t palette[256];read_scene(0,&scene,palette);
 TaxiPaletteTransfer transfer={11,22,33},request_before=transfer;
 assert(taxi_ordinary_scene_palette_owned(&absent.graphics,&scene,palette,&transfer)==-1);
 assert(!memcmp(&absent.registry,&before.registry,sizeof absent.registry)&&!memcmp(&absent.banks,&before.banks,sizeof absent.banks)&&!memcmp(&transfer,&request_before,sizeof transfer));unchanged(&absent,&before);
 Fixture bad;init(&bad,&candidate,4);before=bad;palette[0]^=1;
 assert(taxi_ordinary_scene_palette_owned(&bad.graphics,&scene,palette,&transfer)==-1&&!memcmp(&bad.registry,&before.registry,sizeof bad.registry)&&!memcmp(&bad.banks,&before.banks,sizeof bad.banks));
 palette[0]^=1;scene.registered=0;assert(taxi_ordinary_scene_palette_owned(&bad.graphics,&scene,palette,&transfer)==-1);
 free(original);taxi_graphics_package_release(&old);taxi_graphics_package_release(&candidate);
 puts("PASS actual shared scene palette: 24 scene/driver/HUD-order cases, original16OBJ records exact, sceneBG index16, retained fonts/objects/queues, source coverage, alternate/restore, flags and missing-data/duplicate rollback");
 return 0;
}