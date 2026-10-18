/* Original rank HUD presentation. All mutable resources here are private
   renderer copies. Original retained HUD/fare owners remain read-only. */
/* Assign each destination pixel by its center, including exact scale ties.
   Constant source-texel centers avoid interpolator rounding at 13.5x. */
static void rank_quad(double x,double y,double w,double h,double u,double v,unsigned tw,unsigned th){
 unsigned cols=(unsigned)lround(u*tw),rows=(unsigned)lround(v*th);
 if(!cols||!rows)return;
 glBegin(GL_QUADS);
 for(unsigned row=0;row<rows;row++)for(unsigned col=0;col<cols;col++){
  double left=ceil(x+w*col/cols-0.5),right=ceil(x+w*(col+1)/cols-0.5);
  double top=ceil(y+h*row/rows-0.5),bottom=ceil(y+h*(row+1)/rows-0.5);
  glTexCoord2d((col+0.5)/tw,(row+0.5)/th);
  glVertex2d(left,top);glVertex2d(right,top);glVertex2d(right,bottom);glVertex2d(left,bottom);
 }
 glEnd();
}
static void draw_rank_plan(int width,int height,const TaxiRankHudPlan *p,unsigned language,int digits){
 if(!p||!p->active)return;
 if(language>=5)fail("Rank language outside original assets.");
 TaxiRankBitmapRect rect;if(taxi_rank_bitmap_rect(&rect,p->x))fail("Rank bitmap outside recovered animation domain.");
 double scale=height/160.0,offset=width-240*scale;
 glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_TEXTURE_BIT|GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
 glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_ALPHA_TEST);glEnable(GL_TEXTURE_2D);glColor4f(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,width,height,0,-1,1);
 glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 if(rect.width){
  uint8_t rgba[1152*4];
  for(unsigned i=0;i<1152;i++){
   uint16_t color=taxi_palette_fade_cap_rgb555(pause_scene_palette()[rank_label_indices[language][i]],(int16_t)city_fade_cap);
   for(unsigned c=0;c<3;c++)rgba[i*4+c]=(uint8_t)(((color>>(5*c))&31)*255/31);
   rgba[i*4+3]=255;
  }
  if(!rank_label_texture)glGenTextures(1,&rank_label_texture);
  glBindTexture(GL_TEXTURE_2D,rank_label_texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,36,32,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
  rank_quad(offset+rect.x*scale,rect.y*scale,rect.width*scale,rect.height*scale,rect.width/36.0,rect.height/32.0,36,32);
 }
 if(digits){
  glEnable(GL_ALPHA_TEST);glAlphaFunc(GL_GREATER,0.5f);
  for(unsigned i=0;i<2;i++){
   if(!cash_owned_slot_textures[19+i])fail("Rank digit texture was not published.");
   glBindTexture(GL_TEXTURE_2D,cash_owned_slot_textures[19+i]);
   rank_quad(offset+p->sprites[i].x*scale,p->sprites[i].y*scale,8*scale,16*scale,1,1,8,16);
  }
 }
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopAttrib();
}
static void draw_rank_panel(int width,int height){
 if(!passenger_diagnostic)return;
 TaxiRankHudPlan plan=rank_current_plan();
 draw_rank_plan(width,height,&plan,0,1);
}
static void rank_fixture_plan(TaxiRankHudPlan *out,int32_t x,uint16_t first,uint16_t second,unsigned language){
 TaxiRankHudSnapshot state={1,x*256,{first,second}};
 if(taxi_rank_hud_plan(out,&state,(uint8_t)language))fail("Rank rendering fixture rejected.");
}
static void rank_test_color(uint8_t out[4],uint8_t index){
 uint16_t color=taxi_palette_fade_cap_rgb555(pause_scene_palette()[index],(int16_t)city_fade_cap);
 for(unsigned c=0;c<3;c++)out[c]=(uint8_t)(((color>>(5*c))&31)*255/31);out[3]=255;
}
static void rank_check_pixels(const uint8_t *pixels,const uint8_t *expected,unsigned width,unsigned height){
 for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++)for(unsigned c=0;c<3;c++){
  size_t actual=((size_t)(height-1-y)*width+x)*4+c,want=((size_t)y*width+x)*4+c;
  if(abs((int)pixels[actual]-(int)expected[want])>1){
   fprintf(stderr,"Rank GPU mismatch %ux%u at%u,%u channel%u got%u expected%u\n",width,height,x,y,c,pixels[actual],expected[want]);fail("Rank GPU original-art pixel comparison failed.");
  }
 }
}
static void check_rank_hud_gpu(void){
 const char *work=getenv("TAXI_RANK_GPU_WORK");if(!work||!target_fbo||target_width<3840||target_height<2160)fail("Rank GPU check needs oracle workspace and native4K target.");
 char path[MAX_PATH];snprintf(path,sizeof path,"%s/bitmap-oracles.bin",work);FILE *f=fopen(path,"rb");uint32_t count=0;
 if(!f||fread(&count,4,1,f)!=1||count!=65)fail("Rank bitmap reference missing.");
 int saved_cap=city_fade_cap,was_bound=target_bound;GLint old_fbo,old_draw,old_read;
 glGetIntegerv(0x8ca6,&old_fbo);glGetIntegerv(GL_DRAW_BUFFER,&old_draw);glGetIntegerv(GL_READ_BUFFER,&old_read);
 TaxiRankDisplayOwner owner=native_rank_display_owner();uint8_t saved_scalar[32],saved_cache[256];memcpy(saved_scalar,owner.scalar_16c0,32);memcpy(saved_cache,owner.cache_5300,256);
 TaxiTitleAssets saved_assets=live_taxi_assets;TaxiPaletteBanks saved_banks=live_taxi_banks;TaxiPaletteRegistration saved_registry=live_taxi_registry;
 TaxiCashGlyphs saved_cash=cash_glyphs;TaxiPassengerScene saved_passengers=passenger_scene;
 glPushAttrib(GL_ALL_ATTRIB_BITS);target_begin();city_fade_cap=31;
 glDisable(GL_DEPTH_TEST);glDisable(GL_FOG);glDisable(GL_CULL_FACE);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glDisable(GL_DITHER);glViewport(0,0,240,160);
 uint8_t bg[4];rank_test_color(bg,111);glClearColor(bg[0]/255.0f,bg[1]/255.0f,bg[2]/255.0f,1);
 uint8_t original[38400],pixels[38400*4],expected[38400*4],digit_pixels[80][2][512];
 for(unsigned n=0;n<count;n++){
  int32_t a[2];if(fread(a,4,2,f)!=2||fread(original,1,sizeof original,f)!=sizeof original)fail("Truncated rank bitmap reference.");
  TaxiRankHudPlan plan;rank_fixture_plan(&plan,a[1],72,64,(unsigned)a[0]);
  glClear(GL_COLOR_BUFFER_BIT);draw_rank_plan(240,160,&plan,(unsigned)a[0],0);glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  for(unsigned i=0;i<38400;i++)rank_test_color(expected+i*4,original[i]);
  rank_check_pixels(pixels,expected,240,160);
 }
 if(fgetc(f)!=EOF||ferror(f)||fclose(f))fail("Rank bitmap reference length invalid.");
 snprintf(path,sizeof path,"%s/pixels.rgba",work);f=fopen(path,"rb");
 if(!f||fread(digit_pixels,1,sizeof digit_pixels,f)!=sizeof digit_pixels||fgetc(f)!=EOF||ferror(f)||fclose(f))fail("Rank digit reference missing.");
 for(unsigned row=0;row<80;row++){
  TaxiRankHudPlan plan;rank_fixture_plan(&plan,200,(uint16_t)row,(uint16_t)(79-row),0);
  cash_owned_textures(&cash_glyphs,&plan);
  glClear(GL_COLOR_BUFFER_BIT);draw_rank_plan(240,160,&plan,0,1);glFinish();glReadPixels(0,0,240,160,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
  for(unsigned i=0;i<38400;i++)memcpy(expected+i*4,bg,4);
  for(unsigned y=0;y<32;y++)for(unsigned x=0;x<36;x++)rank_test_color(expected+((88+y)*240+200+x)*4,rank_label_indices[0][y*36+x]);
  for(unsigned slot=0;slot<2;slot++)for(unsigned y=0;y<16;y++)for(unsigned x=0;x<8;x++){
   const uint8_t *p=digit_pixels[row][slot]+(y*8+x)*4;
   if(p[3])memcpy(expected+((100+y)*240+210+9*slot+x)*4,p,4);
  }
  rank_check_pixels(pixels,expected,240,160);
 }
 uint8_t *large=malloc((size_t)3840*2160*4),*want=malloc((size_t)3840*2160*4);if(!large||!want)fail("Rank4K pixel buffer unavailable.");
 const int positions[3]={200,202,234};
 for(unsigned test=0;test<3;test++){
  TaxiRankHudPlan plan;rank_fixture_plan(&plan,positions[test],72,7,0);cash_owned_textures(&cash_glyphs,&plan);
  glViewport(0,0,3840,2160);glClear(GL_COLOR_BUFFER_BIT);draw_rank_plan(3840,2160,&plan,0,1);glFinish();glReadPixels(0,0,3840,2160,GL_RGBA,GL_UNSIGNED_BYTE,large);
  TaxiRankBitmapRect rect;if(taxi_rank_bitmap_rect(&rect,positions[test]))fail("Rank4K reference rectangle invalid.");
  for(unsigned y=0;y<2160;y++)for(unsigned x=0;x<3840;x++){
   int sx=(int)floor(((double)x+0.5-600)/13.5),sy=(int)floor(((double)y+0.5)/13.5);uint8_t *p=want+((size_t)y*3840+x)*4;memcpy(p,bg,4);
   if(sx>=rect.x&&sx<rect.x+rect.width&&sy>=88&&sy<120)rank_test_color(p,rank_label_indices[0][(sy-88)*36+sx-rect.x]);
   for(unsigned slot=0;slot<2;slot++)if(sx>=plan.sprites[slot].x&&sx<plan.sprites[slot].x+8&&sy>=100&&sy<116){
    const uint8_t *d=digit_pixels[72][slot]+((sy-100)*8+sx-plan.sprites[slot].x)*4;if(d[3])memcpy(p,d,4);
   }
  }
  rank_check_pixels(large,want,3840,2160);
 }
 free(large);free(want);
 if(glGetError()!=GL_NO_ERROR)fail("Rank GPU rendering error.");
 if(memcmp(saved_scalar,owner.scalar_16c0,32)||memcmp(saved_cache,owner.cache_5300,256)||memcmp(&saved_assets,&live_taxi_assets,sizeof saved_assets)||memcmp(&saved_banks,&live_taxi_banks,sizeof saved_banks)||memcmp(&saved_registry,&live_taxi_registry,sizeof saved_registry)||memcmp(&saved_cash,&cash_glyphs,sizeof saved_cash)||memcmp(&saved_passengers,&passenger_scene,sizeof saved_passengers))fail("Rank rendering changed retained game owners.");
 city_fade_cap=saved_cap;target_bind_fbo(0x8d40,(GLuint)old_fbo);glDrawBuffer((GLenum)old_draw);glReadBuffer((GLenum)old_read);target_bound=was_bound;glPopAttrib();
 puts("PASS RANK GPU: 65 original ARM bitmap cases,80 combined original digit-row cases,3 native3840x2160 widescreen cases; retained HUD/game/graphics owners unchanged. Explicit unfaded native palette; controlled rendering fixtures, not earned fares.");
}
static void draw_rank_capture_fixture(int width,int height){
 TaxiRankHudPlan p;rank_fixture_plan(&p,200,56,23,0);cash_owned_textures(&cash_glyphs,&p);draw_rank_plan(width,height,&p,0,1);
 puts("RANK CAPTURE: controlled original-art rank7 with rolling second digit; no earned settlement claim.");
}

