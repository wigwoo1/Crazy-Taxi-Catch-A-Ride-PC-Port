"""Exact ROM owner audit; no execution or renderer-equivalence claim."""
import hashlib,json,pathlib,struct,sys
sys.path.insert(0,"work/python-packages")
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
rom=pathlib.Path("Crazy Taxi - Catch a Ride (USA).gba").read_bytes()
assert hashlib.sha256(rom).hexdigest()=="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
def instruction(at):
 i=next(md.disasm(rom[at-0x08000000:at-0x08000000+4],at,count=1))
 return i.mnemonic,i.op_str
expected={0x0802C750:("ldr","r2, [pc, #0x2c]"),0x0802C752:("ldr","r4, [r2]"),0x0802C76C:("str","r0, [r4, #0x64]"),0x0802C834:("ldr","r1, [pc, #4]"),0x0802C836:("str","r0, [r1, #4]"),0x0802B404:("ldr","r4, [pc, #0x3c]"),0x0802B41A:("ldr","r2, [r4]"),0x0802B422:("ldr","r0, [r2, #0x40]"),0x0802B424:("str","r0, [r1]")}
for at,want in expected.items():assert instruction(at)==want,(hex(at),instruction(at),want)
for at in (0x2C780,0x2C83C,0x2B444):assert struct.unpack_from("<I",rom,at)[0]==0x03006810
assert instruction(0x0802AF02)==("movs","r0, #0")
assert instruction(0x0802AF04)==("bl","#0x802c834")
assert instruction(0x0802AF20)==("movs","r0, #0")
assert instruction(0x0802AF22)==("movs","r1, #0")
assert instruction(0x0802AF24)==("movs","r2, #0")
assert instruction(0x0802AF26)==("bl","#0x802c558")
report={"rom_sha256":hashlib.sha256(rom).hexdigest(),"verified_instructions":{f"{k:08X}":" ".join(v) for k,v in expected.items()},"palette_pattern_owner":"*(03006810)+0x64 (100 bytes)","bootstrap_transform_reset":"2AF02..2AF04 selects NULL before caller rebinds context+E18", "bootstrap_clear_color":"2AF20..2AF26 invokes2C558 with RGB(0,0,0)", "transform_pointer_owner":"03006814; stores caller pointer without copying transform", "different_field":"2B422 reads *(03006810)+0x40 (64 bytes) into global030068E8, not the palette pattern", "scope":"Exact instruction/literal assertions only. No reachable full-frame execution, palette consumer, native adapter or rendered fidelity proof."}
pathlib.Path("outputs/crazy-taxi-native/reports/restart-render-owner-audit.json").write_text(json.dumps(report,indent=2)+"\n")
print("PASS exact restart graphics owners; pattern100, transform pointer global+4, separate field64")
