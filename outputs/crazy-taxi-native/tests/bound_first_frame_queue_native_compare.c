#include "render_queue.h"
#include "raster_dispatch.h"
#include "engine_callback_identity.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
typedef struct {unsigned flags,textured,material;} Input;
typedef struct {Input input[101];} Context;
static int dispatch(void *opaque,unsigned kind,size_t payload){
 Context *c=opaque;assert(kind==0&&payload<101);Input in=c->input[payload];uint32_t owner=taxi_raster_callback_global((uint16_t)in.flags,(int)in.textured,(uint8_t)in.material);
 uint32_t original=taxi_engine_callback_identity(owner);assert(original);uint32_t out[3]={(uint32_t)payload,original-0x08000700+0x03000008,taxi_raster_dispatch((uint16_t)in.flags).vertices};
 return fwrite(out,1,sizeof out,stdout)==sizeof out?0:-1;
}
int main(int argc,char **argv){if(argc!=2)return 1;FILE *f=fopen(argv[1],"rb");if(!f)return 2;if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 3;
 TaxiRenderPacket storage[101];TaxiRenderQueue queue;Context context;taxi_render_queue_init(&queue,storage,101);
 for(unsigned i=0;i<101;i++){uint32_t row[4];if(fread(row,1,sizeof row,f)!=sizeof row)return 4;context.input[i]=(Input){row[1],row[2],row[3]};if(taxi_render_queue_push(&queue,row[0],0,i))return 5;}
 fclose(f);if(taxi_render_queue_consume(&queue,dispatch,&context))return 6;assert(queue.count==0);for(unsigned i=0;i<128;i++)assert(queue.heads[i]==SIZE_MAX);return 0;}
