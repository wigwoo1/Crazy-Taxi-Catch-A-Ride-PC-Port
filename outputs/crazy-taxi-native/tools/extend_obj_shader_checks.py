import pathlib
p=pathlib.Path('outputs/crazy-taxi-native/src/scene_viewer.c')
s=p.read_text()
s=s.replace('taxi_material_shader_free();taxi_axis_shader_free();','taxi_obj_shader_free();taxi_material_shader_free();taxi_axis_shader_free();')
a=s.index('static void check_native_obj_shader(void) {');b=s.index('static void check_native_taxi_half_overlap',a)
f=s[a:b]
f=f.replace('matrices[3][4]={{255,0,0,255},{0,255,-255,0},{128,0,0,128}}','matrices[4][4]={{255,0,0,255},{0,255,-255,0},{128,0,0,128},{181,181,-181,181}}')
f=f.replace('readback[128*128*4]','readback[256*256*4]')
f=f.replace('glBindTexture(GL_TEXTURE_2D,taxi_textures[0]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,art);','')
f=f.replace('glViewport(0,0,128,128);','')
f=f.replace('for(unsigned c=0;c<3;c++)for(unsigned half=0;half<2;half++){','''for(unsigned view=0;view<64;view++){
    glBindTexture(GL_TEXTURE_2D,taxi_textures[view]);glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,art);
    for(unsigned mode=0;mode<2;mode++){
    int dimension=mode?256:128;glViewport(0,0,dimension,dimension);
    for(unsigned c=0;c<4;c++)for(unsigned half=0;half<2;half++){''')
f=f.replace('taxi_obj_shader_begin(matrices[c],1,half,1)','taxi_obj_shader_begin(matrices[c],1,half,!mode)')
f=f.replace('glReadPixels(0,0,128,128','glReadPixels(0,0,dimension,dimension')
f=f.replace('y<128;y++)for(int x=0;x<128','y<dimension;y++)for(int x=0;x<dimension')
f=f.replace('if(taxi_obj_affine_point(matrices[c],64,64,1,x,y,&u,&v)==1){','''int valid;
            if(!mode)valid=taxi_obj_affine_point(matrices[c],64,64,1,x,y,&u,&v);
            else{
                /* Independent exact quarter-pixel arithmetic for 2x native coverage. */
                int dx=2*x-257,dy=2*y-257;
                int nu=matrices[c][0]*dx+matrices[c][1]*dy+32768;
                int nv=matrices[c][2]*dx+matrices[c][3]*dy+32768;
                u=nu>=0?nu/1024:-((-nu+1023)/1024);
                v=nv>=0?nv/1024:-((-nv+1023)/1024);
                valid=u>=0&&u<64&&v>=0&&v<64;
            }
            if(valid){''')
f=f.replace('((127-y)*128+x)','((dimension-1-y)*dimension+x)')
f=f.replace('    glEnable(GL_DEPTH_TEST);','    }}\n    glEnable(GL_DEPTH_TEST);')
f=f.replace('both actual taxi halves, scale257/512 and quarter turn; diagnostic only.','all64 actual taxi views/both halves, four matrices, snapped and continuous2x coordinates; diagnostic only.')
s=s[:a]+f+s[b:];p.write_text(s)
q=pathlib.Path('outputs/crazy-taxi-native/src/obj_shader.c')
s=q.read_text().replace('half>1||','half>1||(snapped!=0&&snapped!=1)||').replace('Native material shader:','Native OBJ shader:')
q.write_text(s)
