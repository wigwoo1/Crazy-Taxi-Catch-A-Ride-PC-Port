/* GPU verification uses recovered assets and independent signed Q8 sampling. */
static void check_native_effect_shader(void){
 const int16_t matrices[4][4]={{256,0,0,256},{128,0,0,128},{0,255,-255,0},{181,181,-181,181}};
 uint8_t readback[128*128*4];unsigned checked=0,frames=0;
 GLint viewport[4],bound;GLfloat clear[4];GLuint texture;
 glGetIntegerv(GL_VIEWPORT,viewport);glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
 glGetFloatv(GL_COLOR_CLEAR_VALUE,clear);glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();glDisable(GL_DEPTH_TEST);
 for(unsigned container=0;container<2;container++)for(unsigned group=8;group<=10;group++)
 for(unsigned frame=0;frame<(group==8?4:group==9?3:9);frame++)for(unsigned bank=0;bank<(group==9?2:1);bank++){
  TaxiEffectFrame art;if(!taxi_effect_asset_frame(&native_effect_asset,container,group,frame,bank,&art))fail("Effect GPU fixture missing.");
  glBindTexture(GL_TEXTURE_2D,texture);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,art.width,art.height,0,GL_RGBA,GL_UNSIGNED_BYTE,art.rgba);frames++;
  for(unsigned doubled=0;doubled<2;doubled++)for(unsigned zoom=1;zoom<=2;zoom++)for(unsigned matrix=0;matrix<4;matrix++){
   unsigned fw=art.width*(doubled+1),fh=art.height*(doubled+1),vw=fw*zoom,vh=fh*zoom;
   glViewport(0,0,(GLsizei)vw,(GLsizei)vh);glClearColor(1,0,1,0);glClear(GL_COLOR_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,fw,fh,0,-1,1);glMatrixMode(GL_MODELVIEW);
   if(!taxi_obj_shader_frame_begin(matrices[matrix],(int)doubled,art.width,art.height,art.width,art.height,0,0,zoom==1))fail("Effect GPU shader unavailable.");
   glBegin(GL_QUADS);glTexCoord2i(0,0);glVertex2i(0,0);glTexCoord2i((int)fw,0);glVertex2i((int)fw,0);
   glTexCoord2i((int)fw,(int)fh);glVertex2i((int)fw,(int)fh);glTexCoord2i(0,(int)fh);glVertex2i(0,(int)fh);glEnd();taxi_obj_shader_end();
   glFinish();glReadPixels(0,0,(GLsizei)vw,(GLsizei)vh,GL_RGBA,GL_UNSIGNED_BYTE,readback);
   for(unsigned y=0;y<vh;y++)for(unsigned x=0;x<vw;x++){
    int u,v,valid;const uint8_t background[3]={255,0,255};const uint8_t *expected=background;
    if(zoom==1)valid=taxi_obj_affine_point(matrices[matrix],art.width,art.height,(int)doubled,(int)x,(int)y,&u,&v);
    else{
     int dx=2*(int)x-1-2*(int)fw,dy=2*(int)y-1-2*(int)fh;
     int nu=matrices[matrix][0]*dx+matrices[matrix][1]*dy+(int)(art.width/2)*1024;
     int nv=matrices[matrix][2]*dx+matrices[matrix][3]*dy+(int)(art.height/2)*1024;
     u=nu>=0?nu/1024:-((-nu+1023)/1024);v=nv>=0?nv/1024:-((-nv+1023)/1024);
     valid=u>=0&&v>=0&&u<art.width&&v<art.height;
    }
    if(valid){const uint8_t *pixel=art.rgba+4*((unsigned)v*art.width+(unsigned)u);if(pixel[3])expected=pixel;}
    if(memcmp(readback+4*((vh-1-y)*vw+x),expected,3))fail("Effect GPU texel mismatch.");checked++;
   }
  }
 }
 glEnable(GL_DEPTH_TEST);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
 glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);glBindTexture(GL_TEXTURE_2D,(GLuint)bound);glDeleteTextures(1,&texture);
 glClearColor(clear[0],clear[1],clear[2],clear[3]);if(glGetError()!=GL_NO_ERROR)fail("Effect GPU validation error.");
 printf("PASS effect GPU shader: %u original frames/banks, %u field pixels; four matrices, both field sizes, native and2x sampling. Diagnostic only.\n",frames,checked);
}