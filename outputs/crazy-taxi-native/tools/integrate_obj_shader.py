import pathlib
p=pathlib.Path('outputs/crazy-taxi-native/src')
for name in ('obj_shader.c','obj_shader.h'):
 s=(p/name).read_text().replace('int doubled,unsigned half)','int doubled,unsigned half,int snapped)')
 if name.endswith('.c'):
  s=s.replace('uniform vec4 field;','uniform vec4 field;uniform int snapped;')
  s=s.replace('vec2 p=floor(gl_TexCoord[0].xy)-field.xy;','vec2 p=(snapped!=0?floor(gl_TexCoord[0].xy):gl_TexCoord[0].xy-vec2(0.5))-field.xy;')
  s=s.replace('uniform_int(uniform_location(program,"image"),0);','uniform_int(uniform_location(program,"image"),0);uniform_int(uniform_location(program,"snapped"),snapped);')
 (p/name).write_text(s)
s=(p/'scene_viewer.c').read_text().replace('taxi_obj_shader_begin(matrices[c],1,half)','taxi_obj_shader_begin(matrices[c],1,half,1)')
start=s.index('static int draw_source_taxi(');end=s.index('static void draw_taxi',start)
section=s[start:end]
section=section.replace('    double size=(scales[0]+(scales[1]-scales[0])*alpha)*pixel/2048;\n','')
section=section.replace('    glRotatef(-(float)presented_roll*360/16777216,0,0,1);\n','')
section=section.replace('    glBindTexture(GL_TEXTURE_2D,taxi_textures[frame]);glColor4f(1,1,1,1);glBegin(GL_QUADS);','''    int16_t obj_matrix[4];
    taxi_obj_matrix_values(runtime.trig.sine[obj_angle],runtime.trig.cosine[obj_angle],(int16_t)obj_scale,obj_matrix);
    glBindTexture(GL_TEXTURE_2D,taxi_textures[frame]);glColor4f(1,1,1,1);''')
a=section.index('        TaxiSpriteBounds b=');b=section.index('    glEnd();glEnable(GL_DEPTH_TEST)',a)
section=section[:a]+'''        TaxiObjField field;
        if(taxi_obj_field(obj_centers[half][0],obj_centers[half][1],64,64,
            (int16_t)obj_scale,(int16_t)obj_angle,&field))fail("Source taxi field invalid.");
        if(!taxi_obj_shader_begin(obj_matrix,field.double_size,half,0))fail("Native taxi affine shader unavailable.");
        double left=(field.left-projected.x)*pixel,top=(field.top-projected.y)*pixel;
        double right=left+field.width*pixel,bottom=top+field.height*pixel;
        glBegin(GL_QUADS);
        glTexCoord2i(0,0);glVertex2d(left,top);
        glTexCoord2i((int)field.width,0);glVertex2d(right,top);
        glTexCoord2i((int)field.width,(int)field.height);glVertex2d(right,bottom);
        glTexCoord2i(0,(int)field.height);glVertex2d(left,bottom);
        glEnd();taxi_obj_shader_end();
    }
''' + section[b:]
section=section.replace('    glEnd();glEnable(GL_DEPTH_TEST)','    glEnable(GL_DEPTH_TEST)')
s=s[:start]+section+s[end:];(p/'scene_viewer.c').write_text(s)
