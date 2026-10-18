from pathlib import Path
import json,struct,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';capt=reports/'active-held-input';ns={};source=(root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=self.stop:');exec(source,ns);ns['BASE']=0x03000008
regs=json.loads((capt/'next-dependency-state.json').read_text())['registers'];assert regs[4]==0x03004300 and regs[:4]==[4,0x02009efc,0x020034c8,0]
iw=bytearray((capt/'bound-first-frame-pre-consumer-owner-03000000.bin').read_bytes());ew=bytearray((capt/'bound-first-frame-pre-consumer-owner-02000000.bin').read_bytes());rom=ns['r'];assert iw[0x4300:0x44b4]==rom[0x49f8:0x4bac]
frame=struct.unpack_from('<I',iw,0x16cc)[0];bounds=list(struct.unpack_from('<4I',iw,0x16b8));assert bounds==[0,0,240,160] and frame in (0x06000000,0x0600a000)
q=ns['MarkerRasterProbe'].__new__(ns['MarkerRasterProbe']);q.blocks=[(0x02000000,ew),(frame,bytearray([173])*38400),(0x03000000,iw),(regs[13]-4096,bytearray(4096))];q.reg=regs.copy();q.entry=regs[4];q.stop=regs[14];q.steps=0;q.n,q.z,q.c,q.v=json.loads((capt/'next-dependency-state.json').read_text())['flags'];stores=[];write=q.write

def guarded(a,v,n=4):
 assert (regs[13]-188<=a and a+n<=regs[13]) or (frame<=a and a+n<=frame+38400),('unqualified-marker-write',hex(a),n,hex(q.reg[15]))
 stores.append([a,n]);write(a,v,n)
q.write=guarded;original=q.run();assert q.reg[4:15]==regs[4:15]
rect=[struct.unpack_from('<i',ew,regs[1]-0x02000000+o)[0] for o in (32,44,96,108)];w,h,log=ew[0x34c8:0x34cb];texture=struct.unpack_from('<I',ew,0x34cc)[0];assert texture>=0x08000000 and texture+w*h<=0x08800000
fixture=struct.pack('<7i',*rect,w,h,log)+rom[texture-0x08000000:texture-0x08000000+w*h];cp=subprocess.run([str(root/'bin/retained-marker-owner-compare.exe')],input=fixture,capture_output=True);assert cp.returncode==0,(cp.returncode,cp.stderr);assert original==cp.stdout,[(i,a,b) for i,(a,b) in enumerate(zip(original,cp.stdout)) if a!=b][:16]
joined=subprocess.run([str(root/'bin/retained-marker-owner-compare.exe'),'source'],input=fixture,capture_output=True);assert joined.returncode==0 and joined.stdout==original,(joined.returncode,joined.stderr);assert joined.stderr.decode().strip()=='callbacks=1 writes=593'
d=dict(status='NATIVE_RETAINED_RAM_BANNER_CALLBACK_PASS',rectangle=rect,descriptor=[w,h,log,texture],native_source_frame_join=True,original_instructions=q.steps,compared_bytes=38400,changed_pixels=sum(v!=173 for v in original),pixel_stores=sum(frame<=a<frame+38400 for a,n in stores),scope='Actual frame50 stopped callback arguments and original installed IWRAM/preconsumer EWRAM owners. Original49F8/clipping/divider instructions vs native indexed_billboard, isolated fill173 framebuffer; does not prove joined full frame or native banner ownership/gameplay.')
(reports/'retained-ram-banner-callback-comparison.json').write_text(json.dumps(d,indent=2)+'\n');print(json.dumps(d))
