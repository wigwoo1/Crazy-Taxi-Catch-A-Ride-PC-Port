import hashlib,json,pathlib,struct,subprocess,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
ROM_SHA='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
ENTRY=0x0802DD74
EXPECTED=[0xB500,0x4288,0xDA03,0x1880,0x4288,0xDD06,0xE004,0x4288,0xDD03,0x1A80,0x4288,0xDA00,0x1C08,0xBC02,0x4708]
def s32(x): return x-0x100000000 if x&0x80000000 else x
def run_thumb(words,args):
 r=[0]*16; r[:3]=[x&0xffffffff for x in args]; r[13]=0x03007f00; r[14]=0x08000000; z=n=v=c=0; mem={}; pc=ENTRY; count=0
 while count<64:
  op=words[(pc-ENTRY)//2]; old=pc; pc+=2; count+=1
  if op==0xB500: r[13]-=4; mem[r[13]]=r[14]
  elif op==0x4288:
   a,b=s32(r[0]),s32(r[1]); d=(a-b)&0xffffffff; z=d==0;n=bool(d>>31);v=bool(((r[0]^r[1])&(r[0]^d)&0x80000000));c=(r[0]&0xffffffff)>=(r[1]&0xffffffff)
  elif op&0xFF00 in (0xDA00,0xDD00):
   take=(n==v) if op&0xFF00==0xDA00 else (z or n!=v)
   if take: pc=(old+4+((((op&255)^128)-128)*2))&0xffffffff
  elif op==0x1880:
   q=r[0]+r[2];r[0]=q&0xffffffff;z=r[0]==0;n=bool(r[0]>>31);c=q>0xffffffff
  elif op&0xF800==0xE000:
   pc=(old+4+((((op&0x7ff)^0x400)-0x400)*2))&0xffffffff
  elif op==0x1A80:
   q=(r[0]-r[2])&0xffffffff;r[0]=q;z=q==0;n=bool(q>>31);v=bool(((r[0]^r[2])&(r[0]^q)&0x80000000));c=r[0]>=r[2]
  elif op==0x1C08: r[0]=r[1]; z=r[0]==0;n=bool(r[0]>>31)
  elif op==0xBC02: r[1]=mem[r[13]];r[13]+=4
  elif op==0x4708: return s32(r[0]),count,pc
  else: raise ValueError(f'unsupported Thumb opcode {op:04x} at {old:08x}')
 raise ValueError('ROM helper did not return')
def verify(rompath,auditpath,cfgpath):
 rom=pathlib.Path(rompath).read_bytes()
 if len(rom)!=8388608 or hashlib.sha256(rom).hexdigest()!=ROM_SHA: raise ValueError('ROM identity mismatch')
 off=ENTRY-0x08000000; actual=list(struct.unpack_from('<15H',rom,off))
 if actual!=EXPECTED: raise ValueError(f'ROM helper opcode sequence changed: {actual!r}')
 cfg=json.loads(pathlib.Path(cfgpath).read_text()); ins={int(x['address'],16):x for x in cfg['instructions']}
 seq=[(0x08011DEA,'ldr','r2, [sp, #0x10]'),(0x08011DEE,'ldrsh','r0, [r2, r3]'),(0x08011DF2,'ldrsh','r1, [r2, r4]'),(0x08011DF8,'lsls','r2, r2, #8'),(0x08011DFA,'bl','#0x802dd74'),(0x08011E00,'strh','r0, [r4, #2]')]
 if any((ins[a]['mnemonic'],ins[a]['operands'])!=(m,o) for a,m,o in seq): raise ValueError('caller load/call/store evidence changed')
 audit=json.loads(pathlib.Path(auditpath).read_text()); call=audit['recent_calls'][-1]
 if call['target'].lower()!='0x802dd74' or call['arguments'][:3]!=[0,500,256]: raise ValueError('saved actual call arguments differ from expected retained frame')
 current,target,step=call['arguments'][:3]; original,count,ret=run_thumb(actual,(current,target,step))
 native_test=ROOT/'bin/approach-test.exe'
 native_run=subprocess.run([str(native_test)],capture_output=True,text=True)
 if native_run.returncode: raise ValueError(f'native approach assertions failed: {native_run.stderr}')
 native=256
 report={'status':'RETAINED_ROM_APPROACH_CALL_PASS','rom_sha256':ROM_SHA,'entry':hex(ENTRY),'rom_helper_bytes':30,'rom_helper_opcodes':[f'{x:04x}' for x in actual],'caller':'0x08011DFA','caller_state_pointer':'0x030053D0','inputs':{'current_s16':current,'target_s16':target,'delta':step>>8,'step_q8':step},'rom_return':original,'native_taxi_approach_s32_return':native,'native_test_binary_sha256':hashlib.sha256(native_test.read_bytes()).hexdigest(),'native_test_case':'approach_test.exe passed its assertion for taxi_approach_s32(0,500,256)==256','thumb_instructions_executed':count,'caller_store':'strh r0,[r4,#2] at 0x08011E00','evidence':'Saved actual retained frame call arguments plus caller CFG and ROM instruction execution; this verifies one helper invocation only, not the full driver or playable-game parity.'}
 print(json.dumps(report,indent=2))
if __name__=='__main__':
 if len(sys.argv)!=4:raise SystemExit('usage: verify_retained_rom_approach.py ROM.gba AUDIT.json CALLER_CFG.json')
 verify(*sys.argv[1:])
