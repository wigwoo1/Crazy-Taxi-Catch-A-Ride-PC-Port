/* Original startup group7. English/default language0 until settings language
   owner is connected. Separate immutable banks from ordinary loading/HUD. */
static void draw_game_start_art(int width,int height){
 if(!live_game_start.active||live_attract)return;
 static uint8_t initial[12288],banks[184320];static int loaded;
 if(!loaded){
  char path[MAX_PATH];GetModuleFileNameA(NULL,path,MAX_PATH);char *tail=strrchr(path,'\\');
  if(!tail)fail("Startup artwork path invalid.");
  const char *names[2]={"initial.rgba","frames.rgba"};void *data[2]={initial,banks};size_t sizes[2]={sizeof initial,sizeof banks};
  for(unsigned i=0;i<2;i++){
   snprintf(tail+1,MAX_PATH-(size_t)(tail+1-path),"../assets/start-phase/%s",names[i]);FILE *f=fopen(path,"rb");
   if(!f||fread(data[i],1,sizes[i],f)!=sizes[i]||fgetc(f)!=EOF)fail("Startup artwork source invalid.");fclose(f);
  }loaded=1;
 }
 int phase=live_game_start.phase;TaxiStartArtSubmission plan;
 if(taxi_start_art_submission(&plan,(uint8_t)live_game_start.active,(int16_t)phase,240,(int16_t)live_game_start.offset,(int16_t)live_game_start.scale))fail("Startup submission invalid.");
 if(!plan.count)return;int affine=(int)plan.affine;
 const uint8_t *source=native_start_art_owned_active?bound_initial_start_art():phase<=0?initial:banks+phase*5*12288;
 double pixel=height/160.0,left=(width-240*pixel)/2;
 glPushAttrib(GL_ENABLE_BIT|GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT);
 glDisable(GL_DEPTH_TEST);glEnable(GL_TEXTURE_2D);glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);glDisable(GL_BLEND);glColor4f(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
 glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 for(unsigned i=0;i<plan.count;i++){
  uint8_t rgba[4096];taxi_palette_fade_cap_rgba(source+(i%3)*4096,rgba,1024,(int16_t)city_fade_cap);
  GLuint texture;glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
  if(!affine){
   int x=plan.x[i];
   if(x<240&&x>-32)result_quad(texture,left+x*pixel,64*pixel,32*pixel,32*pixel);
  }else{
   int16_t matrix[4];taxi_obj_matrix_values(0,256,live_game_start.scale,matrix);
   int doubled=live_game_start.scale>256;int extent=32*(doubled+1);
   if(!taxi_obj_shader_frame_begin(matrix,doubled,32,32,32,32,0,0,0))fail("Startup affine shader unavailable.");
   int x=plan.x[i];
   double px=left+(x-extent/2)*pixel,py=(80-extent/2)*pixel;
   glBegin(GL_QUADS);
   glTexCoord2i(0,0);glVertex2d(px,py);
   glTexCoord2i(extent,0);glVertex2d(px+extent*pixel,py);
   glTexCoord2i(extent,extent);glVertex2d(px+extent*pixel,py+extent*pixel);
   glTexCoord2i(0,extent);glVertex2d(px,py+extent*pixel);glEnd();
   taxi_obj_shader_end();
  }
  glDeleteTextures(1,&texture);
 }
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}

