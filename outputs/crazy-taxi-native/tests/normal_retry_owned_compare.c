#include "normal_retry_owned.h"
#include "game_audio_cleanup.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 unsigned seed,frozen,playing,saved,effects;
 while(scanf("%u %u %u %u %u",&seed,&frozen,&playing,&saved,&effects)==5){
  TaxiPauseMenu pause={.outer_state=1,.frozen=(uint8_t)frozen,.exit_flag=17};TaxiSessionInit session={0};TaxiPassengerScene scene={0};
  TaxiGameAudioController sounds={0};TaxiAudioChannel channels[4]={0};TaxiAudioSettingsState audio={17,19,128,(uint8_t)effects,(uint8_t)saved};TaxiMusicTracker tracker={0};tracker.state.playing=(uint8_t)playing;
  TaxiPaletteRegistration registry={0};TaxiPaletteBanks banks={0};TaxiPaletteFadeStart fade={0};const uint8_t rom[1]={0};TaxiCityPaletteOwners palette={&registry,&banks,&fade,rom,1,240,160};TaxiOrdinaryPresentationOwner presentation={.palette=&palette};TaxiPresentationService service={0};TaxiOrdinaryLoadingPending loading={.presentation=&presentation,.service=&service};uint16_t rate=16000;uint8_t override=0,exit=165;
  for(unsigned j=0;j<10;j++)sounds.handles[j]=((seed>>((j%4)*2))+j*3)%5;
  uint32_t stops[10]={0};unsigned count=taxi_game_audio_cleanup_requests(sounds.handles,stops);
  TaxiNormalRetryOwned owner={.pause=&pause,.loading=&loading,.session=&session,.scene=&scene,.sounds=&sounds,.channels=channels,.audio=&audio,.tracker=&tracker,.output_rate=&rate,.override_1708=&override,.exit_5f1a=&exit};
  session.session_165e[51]=255;session.session_165e[57]=9;TaxiPauseEvents event={.fade_out=1};assert(!taxi_normal_retry_owned_fade(&owner,&event));fade.fade.active_177b=0;assert(taxi_normal_retry_owned_transfer(&owner)==2&&owner.stops==count);
  printf("%u %u %u %u %u %u %u %u %u %u %u %u",seed,frozen,playing,saved,effects,tracker.state.playing,audio.effects_enabled,audio.saved_effects_enabled,session.outer_1ea8,session.effect_5f20[0],exit,owner.requests.count);
  for(unsigned j=0;j<owner.requests.count;j++)printf(" %u",owner.requests.calls[j]);printf(" %u",count);for(unsigned j=0;j<count;j++)printf(" %u",stops[j]);
  for(unsigned j=0;j<10;j++)printf(" %u",sounds.handles[j]);printf(" %u %u\n",session.session_165e[51],session.session_165e[57]);
  TaxiNormalRetryOwned before=owner;assert(taxi_normal_retry_owned_transfer(&owner)==2&&!memcmp(&before,&owner,sizeof owner));
 }
 return 0;
}
