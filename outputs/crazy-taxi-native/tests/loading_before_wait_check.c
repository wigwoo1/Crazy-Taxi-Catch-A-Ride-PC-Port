#define main taxi_previous_loading_fixture_main
#include "ordinary_loading_pending_check.c"
#undef main
typedef struct {Fixture f;unsigned calls;int fail;} Wall;
static int before_wait(void *context,TaxiHostTime *host){
 Wall *w=context;Fixture *f=&w->f;assert(host==&f->host&&f->owner.phase==TAXI_LOADING_EXECUTING);w->calls++;
 TaxiOrdinaryLoadingPending saved=f->owner;assert(taxi_ordinary_loading_pending_prepare(&f->owner)==-1&&taxi_ordinary_loading_pending_begin(&f->owner)==-1&&taxi_ordinary_loading_pending_poll(&f->owner)==-1);taxi_ordinary_loading_pending_cancel(&f->owner);assert(!memcmp(&saved,&f->owner,sizeof saved));
 release(f);assert(f->wait.handshake==1&&f->event.acknowledge==1&&f->audio);return w->fail?-1:0;
}
int main(int argc,char **argv){
 assert(argc==2);uint8_t *rom=malloc(8388608);assert(rom);FILE *file=fopen(argv[1],"rb");assert(file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF&&!fclose(file));Wall *w=calloc(1,sizeof *w);assert(w);
 for(unsigned standalone=0;standalone<2;standalone++)for(unsigned failure=0;failure<2;failure++){
  memset(w,0,sizeof *w);Fixture *f=&w->f;initialize(f,rom);w->fail=(int)failure;f->owner.before_wait=before_wait;f->owner.wait_context=w;
  assert(!(standalone?taxi_ordinary_loading_pending_prepare_present(&f->owner):taxi_ordinary_loading_pending_prepare(&f->owner)));
  TaxiLoadingPublishedFrame published=f->published;TaxiPaletteFadeStart fade=f->fade;TaxiPaletteBanks banks=f->banks;uint32_t counter=f->host.counter;int32_t delta=f->owner.preparation_delta;unsigned events=f->events;
  int rc=taxi_ordinary_loading_pending_begin(&f->owner);assert(w->calls==1&&f->host.counter!=counter&&f->events>events&&!memcmp(&published,&f->published,sizeof published)&&delta==f->owner.preparation_delta&&!memcmp(&fade,&f->fade,sizeof fade)&&!memcmp(&banks,&f->banks,sizeof banks));
  if(failure){assert(rc==-1&&f->owner.phase==TAXI_LOADING_ABORTED&&f->wait.handshake==1&&taxi_ordinary_loading_pending_begin(&f->owner)==-1&&w->calls==1);continue;}
  assert(!rc&&f->owner.phase==TAXI_LOADING_WAITING&&!(f->wait.handshake&1));
  for(unsigned k=0;k<16;k++)assert(!taxi_ordinary_loading_pending_poll(&f->owner)&&w->calls==1&&!memcmp(&published,&f->published,sizeof published)&&!f->samples);
  release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1&&f->published.generation==1&&f->samples==1&&w->calls==1&&f->published.counter==f->host.counter&&f->published.preparation_delta==delta);
  printf("PASS %s loading pre-wait: preparation elapsed/audio delivered before fresh flagclear;16stale polls unchanged; next event alone publishes once\n",standalone?"standalone26318":"D764");
 }
 free(w);free(rom);puts("PASS host loading pre-wait hook: both native preparation paths, reentry rejection, post-time failure partial effects retained/no retry; audio event body counter fixture only");return 0;
}
