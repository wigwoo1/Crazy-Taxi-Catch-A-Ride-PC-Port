exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
needle=struct.pack('<I',0x0800dc59)
start=0
while True:
 off=r.find(needle,start)
 if off<0:break
 lines.append(f'THUMB callback literal0800dc59 at ROM{off:06x}; data locator only')
 start=off+1
pathlib.Path('outputs/crazy-taxi-native/reports/attract-road-callback-literals.txt').write_text('\n'.join(lines));print('\n'.join(lines))
