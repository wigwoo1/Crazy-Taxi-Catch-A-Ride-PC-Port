/* Opt-in diagnostic only. Record completed native frame inputs in memory;
   defer file I/O until shutdown so capture cannot add a frame-time stall.
   The first1024 completed records and final record are retained explicitly. */
typedef struct {
 TaxiPoint taxi,projected;TaxiCameraView view;TaxiOrdinaryFrameInput input;
 int32_t centers[2][2],distance_fixed,source_distance_fixed,base,heading,motion;
 int32_t tracked_pitch,tracked_heading,yaw_offset,yaw_target,preparation_delta;
 uint32_t prepared_counter,published_counter;uint64_t generation;
 unsigned root,city,width,height,intro,fade,frozen,mode,pullback;
} TaxiSourceCaptureRecord;
static struct {
 TaxiSourceCaptureRecord pending,last,first[1024];uint64_t count;unsigned valid;
} source_taxi_capture;
static void source_taxi_capture_prepare(TaxiCameraView view,const TaxiHalfPlan *plan,
 TaxiPoint projected,const TaxiCameraOffsets *offsets,const TaxiOrdinaryFrameInput *input){
 if(!taxi_source_capture_path)return;
 TaxiSourceCaptureRecord *r=&source_taxi_capture.pending;memset(r,0,sizeof *r);
 r->taxi=car.world.height.position;r->projected=projected;r->view=view;r->input=*input;
 memcpy(r->centers,plan->centers,sizeof r->centers);
 r->distance_fixed=offsets->distance_770;r->source_distance_fixed=camera_offsets.distance_770;
 r->base=camera_base;r->heading=car.world.heading_1c;r->motion=car.world.heading_20;
 r->tracked_pitch=camera_pitch;r->tracked_heading=camera_heading;
 r->yaw_offset=camera_yaw_offset;r->yaw_target=camera_yaw_target;
 r->preparation_delta=source_viewer.owner.preparation_delta;
 r->prepared_counter=live_host_time.counter;r->root=source_restart.completed;
 r->city=(unsigned)scene_index+1;r->width=(unsigned)target_width;r->height=(unsigned)target_height;
 r->intro=live_game_start.active;r->fade=city_fade.active_177b;r->frozen=native_pause.menu.frozen;
 r->mode=live_camera_mode_64c;r->pullback=prototype_camera_framing?(unsigned)prototype_camera_pullback_units:0;
 source_taxi_capture.valid=1;
}
static void source_taxi_capture_publish(void){
 if(!taxi_source_capture_path)return;
 const TaxiOrdinaryPublishedFrame *p=&source_viewer.published;
 const TaxiOrdinaryFrameInput *a=&source_taxi_capture.pending.input,*b=&p->input;
 if(!source_taxi_capture.valid||!p->valid||a->frame!=b->frame||a->scale!=b->scale||
    a->driver!=b->driver||a->roll!=b->roll||a->mask!=b->mask||a->sine!=b->sine||a->cosine!=b->cosine||
    memcmp(a->left,b->left,sizeof a->left)||memcmp(a->right,b->right,sizeof a->right)||
    source_taxi_capture.pending.preparation_delta!=p->preparation_delta)
  fail("Source taxi capture does not match the completed publication.");
 TaxiSourceCaptureRecord r=source_taxi_capture.pending;
 r.generation=p->generation;r.published_counter=p->counter;
 source_taxi_capture.last=r;
 if(source_taxi_capture.count<1024)source_taxi_capture.first[source_taxi_capture.count]=r;
 source_taxi_capture.count++;source_taxi_capture.valid=0;taxi_source_capture_written=1;
}
static void source_taxi_capture_json(FILE *f,const TaxiSourceCaptureRecord *r){
 fprintf(f,"{\"source_frame\":true,\"viewport\":[%u,%u],\"recovered_camera\":true,\"owned_graphics\":true,\"alpha\":1,\"root\":%u,\"city\":%u,\"driver\":%u,\"generation\":%llu,\"prepared_counter\":%u,\"published_counter\":%u,\"preparation_delta\":%ld,",
  r->width,r->height,r->root,r->city,r->input.driver,(unsigned long long)r->generation,r->prepared_counter,r->published_counter,(long)r->preparation_delta);
 fprintf(f,"\"vehicle\":[%ld,%ld,%ld,%ld,%d],\"camera\":[%ld,%ld,%ld,%d,%d,%d],\"distance\":%ld,\"distance_fixed\":%ld,\"source_distance_fixed\":%ld,\"base\":%ld,\"frame\":%u,\"scale\":%u,\"depth\":%ld,\"centers\":[[%ld,%ld],[%ld,%ld]],",
  (long)r->taxi.x,(long)r->taxi.y,(long)r->taxi.z,(long)r->heading,(int)(int16_t)r->input.roll,
  (long)r->view.position.x,(long)r->view.position.y,(long)r->view.position.z,r->view.pitch,r->view.yaw,r->view.roll,
  (long)(r->distance_fixed/4096),(long)r->distance_fixed,(long)r->source_distance_fixed,(long)r->base,r->input.frame,r->input.scale,(long)r->projected.z,
  (long)r->centers[0][0],(long)r->centers[0][1],(long)r->centers[1][0],(long)r->centers[1][1]);
 fprintf(f,"\"camera_tracking\":[%ld,%ld,%ld,%ld],\"motion_heading\":%ld,\"camera_mode\":%u,\"intro\":%u,\"fade\":%u,\"frozen\":%u,\"prototype_pullback\":%u}",
  (long)r->tracked_pitch,(long)r->tracked_heading,(long)r->yaw_offset,(long)r->yaw_target,(long)r->motion,r->mode,r->intro,r->fade,r->frozen,r->pullback);
}
static void source_taxi_capture_finish(void){
 if(!taxi_source_capture_path||!source_frame_active)return;
 if(!source_taxi_capture.count)fail("No completed native taxi frame captured.");
 FILE *f=fopen(taxi_source_capture_path,"wb");if(!f)fail("Native taxi capture unavailable.");
 source_taxi_capture_json(f,&source_taxi_capture.last);fputc('\n',f);
 if(ferror(f)||fclose(f))fail("Native taxi capture write failed.");
 char path[MAX_PATH];if(snprintf(path,sizeof path,"%s.frames.json",taxi_source_capture_path)>=(int)sizeof path)fail("Native taxi frame capture path too long.");
 f=fopen(path,"wb");if(!f)fail("Native taxi frame sequence unavailable.");
 unsigned kept=source_taxi_capture.count<1024?(unsigned)source_taxi_capture.count:1024;
 fprintf(f,"{\"source_frame\":true,\"captured\":%llu,\"kept\":%u,\"dropped\":%llu,\"frames\":[\n",(unsigned long long)source_taxi_capture.count,kept,(unsigned long long)(source_taxi_capture.count-kept));
 for(unsigned i=0;i<kept;i++){if(i)fputs(",\n",f);source_taxi_capture_json(f,source_taxi_capture.first+i);}
 fputs("\n]}\n",f);if(ferror(f)||fclose(f))fail("Native taxi sequence write failed.");
 printf("NATIVE TAXI CAPTURE: %llu completed frames, first%u retained; last record and sequence written after gameplay.\n",(unsigned long long)source_taxi_capture.count,kept);
}
