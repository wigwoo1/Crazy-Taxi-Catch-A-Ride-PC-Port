/* Native source workload measurement. No clock/cadence/game owner changes.
   CPU records stay in RAM. GPU timestamp results are read only AFTER gameplay;
   GPU spans may include idle/CPU submission gaps, not GPU-busy utilization. */
enum {SP_SETUP,SP_RESOURCES,SP_INITIALIZE,SP_ITERATION,SP_PREPARE,SP_SIMULATION,SP_PROJECTION,SP_GRAPHICS,SP_EVENTS,SP_WAIT_BEGIN,SP_PUBLICATION,SP_EMPTY_POLL,SP_RENDER,SP_SNAPSHOT,SP_DRAW,SP_OBJECTS,SP_PURITY,SP_HOST_TRACE,SP_BLIT,SP_SWAP,SP_DRAIN,SP_COUNT};
static const char *source_profile_names[SP_COUNT]={"profile_setup","presentation_resources","initialize","iteration_total","prepare_total","simulation","projection","graphics_prepare","source_event_delivery","wait_begin","publication","empty_poll","render_total","snapshot_before","draw_submit","obj_submit","purity_check","host_trace_io","blit_submit","swap","final_gpu_drain"};
typedef struct {LARGE_INTEGER start;uint64_t publication,preparation;uint32_t counter,reset;unsigned stage;} SourceProfileToken;
typedef struct {SourceProfileToken token;LARGE_INTEGER end;uint32_t counter_end;} SourceProfileRecord;
typedef struct {uint64_t publication;uint32_t reset,counter;GLuint queries[2];} SourceGpuProfile;
typedef void (APIENTRY *SourceGenQueryFn)(GLsizei,GLuint *);
typedef void (APIENTRY *SourceDeleteQueryFn)(GLsizei,const GLuint *);
typedef struct {uint64_t calls,ticks,minimum,maximum,retained;} SourceProfileAggregate;
typedef void (APIENTRY *SourceCounterFn)(GLuint,GLenum);
typedef void (APIENTRY *SourceResultFn)(GLuint,GLenum,uint64_t *);
typedef void (APIENTRY *SourceAvailableFn)(GLuint,GLenum,GLint *);
static struct {SourceProfileRecord *records;SourceGpuProfile *gpu;unsigned count,gpu_count;LARGE_INTEGER frequency;SourceGenQueryFn gen;SourceDeleteQueryFn remove;SourceProfileAggregate aggregate[SP_COUNT];SourceCounterFn counter;SourceResultFn result;SourceAvailableFn available;int ready;} source_profile;
enum {SOURCE_PROFILE_CAPACITY=131072,SOURCE_GPU_CAPACITY=8192};
static SourceProfileToken source_profile_begin(unsigned stage){
 SourceProfileToken t={0};if(!source_frame_profile_path)return t;
 if(stage>=SP_COUNT)fail("Source profile stage invalid.");
 QueryPerformanceCounter(&t.start);t.stage=stage;t.publication=source_commits;t.preparation=source_prepares;t.counter=live_host_time.counter;t.reset=(uint32_t)source_resets;return t;
}
static void source_profile_end(SourceProfileToken t){
 if(!source_frame_profile_path)return;
 if(!source_profile.ready)fail("Source profile CPU recording before initialization.");
 LARGE_INTEGER end;QueryPerformanceCounter(&end);if(end.QuadPart<t.start.QuadPart)fail("Source profile CPU clock moved backwards.");
 uint64_t ticks=(uint64_t)(end.QuadPart-t.start.QuadPart);SourceProfileAggregate *a=source_profile.aggregate+t.stage;
 if(!a->calls||ticks<a->minimum)a->minimum=ticks;if(ticks>a->maximum)a->maximum=ticks;a->calls++;a->ticks+=ticks;
 int quiet=t.stage==SP_EMPTY_POLL||(t.stage==SP_EVENTS&&t.counter==live_host_time.counter)||(t.stage==SP_ITERATION&&t.counter==live_host_time.counter&&t.publication==source_commits&&t.preparation==source_prepares&&t.reset==(uint32_t)source_resets);
 /* Preserve exact quiet-call counts/sum/min/max; detailed rows retain work
    plus quiet outliers>=0.5ms. No timing observations silently disappear. */
 if(quiet&&ticks<(uint64_t)(source_profile.frequency.QuadPart/2000))return;
 if(source_profile.count>=SOURCE_PROFILE_CAPACITY)fail("Source profile detailed CPU capacity exceeded; aggregate counts preserved, run incomplete.");
 SourceProfileRecord *r=source_profile.records+source_profile.count++;r->token=t;r->end=end;r->counter_end=live_host_time.counter;a->retained++;
}
static FARPROC source_profile_symbol(const char *name){FARPROC p=wglGetProcAddress(name);if((uintptr_t)p<=3||(uintptr_t)p==UINTPTR_MAX)fail("Source profile GPU timer API unavailable.");return p;}
static void source_profile_initialize(void){
 if(!source_frame_profile_path)return;
 if(source_profile.ready)fail("Source profile initialized twice.");
 source_profile.records=calloc(SOURCE_PROFILE_CAPACITY,sizeof *source_profile.records);source_profile.gpu=calloc(SOURCE_GPU_CAPACITY,sizeof *source_profile.gpu);
 if(!source_profile.records||!source_profile.gpu||!QueryPerformanceFrequency(&source_profile.frequency))fail("Source profile allocation/clock setup failed.");
 FARPROC p=source_profile_symbol("glGenQueries");memcpy(&source_profile.gen,&p,sizeof p);p=source_profile_symbol("glDeleteQueries");memcpy(&source_profile.remove,&p,sizeof p);p=source_profile_symbol("glQueryCounter");memcpy(&source_profile.counter,&p,sizeof p);p=source_profile_symbol("glGetQueryObjectui64v");memcpy(&source_profile.result,&p,sizeof p);p=source_profile_symbol("glGetQueryObjectiv");memcpy(&source_profile.available,&p,sizeof p);
 source_profile.ready=1;SourceProfileToken t=source_profile_begin(SP_SETUP);
 for(unsigned i=0;i<SOURCE_GPU_CAPACITY;i++)source_profile.gen(2,source_profile.gpu[i].queries);
 if(glGetError()!=GL_NO_ERROR)fail("Source profile GPU query allocation failed.");source_profile_end(t);
}
static unsigned source_profile_gpu_begin(void){
 if(!source_frame_profile_path)return UINT_MAX;
 if(!source_profile.ready||source_profile.gpu_count>=SOURCE_GPU_CAPACITY)fail("Source profile GPU capacity exceeded.");
 unsigned i=source_profile.gpu_count++;SourceGpuProfile *g=source_profile.gpu+i;g->publication=source_commits;g->counter=live_host_time.counter;g->reset=(uint32_t)source_resets;source_profile.counter(g->queries[0],0x8e28);return i;
}
static void source_profile_gpu_end(unsigned i){if(i!=UINT_MAX)source_profile.counter(source_profile.gpu[i].queries[1],0x8e28);}
static void source_profile_finish(void){
 if(!source_frame_profile_path)return;
 if(!source_profile.ready||!source_profile.gpu_count||source_profile.gpu_count!=source_gpu_draws)fail("Source profile incomplete frame coverage.");
 SourceProfileToken drain=source_profile_begin(SP_DRAIN);glFinish();source_profile_end(drain);
 FILE *f=fopen(source_frame_profile_path,"wb");if(!f)fail("Source profile CPU output unavailable.");
 fprintf(f,"stage,publication,preparation,reset,counter_start,counter_end,start_qpc,end_qpc,cpu_ms\n");
 for(unsigned i=0;i<source_profile.count;i++){const SourceProfileRecord *r=source_profile.records+i;fprintf(f,"%s,%llu,%llu,%u,%u,%u,%lld,%lld,%.9f\n",source_profile_names[r->token.stage],(unsigned long long)r->token.publication,(unsigned long long)r->token.preparation,r->token.reset,r->token.counter,r->counter_end,(long long)r->token.start.QuadPart,(long long)r->end.QuadPart,(r->end.QuadPart-r->token.start.QuadPart)*1000.0/source_profile.frequency.QuadPart);}
 if(ferror(f)||fclose(f))fail("Source profile CPU output write failed.");
 char path[MAX_PATH];if(snprintf(path,sizeof path,"%s.stats.csv",source_frame_profile_path)>=(int)sizeof path)fail("Source profile aggregate path too long.");f=fopen(path,"wb");if(!f)fail("Source profile aggregate output unavailable.");
 fprintf(f,"stage,count,retained,total_cpu_ms,mean_cpu_ms,min_cpu_ms,max_cpu_ms\n");uint64_t observed=0;
 for(unsigned i=0;i<SP_COUNT;i++){const SourceProfileAggregate *a=source_profile.aggregate+i;double unit=1000.0/source_profile.frequency.QuadPart;observed+=a->calls;fprintf(f,"%s,%llu,%llu,%.9f,%.9f,%.9f,%.9f\n",source_profile_names[i],(unsigned long long)a->calls,(unsigned long long)a->retained,a->ticks*unit,a->calls?(double)a->ticks*unit/a->calls:0,a->minimum*unit,a->maximum*unit);}
 if(ferror(f)||fclose(f))fail("Source profile aggregate output write failed.");
 if(snprintf(path,sizeof path,"%s.gpu.csv",source_frame_profile_path)>=(int)sizeof path)fail("Source profile GPU path too long.");f=fopen(path,"wb");if(!f)fail("Source profile GPU output unavailable.");
 fprintf(f,"publication,reset,source_counter,gpu_start_ns,gpu_end_ns,gpu_span_ms\n");
 for(unsigned i=0;i<source_profile.gpu_count;i++){SourceGpuProfile *g=source_profile.gpu+i;GLint first_ready=0,last_ready=0;uint64_t first,last;source_profile.available(g->queries[0],0x8867,&first_ready);source_profile.available(g->queries[1],0x8867,&last_ready);if(!first_ready||!last_ready)fail("Source profile GPU query not complete after final drain.");source_profile.result(g->queries[0],0x8866,&first);source_profile.result(g->queries[1],0x8866,&last);if(last<first)fail("Source profile GPU timestamp order invalid.");fprintf(f,"%llu,%u,%u,%llu,%llu,%.9f\n",(unsigned long long)g->publication,g->reset,g->counter,(unsigned long long)first,(unsigned long long)last,(last-first)/1e6);}
 if(ferror(f)||fclose(f)||glGetError()!=GL_NO_ERROR)fail("Source profile GPU output failed.");
 for(unsigned i=0;i<SOURCE_GPU_CAPACITY;i++)source_profile.remove(2,source_profile.gpu[i].queries);
 printf("SOURCE PROFILE CONDITIONS: host_trace%d input_trace%d lifecycle%d passengers%d traffic%d target%dx%d observed%llu; quiet calls aggregate exact counts/sum/min/max, details retain active work and quiet outliers>=0.5ms.\n",host_time_trace!=NULL,input_time_trace!=NULL,source_frame_lifecycle_smoke,passenger_diagnostic,traffic_preview,target_width,target_height,(unsigned long long)observed);
 printf("SOURCE PROFILE: CPU%u GPU%u frequency%lld; buffered RAM records, no per-frame GPU result waits; inclusive CPU stages and GPU timeline spans, not busy utilization or scanout proof.\n",source_profile.count,source_profile.gpu_count,(long long)source_profile.frequency.QuadPart);
 free(source_profile.records);free(source_profile.gpu);memset(&source_profile,0,sizeof source_profile);
}