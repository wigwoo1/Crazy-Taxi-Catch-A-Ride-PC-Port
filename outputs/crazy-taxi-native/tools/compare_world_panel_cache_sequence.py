from pathlib import Path
import json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');j=json.loads((root/'reports/world-producer-cache-sequence-source.json').read_text());lines=[]
for fixture,base in j['cases']:
 city,refs,x,y,z,yaw,pitch=fixture;lines.append(' '.join(map(str,[city,len(refs),base,x,y,z,yaw,pitch,*refs]))+'\n')
r=subprocess.run([str(root/'bin/world-panel-cache-sequence.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw'),str(root/'assets/driving.ctr'),str(root/'assets/indexed-scenes/city1.cti'),str(root/'assets/indexed-scenes/city2.cti')],input=''.join(lines),text=True,capture_output=True,check=True);rows=r.stdout.splitlines();assert len(rows)==len(lines)
for case,result,row in zip(j['cases'],j['results'],rows):
 counts,sums=result;expected=[a for pair in zip(counts,sums) for a in pair];assert expected==list(map(int,row.split())),(case,expected,row)
report={'pass_match':True,'cases':len(rows),'frames':len(rows)*3,'source_instructions':j['actual_arm_instructions'],'scope':'Full original31EC sequenced persistentnormalcache vs actualnative destination_panel then world_frame_build optionaldualcache owners + generationadvance + queueconsume across preview-counter restore and following frame inclwrap. Exact counts/weightedcachechecksum, notbytewisecacheproof. Nativeheads/count cleared eachconsume. Source248copy/generationadvance/drawconsumerreset explicit priorqualified boundaries; no fullactual31860/maincaller scheduling or livecache enable claim.','sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_frame.c','src/world_frame.h','src/destination_panel.c','tests/world-panel-cache-sequence.c','tools/probe_world_producer_cache_sequence.py','tools/compare_world_panel_cache_sequence.py']}}
(root/'reports/world-panel-cache-sequence-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
