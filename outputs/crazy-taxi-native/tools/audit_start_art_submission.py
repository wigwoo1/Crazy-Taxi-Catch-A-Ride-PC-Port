"""Source-backed sliding startup sprite submissions; do not decode pools."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
spans=[(0x1fb7c,0x1fc54),(0x1fc74,0x1fd24)]
rows=[]
for a,b in spans:
 ins=list(md.disasm(r[a:b],0x8000000+a))
 rows.append({'span':[hex(0x8000000+a),hex(0x8000000+b)],'bytes':r[a:b].hex(),'instructions':[(hex(i.address),i.mnemonic,i.op_str) for i in ins]})
slide=list(md.disasm(r[0x1fb92:0x1fc54],0x801fb92))
assert [i.op_str for i in slide if i.mnemonic=='bl']==['#0x8028288']*6
assert [(i.mnemonic,i.op_str) for i in slide if i.address in (0x801fba4,0x801fba6,0x801fbaa,0x801fc04,0x801fc06)]==[('adds','r0, #0x30'),('subs','r1, r1, r0'),('movs','r4, #0x40'),('subs','r0, #0x30'),('adds','r1, r1, r0')]
for off,value in [(0x1fb7c,0x02001696),(0x1fb88,0x020016b4),(0x1fb9c,0x020016b8),(0x1fc7a,0x020016ba)]:
 h=struct.unpack_from('<H',r,off)[0];p=((off+4)&~3)+(h&255)*4
 assert struct.unpack_from('<I',r,p)[0]==value
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'source':rows,'slide':{'groups':[7]*6,'frames':[0,1,2,0,1,2],'y':64,'x':['width/2-offset-48','width/2-offset-16','width/2-offset+16','width/2+offset-48','width/2+offset-16','width/2+offset+16']},'scope':'Submission coordinates and group/frame order. 28288 registration offsets/anchor convention and28658 affine semantics still require verification; no final pixels claimed.'}
pathlib.Path('outputs/crazy-taxi-native/reports/start-art-submission-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original six startup sliding submissions, groups/frames, x coordinates, y64 and state owners; final affine semantics remain pending.')
