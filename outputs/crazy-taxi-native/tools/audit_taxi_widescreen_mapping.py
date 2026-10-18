"""Audit current widescreen taxi mapping, not a whole-game framebuffer oracle."""
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');src=root/'src/scene_viewer.c';text=src.read_text();body=text.split('static int draw_source_taxi(int width,int height) {')[1].split('static void draw_taxi')[0]
for token in ['double pixel=(double)height/160;','double x=width*0.5+(projected.x-120)*pixel;','double y=projected.y*pixel;','double left=(field.left-projected.x)*pixel,top=(field.top-projected.y)*pixel;','double right=left+field.width*pixel,bottom=top+field.height*pixel;']:
 assert token in body,token
d=json.loads((root/'reports/ordinary-physics-startup.json').read_text());width,height=d['viewport'];pixel=height/160;left=width/2-120*pixel
cases=[]
for w,h in [(240,160),(1280,720),(3840,2160)]:
 scale=h/160;offset=w/2-120*scale
 assert 0*scale==0 and 160*scale==h
 assert 120*scale+offset==w/2
 assert scale==h/160 # same factor for both dimensions
 cases.append(dict(viewport=[w,h],uniform_scale=scale,original_view_rectangle=[offset,0,240*scale,h],logical_widescreen_width=w/scale,vertical_crop_from_mapping=0,centers=[[offset+x*scale,y*scale] for x,y in d['centers']]))
report=dict(status='PASS',cases=cases,scope='Current draw_source_taxi maps original vertical0..160 exactly to window0..height with uniform X/Y scale and horizontally centered original240width.16:9 adds side extent; mapping itself adds no vertical crop or stretch. Source sprite geometry may cross160 and be clipped at original viewport. Does not prove actual framebuffer/scene bootstrap, GBA framing intent, or all UI/world consumers.',hashes={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),src,root/'reports/ordinary-physics-startup.json']})
(root/'reports/taxi-widescreen-mapping-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
