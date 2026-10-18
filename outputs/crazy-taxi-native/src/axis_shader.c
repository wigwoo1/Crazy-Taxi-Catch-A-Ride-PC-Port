#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include "axis_shader.h"
#include "palette_fade.h"
static GLuint program,span_texture,index_texture,lookup_texture;
static GLint saved_index_binding,saved_lookup_binding;
static int lookup_active,lookup_cache_valid;
static uint8_t lookup_cache[2560];
static uint16_t palette_cache[256];
static int16_t cap_cache;
static GLint saved_active,saved_binding;
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
static void (APIENTRY *program_log)(GLuint,GLsizei,GLsizei *,char *);
static void (APIENTRY *use_program)(GLuint);
static GLint (APIENTRY *uniform_location)(GLuint,const char *);
static void (APIENTRY *uniform_int)(GLint,GLint);
static void (APIENTRY *uniform_float)(GLint,GLfloat);
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
    LOAD(get_program,"glGetProgramiv");LOAD(program_log,"glGetProgramInfoLog");LOAD(use_program,"glUseProgram");
    LOAD(uniform_location,"glGetUniformLocation");LOAD(active_texture,"glActiveTexture");
    LOAD(uniform_int,"glUniform1i");LOAD(uniform_float,"glUniform1f");
    LOAD(delete_shader,"glDeleteShader");LOAD(delete_program,"glDeleteProgram");
    const char *sources[2]={
        "#version 120\nvoid main(){gl_Position=ftransform();gl_TexCoord[0]=gl_MultiTexCoord0;}",
        "#version 120\nuniform sampler2D spans;"
        "uniform sampler2D image;uniform sampler2D indices;uniform sampler2D paletteLookup;uniform int shaded;uniform int bitmapCoverage;uniform int flatLow;uniform int flatHigh;uniform int columns;uniform int snapped;uniform int count;uniform float left;"
        "vec4 record(int i){return texture2D(spans,vec2((float(i)+0.5)/float(count),0.25));}"
        "vec2 uv(int i,float position){vec4 r=record(i);vec2 g=texture2D(spans,vec2((float(i)+0.5)/float(count),0.75)).xy;return r.yz+g*(position-r.x);}"
        "float shade(int i,float position){vec4 r=record(i);vec4 g=texture2D(spans,vec2((float(i)+0.5)/float(count),0.75));return g.z+g.w*(position-r.x);}"
        "void main(){vec2 p=gl_TexCoord[0].xy;float axis=columns!=0?(p.x-left-0.5)/2.0:p.y-0.5;"
        "float position=columns!=0?p.y-0.5:p.x-0.5;"
        "if(snapped!=0){axis=columns!=0?floor((p.x-left)/2.0):floor(p.y);"
        "position=columns!=0?floor(p.y):floor(p.x/2.0)*2.0;}"
        "int i=int(clamp(floor(axis),0.0,float(count-1)));"
        "vec4 spanRecord=record(i);if(spanRecord.w==0.0)discard;"
        "if(snapped!=0&&(position<spanRecord.x||position>=spanRecord.x+spanRecord.w*(columns!=0?1.0:2.0)))discard;"
        "if(snapped==0){float factor=columns!=0?1.0:2.0;vec2 bounds=vec2(spanRecord.x,spanRecord.x+spanRecord.w*factor);"
        "if(i+1<count&&record(i+1).w!=0.0){vec4 n=record(i+1);bounds=mix(bounds,vec2(n.x,n.x+n.w*factor),fract(max(axis,0.0)));}"
        "if(position<bounds.x-0.5||position>=bounds.y-0.5)discard;}"
        "vec2 t=uv(i,position);"
        "if(snapped==0&&i+1<count&&record(i+1).w!=0.0){"
        "vec2 next=uv(i+1,position);vec2 difference=next-t;"
        "difference-=floor(difference+0.5);t+=difference*fract(max(axis,0.0));}"
        "if(shaded!=0){float index=shaded==2?(mod(floor(p.x)+floor(p.y),2.0)==0.0?float(flatLow):float(flatHigh)):floor(texture2D(indices,t).r*255.0+0.5);if(shaded!=2&&index==0.0)discard;"
        "float s=shade(i,position);if(snapped==0&&i+1<count&&record(i+1).w!=0.0)s=mix(s,shade(i+1,position),fract(max(axis,0.0)));"
        "float bank=clamp(floor(s),0.0,9.0);gl_FragColor=texture2D(paletteLookup,vec2((index+(shaded==3?mod(floor(p.x),2.0)*256.0:0.0)+0.5)/512.0,(bank+0.5)/10.0));if(bitmapCoverage==0)gl_FragColor.a=1.0;}"
        "else{vec4 color=texture2D(image,t);if(color.a<0.5)discard;gl_FragColor=color;}}"};
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
    if(!status){char log[2048]={0};program_log(candidate,sizeof log-1,NULL,log);fprintf(stderr,"Axis shader link: %s\n",log);delete_program(candidate);return 0;}
    program=candidate;return 1;
}
int taxi_axis_shader_begin(const TaxiIndexedSpan *spans,unsigned count,int columns,
    int left,unsigned width,unsigned height,int snapped) {
    if(!spans||!count||count>1024||!width||!height||(!program&&!initialize()))return 0;
    GLfloat data[8192]={0};GLfloat *records=data,*gradients=data+count*4;
    for(unsigned i=0;i<count;i++) {
        int32_t du,dv,shade,dshade;memcpy(&du,&spans[i].du,4);memcpy(&dv,&spans[i].dv,4);
        memcpy(&shade,&spans[i].shade,4);memcpy(&dshade,&spans[i].dshade,4);
        records[i*4]=(GLfloat)spans[i].x;
        records[i*4+1]=(GLfloat)(spans[i].u&((width<<14)-1))/(width*16384.0f);
        records[i*4+2]=(GLfloat)(spans[i].v&((height<<14)-1))/(height*16384.0f);
        records[i*4+3]=(GLfloat)spans[i].pairs;
        gradients[i*4]=(GLfloat)du/(width*(columns?16384.0f:32768.0f));
        gradients[i*4+1]=(GLfloat)dv/(height*(columns?16384.0f:32768.0f));
        gradients[i*4+2]=(GLfloat)shade/4096.0f;gradients[i*4+3]=(GLfloat)dshade/(columns?4096.0f:8192.0f);
    }
    glGetIntegerv(0x84e0,&saved_active);active_texture(0x84c1);
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&saved_binding);
    if(!span_texture)glGenTextures(1,&span_texture);glBindTexture(GL_TEXTURE_2D,span_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,0x812f);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,0x812f);
    glTexImage2D(GL_TEXTURE_2D,0,0x8814,(GLsizei)count,2,0,GL_RGBA,GL_FLOAT,data);
    active_texture((GLenum)saved_active);
    if(glGetError()!=GL_NO_ERROR){active_texture(0x84c1);glBindTexture(GL_TEXTURE_2D,(GLuint)saved_binding);active_texture((GLenum)saved_active);return 0;}
    use_program(program);uniform_int(uniform_location(program,"spans"),1);
    lookup_active=0;uniform_int(uniform_location(program,"shaded"),0);uniform_int(uniform_location(program,"bitmapCoverage"),0);
    uniform_int(uniform_location(program,"image"),0);uniform_int(uniform_location(program,"count"),(GLint)count);
    uniform_int(uniform_location(program,"columns"),columns);uniform_int(uniform_location(program,"snapped"),snapped);
    uniform_float(uniform_location(program,"left"),(GLfloat)left);return 1;
}
static void table_parameters(int repeat) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,repeat?GL_REPEAT:0x812f);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,repeat?GL_REPEAT:0x812f);
}
int taxi_axis_shader_lookup(const TaxiIndexedTexture *texture,const uint8_t lookup[2560],
    const uint16_t palette[256],int16_t cap) {
    if(!program||!texture||!lookup||!palette||!texture->pixels||!texture->width||!texture->height||
       texture->width>128||texture->height>128||texture->size<(size_t)texture->width*texture->height||lookup_active)return 0;
    int changed=!lookup_cache_valid||cap_cache!=cap||
        memcmp(lookup_cache,lookup,sizeof lookup_cache)||
        memcmp(palette_cache,palette,sizeof palette_cache);
    uint8_t rgba[20480];
    if(changed)for(unsigned bank=0;bank<10;bank++)for(unsigned i=0;i<256;i++)for(unsigned side=0;side<2;side++) {
        unsigned next=(i&~3u)|((i+1u)&3u);
        unsigned index=side?(unsigned)(lookup[bank*256+i]|lookup[bank*256+next]):lookup[bank*256+i];
        uint16_t color=taxi_palette_fade_cap_rgb555(palette[index],cap);
        unsigned offset=(bank*512+side*256+i)*4;
        for(unsigned c=0;c<3;c++)rgba[offset+c]=(uint8_t)(((color>>(c*5))&31)*255/31);
        rgba[offset+3]=index?255:0;
    }
    active_texture(0x84c2);glGetIntegerv(GL_TEXTURE_BINDING_2D,&saved_index_binding);
    if(!index_texture)glGenTextures(1,&index_texture);glBindTexture(GL_TEXTURE_2D,index_texture);table_parameters(1);
    GLint alignment;glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE8,(GLsizei)texture->width,(GLsizei)texture->height,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,texture->pixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);
    active_texture(0x84c3);glGetIntegerv(GL_TEXTURE_BINDING_2D,&saved_lookup_binding);
    if(!lookup_texture)glGenTextures(1,&lookup_texture);glBindTexture(GL_TEXTURE_2D,lookup_texture);table_parameters(0);
    /* Reuse only byte-identical lookup/palette/cap inputs. Original palette
       animation, fade changes and diagnostic mutations always invalidate. */
    if(changed)glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,10,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
    lookup_active=1;active_texture((GLenum)saved_active);
    if(glGetError()!=GL_NO_ERROR){lookup_cache_valid=0;return 0;}
    if(changed){memcpy(lookup_cache,lookup,sizeof lookup_cache);memcpy(palette_cache,palette,sizeof palette_cache);cap_cache=cap;lookup_cache_valid=1;}
    uniform_int(uniform_location(program,"indices"),2);uniform_int(uniform_location(program,"paletteLookup"),3);
    uniform_int(uniform_location(program,"shaded"),1);return 1;
}
int taxi_axis_shader_bitmap_coverage(int enabled){
    if(!program||!lookup_active||(enabled!=0&&enabled!=1))return 0;
    uniform_int(uniform_location(program,"bitmapCoverage"),enabled);return 1;
}
int taxi_axis_shader_flat(uint8_t low,uint8_t high) {
    if(!program||!lookup_active)return 0;
    uniform_int(uniform_location(program,"flatLow"),low);uniform_int(uniform_location(program,"flatHigh"),high);
    uniform_int(uniform_location(program,"shaded"),2);return 1;
}
int taxi_axis_shader_special(void) {
    if(!program||!lookup_active)return 0;
    uniform_int(uniform_location(program,"shaded"),3);return 1;
}
void taxi_axis_shader_end(void){if(program){use_program(0);if(lookup_active){active_texture(0x84c2);glBindTexture(GL_TEXTURE_2D,(GLuint)saved_index_binding);active_texture(0x84c3);glBindTexture(GL_TEXTURE_2D,(GLuint)saved_lookup_binding);lookup_active=0;}active_texture(0x84c1);glBindTexture(GL_TEXTURE_2D,(GLuint)saved_binding);active_texture((GLenum)saved_active);}}
void taxi_axis_shader_free(void){lookup_cache_valid=0;if(program){use_program(0);delete_program(program);program=0;}if(span_texture){glDeleteTextures(1,&span_texture);span_texture=0;}if(index_texture){glDeleteTextures(1,&index_texture);index_texture=0;}if(lookup_texture){glDeleteTextures(1,&lookup_texture);lookup_texture=0;}}
