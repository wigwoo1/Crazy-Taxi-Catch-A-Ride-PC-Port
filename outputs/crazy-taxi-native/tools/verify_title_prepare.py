"""Integration check of separately ROM-qualified title loading and prompt requests."""
from pathlib import Path
import subprocess,struct,json,hashlib
root=Path('outputs/crazy-taxi-native');rom='Crazy Taxi - Catch a Ride (USA).gba';r=Path(rom).read_bytes()
# Revalidate original27994 asset comparison before consuming its capture.
subprocess.run(['./',str(root/'tools/compare_title_assets_rom.py')],check=True)
base=(root/'reports/title-assets-native.bin').read_bytes()
requests=subprocess.run([str(root/'bin/title-prompt-setup-rom-compare.exe'),rom],capture_output=True,text=True,check=True).stdout.splitlines()
results=[]
for line in requests:
 v=line.split();language=int(v[0]);fields=list(map(int,v[1:16]));expected=bytearray(base[44:]);fonts=bytearray([165]*144)
 for slot,b,c,d in [(0,0,0x083fd3b4,3),(1,1,0x084097b8,1)]:
  struct.pack_into('<5I',fonts,slot*36,0,b,32,c,d);fonts[slot*36+32]=1
 for i in range(4):
  src,dst,control=fields[i*3:i*3+3];n=(control&65535)*4;dst-=0x6010000;src-=0x8000000
  expected[dst:dst+n]=r[src:src+n]
 struct.pack_into('<I',expected,32768+10*20+16,0x08290ad4)
 expected[32768+2560+512+320:32768+2560+512+352]=r[0x290ad4:0x290af4]
 expected+=fonts+struct.pack('<I',0)
 capture=root/f'reports/title-prepare-language-{language}.bin'
 subprocess.run([str(root/'bin/title-prepare-compare.exe'),rom,str(language),str(capture)],check=True)
 actual=capture.read_bytes();assert len(actual)==len(expected)
 assert actual==expected,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b)
 results.append({'language':language,'compared_bytes':len(actual)})
report={'status':'PASS','languages':results,'scope':'Native integration of separately-qualified title asset load, font descriptor configuration, prompt setup and logical transfers. Asset baseline rerun against original27994. Prompt request fixture has existing direct1630C..1638E ROM qualification. No whole title caller/fade/RNG/OAM/frame/parent verification.','native_sha256':hashlib.sha256((root/'bin/title-prepare-compare.exe').read_bytes()).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/title-prepare-integration-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS title preparation four languages',results)
