#include "title_prompt_setup.h"
int taxi_title_prompt_setup(TaxiPaletteRegistration *registry,const uint32_t languages[4],unsigned language,const TaxiSpriteCopyAsset frames[4],TaxiTitlePromptSetup *out){
 if(!registry||!languages||!frames||!out||language>=4||registry->count<=10||registry->count>128)return -1;
 TaxiSpriteCopies queue={0};
 for(unsigned i=0;i<4;i++)if(taxi_sprite_replace(&queue,frames+i,languages[language],i,i,10,1,out->frames+i)!=1)return -1;
 out->palette=(TaxiSpriteImmediateCopy){0x08290ad4,0x05000340,0x80000010};
 for(unsigned i=0;i<4;i++)registry->records[10][16+i]=(uint8_t)(0x08290ad4u>>(8*i));
 return 0;
}
