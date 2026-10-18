/* Original Ball frames, registered course palette, source projection and
   production queue consumer. Controlled poses, not playable course behavior. */
static void ball_read(const char *relative,void *out,size_t bytes){
 char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);if(!tail)fail("Ball asset path failed.");
 snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/%s",relative);
 FILE *f=fopen(path,"rb");if(!f||fread(out,1,bytes,f)!=bytes||fgetc(f)!=EOF)fail("Ball asset invalid.");if(fclose(f))fail("Ball asset close failed.");
}
typedef struct {uint8_t *pixels;const uint8_t *atlas;unsigned count;} BallCourseCpu;
static int ball_course_cpu(void *data,unsigned kind,size_t payload){
 BallCourseCpu *c=data;if(kind!=TAXI_RENDER_BALL||payload>=20)return -1;
 const TaxiBallProjection *p=queued_balls+payload;
 /* Same original even-pair empty-raster bounds as GPU consumer. */
 int left=p->bounds.left,right=p->bounds.right,top=p->bounds.top,bottom=p->bounds.bottom;
 if(p->packet_flags&4){if(left<0)left=0;if(right>240)right=240;}
 if(left>=right||top>=160||bottom<=0||((left&~1)+1>=right))return 0;
 if(taxi_challenge_ball_indexed(c->pixels,p,c->atlas)!=1)return -1;c->count++;return 0;
}
static void check_ball_gpu(int width,int height){
 uint8_t atlas[8192],raw[512],oracle[38400],actual[153600];uint16_t palette[256];
 ball_read("challenge-ball/balloon.indices",atlas,sizeof atlas);ball_read("challenge-worlds/challenge2-palette.rgb555",raw,sizeof raw);
 for(unsigned i=0;i<256;i++)palette[i]=(uint16_t)(raw[i*2]|((unsigned)raw[i*2+1]<<8));
 uint8_t used[256]={0};for(unsigned i=0;i<8192;i++)used[atlas[i]]=1;
 unsigned backdrop=255;while(used[backdrop]){if(!backdrop)fail("Ball backdrop unavailable.");backdrop--;}
 TaxiRuntimeAsset ball_runtime;char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,92);if(!tail)fail("Ball runtime path failed.");
 snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/driving.ctr");if(taxi_runtime_asset_load(&ball_runtime,path))fail("Ball runtime invalid.");
 glPushAttrib(GL_ALL_ATTRIB_BITS);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
 glGenTextures(8,ball_textures);for(unsigned frame=0;frame<8;frame++){
  glBindTexture(GL_TEXTURE_2D,ball_textures[frame]);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 }
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
 glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();int saved_snapped=native_axis_snapped_check;native_axis_snapped_check=1;
 const TaxiPoint poses[]={{0,0,4096},{-2450,0,4096},{2450,0,4096},{0,-1700,4096},{0,1400,4096},{0,0,2048}};
 const int16_t caps[]={0,7,31};unsigned checked=0,opaque=0,cases=0,clipped=0;
 for(unsigned c=0;c<3;c++){
  ball_palette_upload(atlas,palette,caps[c]);
  for(unsigned pose=0;pose<sizeof poses/sizeof poses[0];pose++)for(unsigned frame=0;frame<8;frame++){
   TaxiBallRenderRequest r={poses[pose].x,poses[pose].y,poses[pose].z,2178048,0x0849b0c9u+frame*1024};
   int projected=taxi_challenge_ball_project(&r,(TaxiPoint){0},0,0,160,106,&ball_runtime.trig,queued_balls);
   if(projected<0)fail("Ball original projection invalid.");if(!projected)continue;
   clipped+=(queued_balls[0].packet_flags&4)!=0;cases++;
   memset(oracle,(int)backdrop,sizeof oracle);if(taxi_challenge_ball_indexed(oracle,queued_balls,atlas)!=1)fail("Ball CPU indexed oracle failed.");
   glClearColor(17/255.0f,19/255.0f,23/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
   TaxiRenderPacket storage[1];TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,1);QueueDraw draw_context={.scale=1};
   if(taxi_render_queue_push(&queue,queued_balls[0].bucket,TAXI_RENDER_BALL,0)||taxi_render_queue_consume(&queue,draw_queue_packet,&draw_context))fail("Ball GPU production queue failed.");
   glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,actual);
   for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
    unsigned index=oracle[y*240+x];const unsigned background[]={17,19,23};uint16_t color=taxi_palette_fade_cap_rgb555(palette[index],caps[c]);
    for(unsigned k=0;k<3;k++){
     unsigned expected=index==backdrop?background[k]:((color>>(5*k))&31)*255/31;
     if(abs((int)actual[((159-y)*240+x)*4+k]-(int)expected)>2){fprintf(stderr,"Ball GPU mismatch frame%u pose%u cap%d x%u y%u index%u channel%u expected%u actual%u\n",frame,pose,caps[c],x,y,index,k,expected,actual[((159-y)*240+x)*4+k]);fail("Ball indexed GPU differs from native source raster.");}
    }
    checked++;opaque+=index!=backdrop;
   }
  }
 }

 /* Source rectangle callback legitimately writes no pixels for empty
    even-aligned raster bounds; do not turn that into a queue failure. */
 const TaxiTrafficScreenBounds empty_bounds[]={{120,40,121,80},{-10,40,-2,80},{100,160,140,180},{100,-20,140,0}};
 for(unsigned n=0;n<sizeof empty_bounds/sizeof empty_bounds[0];n++){
  queued_balls[0]=(TaxiBallProjection){.bounds=empty_bounds[n],.packet_flags=12,.bitmap_identity=0x0849b0c9u};
  QueueDraw context={.scale=1};if(draw_queue_packet(&context,TAXI_RENDER_BALL,0))fail("Ball no-raster packet failed.");
 }
 puts("PASS Ball no-raster GPU packets: width1/even alignment, outsideleft and both vertical edges return without drawing.");
 /* Continuous4K is a PC adaptation. Validate original palette contribution,
    transparent backdrop preservation and projected coverage, not ROM pixels. */
 native_axis_snapped_check=0;ball_palette_upload(atlas,palette,31);
 size_t native_bytes=(size_t)width*height*4;uint8_t *native_pixels=malloc(native_bytes);if(!native_pixels)fail("Ball4K allocation failed.");
 unsigned char allowed[32768]={0};
 for(unsigned i=0;i<8192;i++)if(atlas[i])allowed[palette[atlas[i]]&32767]=1;
 unsigned continuous_visible=0;
 for(unsigned frame=0;frame<8;frame++){
  TaxiBallRenderRequest request={0,0,4096,2178048,0x0849b0c9u+frame*1024};
  if(taxi_challenge_ball_project(&request,(TaxiPoint){0},0,0,160,106,&ball_runtime.trig,queued_balls)!=1)fail("Continuous Ball projection failed.");
  glClearColor(17/255.0f,19/255.0f,23/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);
  float pixel=(float)height/160,offset=((float)width-240*pixel)/2;
  QueueDraw draw_context={.scale=pixel,.offset=offset};TaxiRenderPacket storage[1];TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,1);
  if(taxi_render_queue_push(&queue,queued_balls[0].bucket,TAXI_RENDER_BALL,0)||taxi_render_queue_consume(&queue,draw_queue_packet,&draw_context))fail("Continuous Ball queue failed.");
  glFinish();glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,native_pixels);unsigned visible=0,retained=0;
  for(int y=0;y<height;y++)for(int x=0;x<width;x++){
   const uint8_t *color=native_pixels+((size_t)y*width+x)*4;
   if(color[0]==17&&color[1]==19&&color[2]==23){retained++;continue;}
   unsigned rgb=((unsigned)color[0]*31+127)/255|(((unsigned)color[1]*31+127)/255)<<5|(((unsigned)color[2]*31+127)/255)<<10;
   if(!allowed[rgb])fail("Continuous Ball invented a source palette color.");
   float sx=(x-offset)/pixel,sy=(height-1-y)/pixel;
   if(sx<queued_balls[0].bounds.left-2||sx>queued_balls[0].bounds.right+2||sy<queued_balls[0].bounds.top-1||sy>queued_balls[0].bounds.bottom+1)fail("Continuous Ball changed pixels outside projected coverage.");
   visible++;
  }
  if(!visible||!retained)fail("Continuous Ball lacks original-art/transparency witnesses.");continuous_visible+=visible;
 }

 /* Actual recovered20actor course, controlled camera per actor. This uses
    native ground/culling/animation traversal, not invented sprite poses. */
 TaxiChallengeBallCourse course={0};TaxiPassengerState actors[20]={0};
 GetModuleFileNameA(NULL,path,MAX_PATH);tail=strrchr(path,92);if(!tail)fail("Ball course path failed.");
 snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets");
 if(taxi_challenge_ball_course_load(&course,path)||taxi_challenge_ball_course_actors(&course,actors)!=1)fail("Ball actualcourse actors failed.");
 native_axis_snapped_check=1;ball_palette_upload(course.balloon_indices,course.resources.palette,31);
 unsigned course_pixels=0,course_packets=0,course_samples=0;
 for(unsigned sample=0;sample<24;sample++){
  unsigned target=sample<20?sample:0;TaxiPoint player;memcpy(&player,actors[target].bytes+16,12);
  TaxiPoint camera={player.x,player.y,player.z-4000};
  if(sample>=20){actors[0].bytes[10]=1;actors[0].bytes[8]=(uint8_t)(sample-20);actors[0].bytes[9]=15;actors[0].bytes[12]=15;actors[0].bytes[13]=0;}
  uint32_t visible=0;
  if(taxi_challenge_ball_course_render(&course,actors,player,camera,0,0,160,106,1,0,0,&ball_runtime.trig,queued_balls,&visible)||!(visible&(1u<<target)))fail("Ball actualcourse projection failed.");
  TaxiRenderPacket gpu_storage[20],cpu_storage[20];TaxiRenderQueue gpu_queue,cpu_queue;
  taxi_render_queue_init(&gpu_queue,gpu_storage,20);taxi_render_queue_init(&cpu_queue,cpu_storage,20);
  for(unsigned i=0;i<20;i++)if(visible&(1u<<i)){
   if(taxi_render_queue_push(&gpu_queue,queued_balls[i].bucket,TAXI_RENDER_BALL,i)||taxi_render_queue_push(&cpu_queue,queued_balls[i].bucket,TAXI_RENDER_BALL,i))fail("Ball actualcourse queue overflow.");
  }
  memset(oracle,(int)backdrop,sizeof oracle);BallCourseCpu cpu={oracle,course.balloon_indices,0};
  if(taxi_render_queue_consume(&cpu_queue,ball_course_cpu,&cpu))fail("Ball actualcourse CPU queue failed.");
  glClearColor(17/255.0f,19/255.0f,23/255.0f,1);glClear(GL_COLOR_BUFFER_BIT);QueueDraw draw_context={.scale=1};
  if(taxi_render_queue_consume(&gpu_queue,draw_queue_packet,&draw_context))fail("Ball actualcourse GPU queue failed.");
  glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,actual);
  for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
   unsigned index=oracle[y*240+x];const unsigned background[]={17,19,23};uint16_t color=course.resources.palette[index];
   for(unsigned k=0;k<3;k++){
    unsigned expected=index==backdrop?background[k]:((color>>(5*k))&31)*255/31;
    if(abs((int)actual[((159-y)*240+x)*4+k]-(int)expected)>2)fail("Ball actualcourse GPU differs from native queue raster.");
   }
   course_pixels++;
  }
  course_packets+=cpu.count;course_samples++;
 }
 taxi_challenge_ball_course_free(&course);
 printf("PASS actualBallcourse20 actors/nativeground/depthqueue: %u controlled camera/burst samples, %u source packets, %u full-frame compared pixels. No livecourse/GBA execution fidelity claim.\n",course_samples,course_packets,course_pixels);
 free(native_pixels);
 printf("PASS continuous Ball %dx%d: all8 original frames, %u palette-qualified visible pixels, projected bounds and untouched keyed backdrop; PC interpolation diagnostic, not running GBA pixel oracle.\n",width,height,continuous_visible);
 if(!opaque||!clipped)fail("Ball GPU check lacks visible/clipped witnesses.");
 native_axis_snapped_check=saved_snapped;glDeleteTextures(8,ball_textures);memset(ball_textures,0,sizeof ball_textures);
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();if(glGetError()!=GL_NO_ERROR)fail("Ball GPU state error.");
 printf("PASS Ball production GPU queue: %u cases, all8 ROM frames, registered course palette caps0/7/31, %u clipped cases, %u full-frame pixels (%u opaque). Snapped native raster comparison inside%dx%d; live course and continuous4K fidelity pending.\n",cases,clipped,checked,opaque,width,height);
}
