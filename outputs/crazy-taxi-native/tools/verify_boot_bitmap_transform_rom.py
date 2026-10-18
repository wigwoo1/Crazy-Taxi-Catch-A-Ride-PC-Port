from pathlib import Path
import json,subprocess,hashlib
exec(Path('outputs/crazy-taxi-native/tools/audit_boot_whole_cold_source.py').read_text().split('\nresults=[]')[0])
results=[]
for seed in (0,165,255):
 p=ColdBootProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x04000000:bytearray([seed])*1024};p.scalars={};p.stores=[]
 p.reg[4]=256;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 initial=bytes(p.regions[0x04000000]);before=p.reg.copy()
 p.run(entry=0x0801e38a,stop=0x0801e3cc)
 expected=bytes(p.regions[0x04000000][0x20:0x40])
 assert bytes(p.regions[0x04000000][:0x20])==initial[:0x20] and bytes(p.regions[0x04000000][0x40:])==initial[0x40:]
 assert p.reg[4:15]==before[4:15]
 cp=subprocess.run([str(root/'bin/boot-bitmap-transform-compare.exe')],input=initial[0x20:0x40],capture_output=True)
 assert cp.returncode==0 and cp.stdout==expected,(seed,cp.returncode,cp.stdout.hex(),expected.hex())
 results.append({'seed':seed,'instructions':p.steps,'bytes':len(expected),'source_hex':expected.hex()})
report={'status':'NATIVE_ORIGINAL_BOOT_BITMAP_TRANSFORMS_PASS','cases':results,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'ActualROM1E38A..1E3CA source stores vs typednative BG2/BG3 transform initialization, 32bytes exact and otherIO unchanged, retained r4=256 qualified from priorboot source. Isolated prefix and existingwholecoldboot evidence; not new uninterruptedboot/frame/liveintegration or hardwaredisplay proof.'}
(root/'reports/boot-bitmap-transform-native-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
