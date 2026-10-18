#ifndef TAXI_DIAGNOSTIC_AUDIO_OWNER_SNAPSHOT_H
#define TAXI_DIAGNOSTIC_AUDIO_OWNER_SNAPSHOT_H
/* Source-fixture importer only; never a native production startup dependency.
   All pointers are normalized to native owner indices or immutable assets. */
#include "../src/game_audio_controller.h"
#include "../src/game_audio_channels.h"
#include <string.h>
#include <assert.h>
static uint32_t audio_snapshot_u32(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t audio_snapshot_s32(const uint8_t *p){uint32_t u=audio_snapshot_u32(p);int32_t s;memcpy(&s,&u,4);return s;}
static int16_t audio_snapshot_s16(const uint8_t *p){uint16_t u=(uint16_t)(p[0]|((unsigned)p[1]<<8));int16_t s;memcpy(&s,&u,2);return s;}
static uint32_t audio_snapshot_handle(uint32_t pointer){
 if(!pointer)return 0;
 assert(pointer>=0x02007574&&pointer<0x020075f4&&(pointer-0x02007574)%32==0);
 return (pointer-0x02007574)/32+1;
}
static void audio_owner_snapshot_import(const uint8_t ram[0x40000],const uint32_t payload[203],
 TaxiAudioAsset *assets,TaxiGameAudioController *controller,TaxiGameAudioChannels *channels){
 TaxiGameAudioController c={0};TaxiGameAudioChannels bank={0};bank.assets=assets;
 bank.output_rate=audio_snapshot_u32(ram+0x7564);assert(bank.output_rate);
 for(unsigned j=0;j<10;j++)c.handles[j]=audio_snapshot_handle(audio_snapshot_u32(ram+0x25e8+j*4));
 c.cooldown=audio_snapshot_s32(ram+0x2614);c.amplitude=audio_snapshot_s32(ram+0x25dc);
 c.hold_timer=audio_snapshot_s32(ram+0x25e4);c.factor=audio_snapshot_s16(ram+0x25da);
 c.flag0=ram[0x25d0];c.flag1=ram[0x25d1];c.pending2=ram[0x25d2];c.pending3=ram[0x25d3];
 c.enabled4=ram[0x25d4];c.reward_cooldown=ram[0x25d5];c.last=ram[0x25d8];
 c.context_selection=(int8_t)ram[0x25d6];c.driver_selection=(int8_t)ram[0x25d7];c.classification=(int8_t)ram[0x2610];
 for(unsigned slot=0;slot<4;slot++){
  const uint8_t *p=ram+0x7574+slot*32;TaxiAudioChannel *q=bank.channels+slot;
  q->stamp=audio_snapshot_u32(p);q->flags4=p[4];q->flags5=p[5];q->volume=p[6];q->last=p[7];
  q->rate=(uint16_t)(p[8]|((unsigned)p[9]<<8));q->step=audio_snapshot_u32(p+16);q->phase=audio_snapshot_u32(p+20);
  uint32_t sound=audio_snapshot_u32(p+24);
  if(sound){int found=-1;for(unsigned index=0;index<203;index++)if(payload[index]==sound){assert(found<0);found=(int)index;}
   assert(found>=0);q->sound=assets->sounds+found;
  }
 }
 *controller=c;*channels=bank;
}
#endif