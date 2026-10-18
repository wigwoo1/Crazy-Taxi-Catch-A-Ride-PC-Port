#include "startup_save_preview.h"
#include "startup_save_owned.h"
#include "save_audio_host.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int registered(void *context,uint32_t identity){(void)context;return identity==0x080161d9?0:-1;}
static int read_file(const char *path,void *data,size_t size){FILE *f=fopen(path,"rb");if(!f)return -1;int rc=fread(data,1,size,f)==size?0:-1;fclose(f);return rc;}
int taxi_startup_save_preview(TaxiStartupInitContinuation *c,const char *path,uint32_t clock){
 if(!c||!path)return -1;
 uint8_t *module=malloc(181241),pitch[1536];TaxiAudioAsset effects={0};TaxiAudioHost *host=NULL;int rc=-1;
 if(!module||read_file("outputs/crazy-taxi-native/assets/music/original-module.bin",module,181241)||read_file("outputs/crazy-taxi-native/assets/music/pitch-table.bin",pitch,1536))goto done;
 TaxiMusicAsset music;if(taxi_music_asset_index(&music,module,181241)||taxi_audio_asset_load(&effects,"outputs/crazy-taxi-native/assets/audio"))goto done;
 TaxiSaveFileDevice device;int status=taxi_save_file_device_open(&device,path);if(status<0||taxi_audio_host_open(&host,21024))goto done;
 TaxiMusicTracker tracker={0};TaxiMusicContext music_context={0};TaxiAudioFrame audio={.rate=21024,.processing=1};TaxiAudioChannel channels[4]={0};
 uint8_t storage[704]={0};int16_t mix[352]={0};TaxiAudioOutputBuffer buffers[2]={{storage,352},{storage+352,352}};
 TaxiAudioBootstrap bootstrap={.audio=&audio,.tracker=&tracker,.channels=channels,.configuration={21024,352,1},.buffers=buffers,.output_storage=storage,.output_capacity=704,.mix=mix,.mix_capacity=352};
 TaxiFrameEvent event={0};TaxiFrameWait wait={1,0};
 TaxiFrameAudioOwners frame={.event=&event,.audio=&audio,.tracker=&tracker,.music=&music,.pitch=pitch,.music_context=&music_context,.channels=channels,.effects=&effects,.mix=mix,.capacity=352,.buffers=buffers,.scanline=160,.registered=registered};
 TaxiSaveAudioHost native_host={host,&frame,21024};TaxiSaveLoadOwners owners={&device,&bootstrap,&wait,&event,taxi_save_audio_host_deliver,taxi_save_audio_host_platform,&native_host};
 TaxiSaveLoad save={.settings=c->owners.settings,.runtime_rows=c->owners.runtime_rows,.first_ten=c->owners.first_ten,.current_clock=clock,.interrupt_callback=0x080161d9,.music_enabled=1,.effects_enabled=1,.music_volume=*c->owners.music_volume,.effects_volume=*c->owners.effects_volume,.language=*c->owners.language};
 rc=taxi_startup_save_owned(c,&save,&owners);
 if(!rc)printf("NATIVE STARTUP SAVE COMPLETE: device=%d reads=%u verify=%u clock=%u->%u initialized=%u dispatcher=%08lx volumes=%u/%u clock_source=diagnostic_host_frames\n",status,device.reads,device.verifications,clock,save.current_clock,c->init->initialized_1ea4,(unsigned long)c->init->dispatcher_3878,save.music_volume,save.effects_volume);
 done:if(host&&taxi_audio_host_close(&host))rc=-1;taxi_audio_asset_free(&effects);free(module);return rc;
}
