#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include "material_shader.h"
#include "palette_fade.h"
static GLuint program;
static int material_active;
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
static int load(void *target,size_t size,const char *name) {
    PROC proc=wglGetProcAddress(name);
    if(!proc || (uintptr_t)proc<=3 || (uintptr_t)proc==UINTPTR_MAX || size!=sizeof proc)return 0;
    memcpy(target,&proc,size);return 1;
}
#define LOAD(field,name) if(!load(&field,sizeof field,name))return 0
static int initialize(void) {
    LOAD(create_shader,"glCreateShader");LOAD(shader_source,"glShaderSource");
    LOAD(compile_shader,"glCompileShader");LOAD(get_shader,"glGetShaderiv");
    LOAD(shader_log,"glGetShaderInfoLog");LOAD(create_program,"glCreateProgram");
    LOAD(attach_shader,"glAttachShader");LOAD(link_program,"glLinkProgram");
    LOAD(get_program,"glGetProgramiv");LOAD(use_program,"glUseProgram");
    LOAD(uniform_location,"glGetUniformLocation");LOAD(uniform_colors,"glUniform4fv");LOAD(uniform_int,"glUniform1i");
    LOAD(delete_shader,"glDeleteShader");LOAD(delete_program,"glDeleteProgram");
    const char *sources[2]={
        "#version 120\nvoid main(){gl_Position=ftransform();gl_TexCoord[0]=gl_MultiTexCoord0;}",
        "#version 120\nuniform vec4 colors[16];uniform int bitmapCoverage;void main(){"
        "float a=clamp(floor(gl_TexCoord[0].z),0.0,7.0);"
        "float parity=mod(floor(gl_TexCoord[0].x)+floor(gl_TexCoord[0].y),2.0);"
        "int index=int(a*2.0+parity);gl_FragColor=colors[index];if(bitmapCoverage==0)gl_FragColor.a=1.0;}"};
    GLuint shaders[2]={create_shader(0x8b31),create_shader(0x8b30)};
    for(unsigned i=0;i<2;i++) {
        shader_source(shaders[i],1,&sources[i],NULL);compile_shader(shaders[i]);
        GLint status=0;get_shader(shaders[i],0x8b81,&status);
        if(!status) {
            char log[1024]={0};shader_log(shaders[i],sizeof log-1,NULL,log);
            fprintf(stderr,"Native material shader: %s\n",log);
            delete_shader(shaders[0]);delete_shader(shaders[1]);return 0;
        }
    }
    GLuint candidate=create_program();attach_shader(candidate,shaders[0]);attach_shader(candidate,shaders[1]);
    link_program(candidate);GLint status=0;get_program(candidate,0x8b82,&status);
    delete_shader(shaders[0]);delete_shader(shaders[1]);
    if(!status){delete_program(candidate);return 0;}
    program=candidate;return 1;
}
int taxi_material_shader_begin(const uint8_t material[28],const uint16_t palette[256],int16_t cap) {
    if(!material||!palette||(!program&&!initialize()))return 0;
    GLfloat colors[64];
    for(unsigned i=0;i<16;i++) {
        uint16_t color=taxi_palette_fade_cap_rgb555(palette[material[i]],cap);
        for(unsigned c=0;c<3;c++)colors[i*4+c]=(GLfloat)((color>>(c*5))&31)/31;
        colors[i*4+3]=material[i]?1:0;
    }
    use_program(program);material_active=1;uniform_int(uniform_location(program,"bitmapCoverage"),0);uniform_colors(uniform_location(program,"colors[0]"),16,colors);return 1;
}
int taxi_material_shader_bitmap_coverage(int enabled){
    if(!program||!material_active||(enabled!=0&&enabled!=1))return 0;
    uniform_int(uniform_location(program,"bitmapCoverage"),enabled);return 1;
}
void taxi_material_shader_end(void){if(program)use_program(0);material_active=0;}
void taxi_material_shader_free(void){material_active=0;if(program){use_program(0);delete_program(program);program=0;}}
