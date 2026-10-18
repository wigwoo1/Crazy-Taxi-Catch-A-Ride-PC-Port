/* Read-only diagnostic: native initial owners and sampled inputs.
   Captures named fields, never struct padding/pointers and never imports state.
   Not a complete game-memory identity or original input-caller proof. */
#include <errno.h>
static const char *source_context_path,*source_context_expect_path;
typedef struct {unsigned count;int64_t v[128];} TaxiContextRow;
static struct {
 int64_t initial[640];unsigned initial_count,count,pending,written,finalized,ok;
 TaxiContextRow rows[20000];
} source_context;
/* Expected values are comparisons only: never assigned to native game state.
   Text contract v1: CTX1, 625 initial values, row count, then count+values per row.
   Bounded parse at startup; no file access during input sampling. */
static struct {
 int64_t initial[625];unsigned count;TaxiContextRow rows[20000];
} source_context_expected;
static int64_t source_context_number(FILE *f){
 char token[32];char *end;int64_t v;
 if(fscanf(f,"%31s",token)!=1)fail("Context contract truncated.");
 errno=0;v=strtoll(token,&end,10);
 if(errno||end==token||*end)fail("Context contract invalid integer.");
 return v;
}
static void source_context_load_expected(void){
 FILE *f=fopen(source_context_expect_path,"rb");char magic[8];
 if(!f)fail("Context contract open failed.");
 if(fscanf(f,"%7s",magic)!=1||strcmp(magic,"CTX1"))fail("Context contract version mismatch.");
 for(unsigned i=0;i<625;i++)source_context_expected.initial[i]=source_context_number(f);
 int64_t count=source_context_number(f);
 if(count<1||count>20000)fail("Context contract row count invalid.");
 source_context_expected.count=(unsigned)count;
 for(unsigned i=0;i<source_context_expected.count;i++){
  TaxiContextRow *r=&source_context_expected.rows[i];int64_t n=source_context_number(f);
  if(n!=8&&n!=120)fail("Context contract row size invalid.");
  r->count=(unsigned)n;
  for(unsigned j=0;j<r->count;j++)r->v[j]=source_context_number(f);
  if(r->v[0]!=(n==8?1:2))fail("Context contract row site invalid.");
 }
 char extra[2];if(fscanf(f,"%1s",extra)==1||ferror(f))fail("Context contract trailing data or read failure.");
 if(fclose(f))fail("Context contract close failed.");
}
static void source_context_compare(const TaxiContextRow *r,unsigned total){
 if(!source_context_expect_path)return;
 if(source_context.count>=source_context_expected.count)fail("Context contract exhausted.");
 const TaxiContextRow *expected=&source_context_expected.rows[source_context.count];
 if(expected->count!=total)fail("Context contract boundary differs.");
 for(unsigned i=0;i<r->count;i++)if(r->v[i]!=expected->v[i]){
  fprintf(stderr,"Context divergence row %u field %u expected %lld actual %lld.\\n",source_context.count,i,(long long)expected->v[i],(long long)r->v[i]);
  fail("Context contract state or input mismatch.");
 }
}
static void source_context_push(TaxiContextRow *r,int64_t v){
 if(r->count>=128)fail("Context row capacity exceeded.");r->v[r->count++]=v;
}
static void source_context_input_state(TaxiContextRow *r,const TaxiInput *s){
 source_context_push(r,s->raw_keys);source_context_push(r,s->auxiliary_mask);source_context_push(r,s->pressed_mask);
 for(unsigned i=0;i<10;i++)source_context_push(r,s->held[i]);
 for(unsigned i=0;i<10;i++)source_context_push(r,s->previous[i]);
 for(unsigned i=0;i<10;i++)source_context_push(r,s->repeat[i]);
 for(unsigned i=0;i<10;i++)source_context_push(r,s->pressed[i]);
 for(unsigned i=0;i<10;i++)source_context_push(r,s->countdown[i]);
}
static TaxiContextRow *source_context_row(void){
 if(!source_context.initial_count||source_context.pending||source_context.count>=20000)fail("Context boundary or capacity failure.");
 return &source_context.rows[source_context.count];
}
static void source_context_initial(unsigned city,unsigned driver){
 if(!source_context_path)return;
 if(source_context.initial_count||source_context.count)fail("Duplicate native initial context.");
 int64_t prefix[]={live_host_time.counter,(int64_t)live_host_time.clock.phase,live_host_time.frame.previous_3524,live_host_time.frame.delta_3b00,live_host_time.frame.elapsed_755c,live_host_time.frame.animation_1e10,live_host_time.frame.page_7540,passenger_scene.random_state,startup_stage,source_parent_phase,city,driver,passenger_scene.session.ranking_loaded};
 for(unsigned i=0;i<sizeof prefix/sizeof *prefix;i++)source_context.initial[source_context.initial_count++]=prefix[i];
 for(unsigned i=0;i<512;i++)source_context.initial[source_context.initial_count++]=native_settings[i];
 for(unsigned i=0;i<100;i++)source_context.initial[source_context.initial_count++]=passenger_scene.session.ranking_scores[i];
 if(source_context.initial_count!=625)fail("Native initial context schema mismatch.");
 if(source_context_expect_path)for(unsigned i=0;i<625;i++)if(source_context.initial[i]!=source_context_expected.initial[i]){
  fprintf(stderr,"Initial context divergence field %u.\\n",i);fail("Context contract initial state mismatch.");
 }
}
static void source_context_menu(uint16_t mask){
 if(!source_context_path)return;TaxiContextRow *r=source_context_row();
 int64_t v[]={1,live_host_time.counter,startup_stage,source_parent_phase,passenger_scene.random_state,mask,native_settings[4],native_settings[5]};
 for(unsigned i=0;i<sizeof v/sizeof *v;i++)source_context_push(r,v[i]);
 source_context_compare(r,8);source_context.count++;
}
static void source_context_input_before(TaxiXboxSample pad,uint16_t keys,uint32_t delta){
 if(!source_context_path)return;TaxiContextRow *r=source_context_row();
 int64_t v[]={2,live_host_time.counter,delta,startup_stage,source_parent_phase,passenger_scene.random_state,native_settings[0],keys,pad.connected,pad.active_low_keys,pad.steering,pad.accelerator,pad.brake,pad.look_back};
 for(unsigned i=0;i<sizeof v/sizeof *v;i++)source_context_push(r,v[i]);
 source_context_input_state(r,&driving_buttons);source_context_compare(r,120);source_context.pending=1;
}
static void source_context_input_after(void){
 if(!source_context_path)return;if(!source_context.pending)fail("Context input post without pre.");
 TaxiContextRow *r=&source_context.rows[source_context.count];source_context_input_state(r,&driving_buttons);
 if(r->count!=120)fail("Native input context schema mismatch.");
 source_context_compare(r,120);source_context.pending=0;source_context.count++;
}
static void source_context_array(FILE *f,const int64_t *v,unsigned n){
 fputc('[',f);for(unsigned i=0;i<n;i++)fprintf(f,"%s%lld",i?",":"",(long long)v[i]);fputc(']',f);
}
static void source_context_write(void){
 if(!source_context_path||source_context.written)return;source_context.written=1;
 if(source_context.pending||!source_context.initial_count){fputs("Context capture has incomplete boundary.",stderr);return;}
 FILE *f=fopen(source_context_path,"wbx");if(!f){fputs("Context capture exclusive open failed.",stderr);return;}
 fprintf(f,"{\"version\":1,\"capture_finalized\":%s,\"initial_prefix_fields\":[\"counter\",\"clock_phase\",\"previous\",\"delta\",\"elapsed\",\"animation\",\"page\",\"rng\",\"startup_stage\",\"parent_phase\",\"city\",\"driver\",\"ranking_loaded\"],\"settings_count\":512,\"scores_count\":100,\"input_prefix_fields\":[\"site\",\"counter\",\"delta\",\"stage\",\"parent\",\"rng\",\"profile_byte\",\"keys\",\"connected\",\"pad_keys\",\"steering\",\"accelerator\",\"brake\",\"look_back\"],\"input_state_layout\":\"raw,auxiliary,pressed_mask,held10,previous10,repeat10,pressed10,countdown10; before then after\",\"initial\":",source_context.finalized?"true":"false");
 source_context_array(f,source_context.initial,source_context.initial_count);fputs(",\"rows\":[",f);
 for(unsigned i=0;i<source_context.count;i++){if(i)fputc(',',f);source_context_array(f,source_context.rows[i].v,source_context.rows[i].count);}
 fputs("]}",f);int error=ferror(f);if(fclose(f))error=1;if(error){fputs("Context capture write failed.",stderr);return;}source_context.ok=1;
}
static void source_context_initialize(void){
 if(source_context_expect_path&&(!source_context_path||!source_timing_replay_path))fail("Context expectation requires capture and clock replay.");
 if(!source_context_path)return;
 if(source_context_expect_path)source_context_load_expected();
 if(!source_parent_smoke||native_save_path)fail("Context capture requires fresh native parent smoke.");
 FILE *f=fopen(source_context_path,"rb");if(f){fclose(f);fail("Context capture refuses existing output.");}
 if(atexit(source_context_write))fail("Context capture shutdown registration failed.");
}
static void source_context_finish(void){
 if(!source_context_path)return;
 if(source_context_expect_path&&(source_context.pending||source_context.count!=source_context_expected.count))fail("Context contract incomplete consumption.");
 source_context.finalized=1;source_context_write();
 if(!source_context.ok)fail("Context capture output failed.");
}
