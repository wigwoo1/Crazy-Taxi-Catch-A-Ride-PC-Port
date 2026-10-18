#include "taxi_graphics_package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
void taxi_graphics_package_release(TaxiGraphicsPackage *p){if(p){free(p->bytes);p->bytes=NULL;p->size=0;}}
int taxi_graphics_package_load(TaxiGraphicsPackage *out,const char *path){
 if(!out||!path)return -1;FILE *f=fopen(path,"rb");if(!f)return -1;
 uint8_t h[12];TaxiGraphicsPackage next={0};int rc=-1;
 if(fread(h,1,12,f)!=12||memcmp(h,"CTG1",4)||word(h+4)!=8388608u)goto done;
 uint32_t count=word(h+8);if(!count||count>4096)goto done;
 next.size=word(h+4);next.bytes=calloc(next.size,1);if(!next.bytes)goto done;
 uint32_t end=0;
 for(uint32_t i=0;i<count;i++){uint8_t entry[8];if(fread(entry,1,8,f)!=8)goto done;uint32_t offset=word(entry),n=word(entry+4);
  if(!n||offset<end||offset>next.size||n>next.size-offset)goto done;
  if(fread(next.bytes+offset,1,n,f)!=n)goto done;end=offset+n;
 }
 if(fgetc(f)!=EOF||ferror(f))goto done;rc=0;
 done:if(fclose(f))rc=-1;
 if(rc){taxi_graphics_package_release(&next);return -1;}
 taxi_graphics_package_release(out);*out=next;return 0;
}
