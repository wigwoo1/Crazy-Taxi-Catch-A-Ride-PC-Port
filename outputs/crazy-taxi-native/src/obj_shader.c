#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include "obj_shader.h"
static GLuint program;
static GLint saved_program,saved_active,saved_background_binding;
static int active_owner,background_bound;
static void (APIENTRY *active_texture)(GLenum);
static GLuint (APIENTRY *create_shader)(GLenum);
static void (APIENTRY *shader_source)(GLuint,GLsizei,const char **,const GLint *);
static void (APIENTRY *compile_shader)(GLuint);
static void (APIENTRY *get_shader)(GLuint,GLenum,GLint *);
static void (APIENTRY *shader_log)(GLuint,GLsizei,GLsizei *,char *);
static GLuint (APIENTRY *create_program)(void);
static void (APIENTRY *attach_shader)(GLuint,GLuint);
static void (APIENTRY *link_program)(GLuint);
static void (APIENTRY *get_program)(GLuint,GLenum,GLint *);
static void (APIENTRY *use_program)(GLuint);
static GLint (APIENTRY *uniform_location)(GLuint,const char *);
static void (APIENTRY *uniform_colors)(GLint,GLsizei,const GLfloat *);
static void (APIENTRY *uniform_int)(GLint,GLint);
static void (APIENTRY *delete_shader)(GLuint);
static void (APIENTRY *delete_program)(GLuint);
static int load(void *target,size_t size,const char *name){
 PROC proc=wglGetProcAddress(name);
 if(!proc||(uintptr_t)proc<=3||(uintptr_t)proc==UINTPTR_MAX||size!=sizeof proc)return 0;
 memcpy(target,&proc,size);return 1;
}
#define LOAD(field,name) if(!load(&field,sizeof field,name))return 0
static int initialize(void){
 LOAD(active_texture,"glActiveTexture");LOAD(create_shader,"glCreateShader");LOAD(shader_source,"glShaderSource");
 LOAD(compile_shader,"glCompileShader");LOAD(get_shader,"glGetShaderiv");
 LOAD(shader_log,"glGetShaderInfoLog");LOAD(create_program,"glCreateProgram");
 LOAD(attach_shader,"glAttachShader");LOAD(link_program,"glLinkProgram");
 LOAD(get_program,"glGetProgramiv");LOAD(use_program,"glUseProgram");
 LOAD(uniform_location,"glGetUniformLocation");LOAD(uniform_colors,"glUniform4fv");LOAD(uniform_int,"glUniform1i");
 LOAD(delete_shader,"glDeleteShader");LOAD(delete_program,"glDeleteProgram");
 const char *sources[2]={
 "#version 120\nvoid main(){gl_Position=ftransform();gl_TexCoord[0]=gl_MultiTexCoord0;}",
 "#version 120\nuniform sampler2D image;uniform vec4 matrix;uniform vec4 field;uniform vec4 dimensions;uniform int snapped;uniform int semi;uniform sampler2D background;uniform vec4 blendConfig;uniform int bgEligible;uniform int backdropEligible;"
 "void main(){vec2 p=(snapped!=0?floor(gl_TexCoord[0].xy):gl_TexCoord[0].xy-vec2(0.5))-field.xy;"
 "vec2 source=floor(vec2(matrix.x*p.x+matrix.y*p.y,matrix.z*p.x+matrix.w*p.y)/256.0+dimensions.xy/2.0);"
 "if(source.x<0.0||source.y<0.0||source.x>=dimensions.x||source.y>=dimensions.y)discard;"
 "vec4 color=texture2D(image,(source+field.zw+vec2(0.5))/dimensions.zw);"
 "if(color.a<0.5)discard;if(semi!=0){vec4 bg=texture2D(background,gl_FragCoord.xy/blendConfig.xy);"
 "if((bg.a>=0.5&&bgEligible!=0)||(bg.a<0.5&&backdropEligible!=0)){vec3 a=floor(color.rgb*31.0+0.5),b=floor(bg.rgb*31.0+0.5);"
 "vec3 rgb=min(vec3(31.0),floor((a*blendConfig.z+b*blendConfig.w)/16.0));color.rgb=floor(rgb*255.0/31.0)/255.0;}color.a=1.0;}gl_FragColor=color;}"};
 GLuint shaders[2]={create_shader(0x8b31),create_shader(0x8b30)};
 for(unsigned i=0;i<2;i++){
  shader_source(shaders[i],1,&sources[i],NULL);compile_shader(shaders[i]);
  GLint status=0;get_shader(shaders[i],0x8b81,&status);
  if(!status){char log[1024]={0};shader_log(shaders[i],sizeof log-1,NULL,log);
   fprintf(stderr,"Native OBJ shader: %s\n",log);delete_shader(shaders[0]);delete_shader(shaders[1]);return 0;}
 }
 GLuint candidate=create_program();attach_shader(candidate,shaders[0]);attach_shader(candidate,shaders[1]);
 link_program(candidate);GLint status=0;get_program(candidate,0x8b82,&status);
 delete_shader(shaders[0]);delete_shader(shaders[1]);
 if(!status){delete_program(candidate);return 0;}program=candidate;return 1;
}
int taxi_obj_shader_begin(const int16_t matrix[4],int doubled,unsigned half,int snapped){
 if(half>1)return 0;
 return taxi_obj_shader_frame_begin(matrix,doubled,64,64,128,64,64*half,0,snapped);
}
int taxi_obj_shader_frame_begin(const int16_t matrix[4],int doubled,unsigned width,
 unsigned height,unsigned texture_width,unsigned texture_height,
 unsigned offset_x,unsigned offset_y,int snapped){
 if(!matrix||(doubled!=0&&doubled!=1)||(snapped!=0&&snapped!=1)||
  width<8||width>64||height<8||height>64||(width&(width-1))||(height&(height-1))||
  texture_width<width||texture_height<height||offset_x>texture_width-width||offset_y>texture_height-height||
  (!program&&!initialize()))return 0;
 GLfloat coefficients[4],field[4]={(GLfloat)(width/2*(doubled+1)),(GLfloat)(height/2*(doubled+1)),(GLfloat)offset_x,(GLfloat)offset_y};
 GLfloat dimensions[4]={(GLfloat)width,(GLfloat)height,(GLfloat)texture_width,(GLfloat)texture_height};
 for(unsigned i=0;i<4;i++)coefficients[i]=matrix[i];
 glGetIntegerv(0x8b8d,&saved_program);use_program(program);
 active_owner=1;background_bound=0;uniform_int(uniform_location(program,"semi"),0);
 uniform_int(uniform_location(program,"image"),0);uniform_int(uniform_location(program,"snapped"),snapped);
 uniform_colors(uniform_location(program,"matrix"),1,coefficients);
 uniform_colors(uniform_location(program,"field"),1,field);
 uniform_colors(uniform_location(program,"dimensions"),1,dimensions);return 1;
}
int taxi_obj_shader_background(unsigned texture,unsigned width,unsigned height,uint16_t bldcnt,uint16_t bldalpha){
 if(!active_owner||background_bound||!texture||!width||!height)return 0;
 glGetIntegerv(0x84e0,&saved_active);active_texture(0x84c1);
 glGetIntegerv(GL_TEXTURE_BINDING_2D,&saved_background_binding);
 glBindTexture(GL_TEXTURE_2D,texture);active_texture((GLenum)saved_active);background_bound=1;
 unsigned eva=bldalpha&31,evb=(bldalpha>>8)&31;if(eva>16)eva=16;if(evb>16)evb=16;
 GLfloat config[4]={(GLfloat)width,(GLfloat)height,(GLfloat)eva,(GLfloat)evb};
 uniform_colors(uniform_location(program,"blendConfig"),1,config);
 uniform_int(uniform_location(program,"background"),1);
 uniform_int(uniform_location(program,"bgEligible"),(bldcnt>>10)&1);
 uniform_int(uniform_location(program,"backdropEligible"),(bldcnt>>13)&1);
 uniform_int(uniform_location(program,"semi"),1);return 1;
}
void taxi_obj_shader_end(void){
 if(!active_owner)return;
 if(background_bound){active_texture(0x84c1);glBindTexture(GL_TEXTURE_2D,(GLuint)saved_background_binding);active_texture((GLenum)saved_active);}
 active_owner=0;background_bound=0;if(program)use_program((GLuint)saved_program);
}
void taxi_obj_shader_free(void){taxi_obj_shader_end();if(program){delete_program(program);program=0;}}