import pathlib
p=pathlib.Path('outputs/crazy-taxi-native/src')
s=(p/'material_shader.c').read_text()
s=s.replace('#include "material_shader.h"','#include "obj_shader.h"').replace('#include "palette_fade.h"','')
s=s.replace('static GLuint program;','static GLuint program;\nstatic GLint saved_program;')
s=s.replace('static void (APIENTRY *uniform_colors)(GLint,GLsizei,const GLfloat *);','static void (APIENTRY *uniform_colors)(GLint,GLsizei,const GLfloat *);\nstatic void (APIENTRY *uniform_int)(GLint,GLint);')
s=s.replace('LOAD(uniform_location,"glGetUniformLocation");LOAD(uniform_colors,"glUniform4fv");','LOAD(uniform_location,"glGetUniformLocation");LOAD(uniform_colors,"glUniform4fv");LOAD(uniform_int,"glUniform1i");')
start=s.index('    const char *sources[2]=')
end=s.index('    GLuint shaders[2]',start)
s=s[:start]+r'''    const char *sources[2]={
        "#version 120\nvoid main(){gl_Position=ftransform();gl_TexCoord[0]=gl_MultiTexCoord0;}",
        "#version 120\nuniform sampler2D image;uniform vec4 matrix;uniform vec4 field;"
        "void main(){vec2 p=floor(gl_TexCoord[0].xy)-field.xy;"
        "vec2 source=floor(vec2(matrix.x*p.x+matrix.y*p.y,matrix.z*p.x+matrix.w*p.y)/256.0+32.0);"
        "if(source.x<0.0||source.y<0.0||source.x>=64.0||source.y>=64.0)discard;"
        "vec4 color=texture2D(image,vec2((source.x+field.z+0.5)/128.0,(source.y+0.5)/64.0));"
        "if(color.a<0.5)discard;gl_FragColor=color;}"};
'''.replace('\\n','\n')+s[end:]
start=s.index('int taxi_material_shader_begin(')
s=s[:start]+'''int taxi_obj_shader_begin(const int16_t matrix[4],int doubled,unsigned half) {
    if(!matrix||(doubled!=0&&doubled!=1)||half>1||(!program&&!initialize()))return 0;
    GLfloat coefficients[4],field[4]={(GLfloat)(32*(doubled+1)),(GLfloat)(32*(doubled+1)),(GLfloat)(64*half),0};
    for(unsigned i=0;i<4;i++)coefficients[i]=matrix[i];
    glGetIntegerv(0x8b8d,&saved_program);use_program(program);
    uniform_int(uniform_location(program,"image"),0);
    uniform_colors(uniform_location(program,"matrix"),1,coefficients);
    uniform_colors(uniform_location(program,"field"),1,field);return 1;
}
void taxi_obj_shader_end(void){if(program)use_program((GLuint)saved_program);}
void taxi_obj_shader_free(void){if(program){delete_program(program);program=0;}}
'''
(p/'obj_shader.c').write_text(s)
(p/'obj_shader.h').write_text('''#ifndef TAXI_OBJ_SHADER_H
#define TAXI_OBJ_SHADER_H
#include <stdint.h>
/* Source field-local coordinates via glTexCoord2; Q8 inverse map.
   Requires taxi128x64 art bound on unit0. Source-resolution sampling,
   native rasterization; full OBJ ordering belongs to caller. */
int taxi_obj_shader_begin(const int16_t matrix[4],int doubled,unsigned half);
void taxi_obj_shader_end(void);
void taxi_obj_shader_free(void);
#endif
''')
b=pathlib.Path('outputs/crazy-taxi-native/build.ps1')
s=b.read_text().replace("(Join-Path $PSScriptRoot 'src/obj_order.c')","(Join-Path $PSScriptRoot 'src/obj_shader.c') (Join-Path $PSScriptRoot 'src/obj_order.c')")
b.write_text(s)
