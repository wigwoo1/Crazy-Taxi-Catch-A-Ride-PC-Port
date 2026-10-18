/* Controlled projection diagnostic through production queue consumer.
   Embedded bitmap palette is qualified preview, not live palette ownership. */
static void flag_read(const char *name,void *out,size_t size){
 char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');if(!tail)fail("Flag asset path failed.");
 snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/challenge-flag/%s",name);
 FILE *f=fopen(path,"rb");if(!f||fread(out,1,size,f)!=size||fgetc(f)!=EOF)fail("Flag asset invalid.");fclose(f);
}
static void check_flag_gpu(int width,int height){
 uint8_t indices[2048],rgba[8192],palette[512],oracle[38400],actual[153600];
 flag_read("flag.indices",indices,sizeof indices);flag_read("flag.rgba",rgba,sizeof rgba);flag_read("flag.palette",palette,sizeof palette);
 glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
 glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
 glGenTextures(1,&flag_texture);glBindTexture(GL_TEXTURE_2D,flag_texture);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,64,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
 glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();native_axis_snapped_check=1;
 uint8_t actor[44]={1};actor[16]=0xda;actor[17]=0x56;actor[18]=1;actor[24]=0xfd;actor[25]=actor[26]=actor[27]=255;
 TaxiRuntimeAsset flag_runtime;char runtime_path[MAX_PATH];GetModuleFileNameA(NULL,runtime_path,MAX_PATH);
 char *runtime_tail=strrchr(runtime_path,'\\');if(!runtime_tail)fail("Flag runtime path failed.");
 snprintf(runtime_tail+1,MAX_PATH-(size_t)(runtime_tail+1-runtime_path),"../assets/driving.ctr");
 if(taxi_runtime_asset_load(&flag_runtime,runtime_path))fail("Flag original runtime load failed.");
 uint8_t course_palette[512];char course_path[MAX_PATH];GetModuleFileNameA(NULL,course_path,MAX_PATH);
 char *course_tail=strrchr(course_path,'\\');if(!course_tail)fail("Flag course palette path failed.");
 snprintf(course_tail+1,MAX_PATH-(size_t)(course_tail+1-course_path),"../assets/challenge-worlds/challenge1-palette.rgb555");
 FILE *course_file=fopen(course_path,"rb");if(!course_file||fread(course_palette,1,512,course_file)!=512||fgetc(course_file)!=EOF)fail("Flag course palette invalid.");fclose(course_file);
 unsigned compared=0,opaque=0;
 for(unsigned palette_case=0;palette_case<2;palette_case++){
  if(palette_case)memcpy(palette,course_palette,sizeof palette);
  uint16_t registered[256];for(unsigned i=0;i<256;i++)registered[i]=(uint16_t)(palette[i*2]|((unsigned)palette[i*2+1]<<8));
  flag_palette_upload(indices,registered,31);
 for(int scale=16;scale<=256;scale++){
  TaxiChallengeFlagRenderRequest request;
  if(taxi_challenge_flag_render_request(actor,87770,-3,735,(int16_t)scale,0,&request)!=1||taxi_challenge_flag_project(&request,(TaxiPoint){87770,735,-4003},0,0,160,106,&flag_runtime.trig,&queued_flag)!=1)fail("Flag GPU source projection failed.");
  memset(oracle,0,sizeof oracle);if(taxi_challenge_flag_indexed(oracle,&queued_flag,indices)!=1)fail("Flag GPU oracle failed.");
  glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
  TaxiRenderPacket storage[1];TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,1);
  QueueDraw context={.scale=1};if(taxi_render_queue_push(&queue,queued_flag.bucket,TAXI_RENDER_FLAG,0)||taxi_render_queue_consume(&queue,draw_queue_packet,&context))fail("Flag GPU queue failed.");
  glFinish();glReadPixels(0,height-160,240,160,GL_RGBA,GL_UNSIGNED_BYTE,actual);
  for(int y=1;y<159;y++)for(int x=1;x<239;x++){
   unsigned index=oracle[y*240+x];int interior=1;
   for(int dy=-1;dy<=1;dy++)for(int dx=-1;dx<=1;dx++)if(oracle[(y+dy)*240+x+dx]!=index)interior=0;
   if(!interior)continue;
   unsigned color=index?(unsigned)palette[index*2]|((unsigned)palette[index*2+1]<<8):0;
   for(unsigned c=0;c<3;c++)if(abs((int)actual[((159-y)*240+x)*4+c]-(int)(((color>>(5*c))&31)*255/31))>2){fprintf(stderr,"Flag GPU mismatch scale%d x%d y%d index%u channel%u\n",scale,x,y,index,c);fail("Flag GPU/native raster mismatch.");}
   compared++;if(index)opaque++;
  }
 }
 }
 if(!opaque)fail("Flag GPU check lacked opaque coverage.");
 native_axis_snapped_check=0;glDeleteTextures(1,&flag_texture);flag_texture=0;
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
 printf("PASS Flag production GPU queue:482 original shrink/rotation and palette cases, %u interior pixels (%u opaque), embedded and registered course palettes; snapped source diagnostic inside %dx%d buffer, live session/palette still pending.\n",compared,opaque,width,height);
}
